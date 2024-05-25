#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Tools/Plugins/PassPlugin.h"

using namespace mlir;

namespace {
class FunctionCallCounterPass
    : public PassWrapper<FunctionCallCounterPass, OperationPass<ModuleOp>> {
public:
  StringRef getArgument() const final { return "ProkofevFuncCallCounter"; }
  StringRef getDescription() const final {
    return "Counts the number of calls to each function and attaches this "
           "count as an attribute";
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    llvm::DenseMap<StringRef, int> callCounter;

    module.walk([&](LLVM::CallOp callOp) {
      if (auto callee = callOp.getCallee()) {
        callCounter[callee.value()]++;
      }
    });

    module.walk([&](LLVM::LLVMFuncOp funcOp) {
      StringRef funcName = funcOp.getName();
      auto callCountIter = callCounter.find(funcName);
      int callCount =
          (callCountIter != callCounter.end()) ? callCountIter->second : 0;
      funcOp->setAttr(
          "CallCount",
          IntegerAttr::get(IntegerType::get(funcOp.getContext(), 32),
                           callCount));
    });
  }
};
} // namespace

MLIR_DECLARE_EXPLICIT_TYPE_ID(FunctionCallCounterPass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(FunctionCallCounterPass)

PassPluginLibraryInfo getFunctionCallCounterPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "ProkofevFuncCallCounter",
          LLVM_VERSION_STRING,
          []() { PassRegistration<FunctionCallCounterPass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo mlirGetPassPluginInfo() {
  return getFunctionCallCounterPassPluginInfo();
}
