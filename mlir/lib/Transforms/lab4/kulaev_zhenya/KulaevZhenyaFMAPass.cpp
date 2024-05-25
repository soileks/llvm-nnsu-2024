#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Tools/Plugins/PassPlugin.h"

using namespace mlir;

namespace {
class KulaevZhenyaFMAPass
    : public PassWrapper<KulaevZhenyaFMAPass, OperationPass<ModuleOp>> {
public:
  StringRef getArgument() const final { return "kulaev_zhenya_fma"; }
  StringRef getDescription() const final {
    return "The pass replaces the addition and multiplication operations of a "
           "single instruction";
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    module.walk([&](Operation *op) {
      if (auto addOp = dyn_cast<LLVM::FAddOp>(op)) {
        Value addLHS = addOp.getOperand(0);
        Value addRHS = addOp.getOperand(1);

        if (auto mulOpLHS = addLHS.getDefiningOp<LLVM::FMulOp>()) {
          handleMulOp(addOp, mulOpLHS, addRHS);
        } else if (auto mulOpRHS = addRHS.getDefiningOp<LLVM::FMulOp>()) {
          handleMulOp(addOp, mulOpRHS, addLHS);
        }
      }
    });

    module.walk([&](Operation *op) {
      if (auto mulOp = dyn_cast<LLVM::FMulOp>(op)) {
        if (mulOp.use_empty()) {
          mulOp.erase();
        }
      }
    });
  }

private:
  void handleMulOp(LLVM::FAddOp &addOp, LLVM::FMulOp &mulOp,
                   Value &otherOperand) {
    OpBuilder builder(addOp);
    Value fma = builder.create<LLVM::FMAOp>(addOp.getLoc(), mulOp.getOperand(0),
                                            mulOp.getOperand(1), otherOperand);
    addOp.replaceAllUsesWith(fma);
    if (mulOp.getOperand(0).hasOneUse()) {
      mulOp.erase();
    }
    addOp.erase();
  }
};
} // namespace

MLIR_DECLARE_EXPLICIT_TYPE_ID(KulaevZhenyaFMAPass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(KulaevZhenyaFMAPass)

PassPluginLibraryInfo getKulaevZhenyaFMAPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "kulaev_zhenya_fma", LLVM_VERSION_STRING,
          []() { PassRegistration<KulaevZhenyaFMAPass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo mlirGetPassPluginInfo() {
  return getKulaevZhenyaFMAPassPluginInfo();
}
