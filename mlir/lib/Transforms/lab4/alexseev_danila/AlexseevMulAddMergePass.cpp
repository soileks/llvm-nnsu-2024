#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Tools/Plugins/PassPlugin.h"

class AlexseevMulAddMergePass
    : public mlir::PassWrapper<AlexseevMulAddMergePass,
                               mlir::OperationPass<mlir::ModuleOp>> {
  void createFMAOperation(mlir::LLVM::FAddOp &addOp, mlir::LLVM::FMulOp &mulOp,
                          mlir::Value &otherOperand) {
    mlir::OpBuilder builder(addOp);
    mlir::Value fma = builder.create<mlir::LLVM::FMAOp>(
        addOp.getLoc(), addOp.getType(), mulOp.getOperand(0),
        mulOp.getOperand(1), otherOperand);
    addOp.replaceAllUsesWith(fma);
    addOp.erase();
    if (mulOp.use_empty())
      mulOp.erase();
  }

public:
  mlir::StringRef getArgument() const final { return "alexseev_mul_add_merge"; }

  mlir::StringRef getDescription() const final {
    return "Merge multiplication and addition into a single math.fma";
  }

  void runOnOperation() override {
    getOperation().walk([&](mlir::LLVM::FAddOp addOp) {
      mlir::Value addLHS = addOp.getOperand(0);
      mlir::Value addRHS = addOp.getOperand(1);

      if (auto mulOpLHS = addLHS.getDefiningOp<mlir::LLVM::FMulOp>()) {
        createFMAOperation(addOp, mulOpLHS, addRHS);
      } else if (auto mulOpRHS = addRHS.getDefiningOp<mlir::LLVM::FMulOp>()) {
        createFMAOperation(addOp, mulOpRHS, addLHS);
      }
    });
  }
};

MLIR_DECLARE_EXPLICIT_TYPE_ID(AlexseevMulAddMergePass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(AlexseevMulAddMergePass)

extern "C" LLVM_ATTRIBUTE_WEAK mlir::PassPluginLibraryInfo
mlirGetPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "alexseev_mul_add_merge",
          LLVM_VERSION_STRING,
          []() { mlir::PassRegistration<AlexseevMulAddMergePass>(); }};
}
