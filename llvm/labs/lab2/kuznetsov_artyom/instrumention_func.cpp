#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct InstrumentFunctions : llvm::PassInfoMixin<InstrumentFunctions> {
  llvm::PreservedAnalyses run(llvm::Function &func,
                              llvm::FunctionAnalysisManager &) {
    llvm::LLVMContext &context = func.getContext();
    llvm::IRBuilder<> builder(context);
    auto module = func.getParent();

    llvm::FunctionType *funcType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::FunctionCallee instrStartFunc =
        module->getOrInsertFunction("instrument_start", funcType);
    llvm::FunctionCallee instEndFunc =
        module->getOrInsertFunction("instrument_end", funcType);

    bool startInserted = false;
    bool endInserted = false;

    for (auto &block : func) {
      for (auto &instruction : block) {
        if (llvm::isa<llvm::CallInst>(&instruction)) {
          llvm::CallInst *callInst = llvm::cast<llvm::CallInst>(&instruction);
          if (callInst->getCalledFunction() == instrStartFunc.getCallee()) {
            startInserted = true;
          } else if (callInst->getCalledFunction() == instEndFunc.getCallee()) {
            endInserted = true;
          }
        }
      }
    }

    if (!startInserted) {
      builder.SetInsertPoint(&func.getEntryBlock().front());
      builder.CreateCall(instrStartFunc);
    }
    if (!endInserted) {
      for (auto &block : func) {
        if (llvm::isa<llvm::ReturnInst>(block.getTerminator())) {
          builder.SetInsertPoint(block.getTerminator());
          builder.CreateCall(instEndFunc);
        }
      }
    }

    return llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InstrumentFunctions", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "instr_func") {
                    FPM.addPass(InstrumentFunctions{});
                    return true;
                  }
                  return false;
                });
          }};
}
