#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Tools/Plugins/PassPlugin.h"

using namespace mlir;

namespace {
class FuncCallCountPass
    : public PassWrapper<FuncCallCountPass, OperationPass<ModuleOp>> {
public:
  StringRef getArgument() const final { return "BodrovFuncCallCountPass"; }
  StringRef getDescription() const final {
    return "Counts the number of calls to each function in the module";
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();
    std::map<StringRef, int> callCounter;

    // Walk through all operations in the module to count function calls
    module.walk([&](Operation *op) {
      if (auto callOp = dyn_cast<LLVM::CallOp>(op)) {
        StringRef callee = callOp.getCallee().value();
        callCounter[callee]++;
      }
    });

    // Attach the call count as an attribute to each func.func operation
    module.walk([&](LLVM::LLVMFuncOp funcOp) {
      StringRef funcName = funcOp.getName();
      int numCalls = callCounter[funcName];
      auto attrValue =
          IntegerAttr::get(IntegerType::get(funcOp.getContext(), 32), numCalls);
      funcOp->setAttr("numCalls", attrValue);
    });
  }
};
} // namespace

MLIR_DECLARE_EXPLICIT_TYPE_ID(FuncCallCountPass)
MLIR_DEFINE_EXPLICIT_TYPE_ID(FuncCallCountPass)

PassPluginLibraryInfo getFuncCallCountPassPluginInfo() {
  return {MLIR_PLUGIN_API_VERSION, "BodrovFuncCallCountPass",
          LLVM_VERSION_STRING, []() { PassRegistration<FuncCallCountPass>(); }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo mlirGetPassPluginInfo() {
  return getFuncCallCountPassPluginInfo();
}
