#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Tools/Plugins/PassPlugin.h"

using namespace mlir;

namespace {
class FunctionCallCounterPass
    : public PassWrapper<FunctionCallCounterPass, OperationPass<ModuleOp>> {
public:
  StringRef getArgument() const final { return "FuncCallCounter"; }
  StringRef getDescription() const final {
    return "Counts the amount of calls to each function in the module";
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    std::map<StringRef, int> callCounter;

    module.walk([&](Operation *op) {
      if (auto call = dyn_cast<LLVM::CallOp>(op)) {
        if (auto funcNameOpt = call.getCallee()) {
          StringRef funcName = funcNameOpt.value();
          callCounter[funcName]++;
        }
      }
    });

    module.walk([&](LLVM::LLVMFuncOp funcOp) {
      StringRef funcName = funcOp.getName();
      int callCount = callCounter[funcName];
      funcOp->setAttr(
          "CallsAmount",
          IntegerAttr::get(IntegerType::get(funcOp.getContext(), 32),
                           callCount));
    });
  }
};
} // namespace

MLIR_DECLARE_EXPLICIT_TYPE_ID(FunctionCallCounterPass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(FunctionCallCounterPass)

PassPluginLibraryInfo getFunctionCallCounterPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "FuncCallCounter", LLVM_VERSION_STRING,
          []() { PassRegistration<FunctionCallCounterPass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo mlirGetPassPluginInfo() {
  return getFunctionCallCounterPassPluginInfo();
}