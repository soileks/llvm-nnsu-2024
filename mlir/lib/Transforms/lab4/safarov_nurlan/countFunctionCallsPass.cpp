#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Tools/Plugins/PassPlugin.h"

using namespace mlir;

namespace {
class FunctionCallCounterPass
    : public PassWrapper<FunctionCallCounterPass, OperationPass<ModuleOp>> {
public:
  StringRef getArgument() const final { return "SafarovNcntFuncCalls"; }
  void runOnOperation() override {
    std::vector<LLVM::LLVMFuncOp> functions;
    std::map<StringRef, int> numberOfCalls;

    getOperation()->walk([&](Operation *operation) {
      auto checkingForAFunction = dyn_cast<LLVM::LLVMFuncOp>(operation);
      if (checkingForAFunction) {
        functions.push_back(checkingForAFunction);
      }
      auto checkingForAFunctionCall = dyn_cast<LLVM::CallOp>(operation);
      if (checkingForAFunctionCall) {
        numberOfCalls[checkingForAFunctionCall.getCallee().value()]++;
      }
    });

    for (auto &elem : functions) {
      auto countCall = numberOfCalls[elem.getName()];
      auto value =
          IntegerAttr::get(IntegerType::get(elem.getContext(), 32), countCall);
      elem->setAttr("countCall", value);
    }
  }
};
} // namespace

MLIR_DECLARE_EXPLICIT_TYPE_ID(FunctionCallCounterPass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(FunctionCallCounterPass)

PassPluginLibraryInfo getFunctionCallCounterPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "SafarovNcntFuncCalls", LLVM_VERSION_STRING,
          []() { PassRegistration<FunctionCallCounterPass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo mlirGetPassPluginInfo() {
  return getFunctionCallCounterPassPluginInfo();
}
