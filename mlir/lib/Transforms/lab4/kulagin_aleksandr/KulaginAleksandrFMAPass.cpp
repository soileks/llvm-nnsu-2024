#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassRegistry.h"
#include "mlir/Support/TypeID.h"
#include "mlir/Tools/Plugins/PassPlugin.h"

namespace {
class KulaginAleksandrFMAPass
    : public mlir::PassWrapper<KulaginAleksandrFMAPass,
                               mlir::OperationPass<mlir::ModuleOp>> {
public:
  static const char *pluginName;
  static const char *pluginVersion;
  mlir::StringRef getArgument() const final { return pluginName; }
  mlir::StringRef getDescription() const final {
    return "A pass that merges multiplication and addition into a single "
           "math.fma";
  }
  void runOnOperation() override {
    mlir::ModuleOp module = getOperation();
    module.walk([this](mlir::Operation *op) {
      if (auto addOp = mlir::dyn_cast<mlir::LLVM::FAddOp>(op)) {
        mlir::Value lhv = addOp.getOperand(0);
        mlir::Value rhv = addOp.getOperand(1);
        if (auto defOpLH = lhv.getDefiningOp<mlir::LLVM::FMulOp>()) {
          if (defOpLH.getResult().hasOneUse())
            buildFMAOp(addOp, defOpLH, defOpLH.getOperand(0),
                       defOpLH.getOperand(1), rhv);
        } else if (auto defOpRH = rhv.getDefiningOp<mlir::LLVM::FMulOp>()) {
          if (defOpRH.getResult().hasOneUse())
            buildFMAOp(addOp, defOpRH, defOpRH.getOperand(0),
                       defOpRH.getOperand(1), lhv);
        }
      } else if (auto subOp = mlir::dyn_cast<mlir::LLVM::FSubOp>(op)) {
        mlir::Value lhv = subOp.getOperand(0);
        mlir::Value rhv = subOp.getOperand(1);
        if (auto defOpLH = lhv.getDefiningOp<mlir::LLVM::FMulOp>()) {
          if (defOpLH.getResult().hasOneUse())
            buildFMAOp(subOp, defOpLH, defOpLH.getOperand(0),
                       defOpLH.getOperand(1), buildFNegativeOp(subOp, rhv));
        } else if (auto defOpRH = rhv.getDefiningOp<mlir::LLVM::FMulOp>()) {
          if (defOpRH.getResult().hasOneUse())
            buildFMAOp(subOp, defOpRH,
                       buildFNegativeOp(subOp, defOpRH.getOperand(0)),
                       defOpRH.getOperand(1), lhv);
        }
      }
    });
  }

protected:
  mlir::Value buildFNegativeOp(mlir::LLVM::FSubOp &op, const mlir::Value &v) {
    mlir::OpBuilder builder(op);
    return builder.create<mlir::LLVM::FNegOp>(op.getLoc(), v);
  }

  template <typename T>
  void buildFMAOp(T &op, mlir::LLVM::FMulOp defMul, mlir::Value defMulOperand1,
                  mlir::Value defMulOperand2, const mlir::Value &otherOperand) {
    mlir::OpBuilder builder(op);
    mlir::Value fma = builder.create<mlir::LLVM::FMAOp>(
        op.getLoc(), defMulOperand1, defMulOperand2, otherOperand);
    op.replaceAllUsesWith(fma);
    op.erase();
    defMul.erase();
  }
};
} // namespace

const char *KulaginAleksandrFMAPass::pluginName = "KulaginAleksandrFMA";
const char *KulaginAleksandrFMAPass::pluginVersion = "1.0.0";

MLIR_DECLARE_EXPLICIT_TYPE_ID(KulaginAleksandrFMAPass);
MLIR_DEFINE_EXPLICIT_TYPE_ID(KulaginAleksandrFMAPass);

mlir::PassPluginLibraryInfo getKulaginAleksandrFMAPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, KulaginAleksandrFMAPass::pluginName,
          KulaginAleksandrFMAPass::pluginVersion,
          []() { mlir::PassRegistration<KulaginAleksandrFMAPass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK mlir::PassPluginLibraryInfo
mlirGetPassPluginInfo() {
  return getKulaginAleksandrFMAPassPluginInfo();
}
