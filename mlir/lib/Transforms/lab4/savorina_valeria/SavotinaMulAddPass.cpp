#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Tools/Plugins/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;

namespace {
class SavotinaMulAddPass
    : public PassWrapper<SavotinaMulAddPass, OperationPass<ModuleOp>> {
public:
  StringRef getArgument() const final { return "SavotinaMulAddPass"; }
  StringRef getDescription() const final { return "fma pass"; }

  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<mlir::math::MathDialect>();
  }

  void runOnOperation() override {
    mlir::ModuleOp module = getOperation();
    mlir::OpBuilder builder(module);

    auto replaceAndEraseOp = [&](mlir::LLVM::FMulOp &mulOp,
                                 mlir::LLVM::FAddOp &addOp,
                                 mlir::Value &thirdOperand) -> void {
      builder.setInsertionPoint(addOp);
      mlir::Value fmaResult =
          builder.create<mlir::math::FmaOp>(addOp.getLoc(), mulOp.getOperand(0),
                                            mulOp.getOperand(1), thirdOperand);
      addOp.replaceAllUsesWith(fmaResult);
      addOp.erase();
      mulOp.erase();
    };

    module.walk([&](mlir::Operation *op) {
      if (auto addOp = llvm::dyn_cast<mlir::LLVM::FAddOp>(op)) {
        mlir::Value addLhs = addOp.getOperand(0);
        mlir::Value addRhs = addOp.getOperand(1);

        if (!addLhs.getType().isa<mlir::FloatType>() ||
            !addRhs.getType().isa<mlir::FloatType>()) {
          return;
        }

        auto isSingleUse = [&](mlir::Value value, mlir::Operation *userOp) {
          for (auto &use : value.getUses()) {
            if (use.getOwner() != userOp) {
              return false;
            }
          }
          return true;
        };

        if (auto mulOp = addLhs.getDefiningOp<mlir::LLVM::FMulOp>()) {
          if (isSingleUse(mulOp->getResult(0), addOp)) {
            replaceAndEraseOp(mulOp, addOp, addRhs);
          }
        } else if (auto mulOp = addRhs.getDefiningOp<mlir::LLVM::FMulOp>()) {
          if (isSingleUse(mulOp->getResult(0), addOp)) {
            replaceAndEraseOp(mulOp, addOp, addLhs);
          }
        }
      }
    });
  }
};
} // namespace

MLIR_DECLARE_EXPLICIT_TYPE_ID(SavotinaMulAddPass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(SavotinaMulAddPass)

mlir::PassPluginLibraryInfo getFunctionCallCounterPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "SavotinaMulAddPass", "0.1",
          []() { mlir::PassRegistration<SavotinaMulAddPass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK mlir::PassPluginLibraryInfo
mlirGetPassPluginInfo() {
  return getFunctionCallCounterPassPluginInfo();
}
