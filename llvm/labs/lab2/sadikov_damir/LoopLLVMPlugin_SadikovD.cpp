#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

struct LoopLLVMPlugin_SadikovD
    : public llvm::PassInfoMixin<LoopLLVMPlugin_SadikovD> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    llvm::FunctionType *FuncType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(F.getContext()), false);

    llvm::FunctionCallee LoopStart =
        F.getParent()->getOrInsertFunction("loop_start", FuncType);
    llvm::FunctionCallee LoopEnd =
        F.getParent()->getOrInsertFunction("loop_end", FuncType);

    llvm::LoopAnalysis::Result &LoopAnalysisResult =
        FAM.getResult<llvm::LoopAnalysis>(F);

    llvm::IRBuilder<> Builder(F.getContext());

    for (auto *Loop : LoopAnalysisResult) {
      llvm::pred_range PredHeaders = llvm::predecessors(Loop->getHeader());

      llvm::SmallVector<llvm::BasicBlock *> ExitBlocks;
      Loop->getExitBlocks(ExitBlocks);

      for (auto Pred : PredHeaders) {
        if (!Loop->contains(Pred) && !BBhaveFC(Pred, LoopStart)) {
          Builder.SetInsertPoint(Pred->getTerminator());
          Builder.CreateCall(LoopStart);
        }
      }

      for (auto *const EB : ExitBlocks) {
        if (!BBhaveFC(EB, LoopEnd)) {
          Builder.SetInsertPoint(EB->getFirstNonPHI());
          Builder.CreateCall(LoopEnd);
        }
      }
    }

    return llvm::PreservedAnalyses::all();
  }

  bool BBhaveFC(llvm::BasicBlock *const BB, llvm::FunctionCallee &FC) {
    for (auto &Instruction : *BB) {
      if (auto *instCall = llvm::dyn_cast<llvm::CallInst>(&Instruction)) {
        if (instCall->getCalledFunction() == FC.getCallee()) {
          return true;
        }
      }
    }
    return false;
  }
};

llvm::PassPluginLibraryInfo getLoopLLVMPlugin_SadikovDPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopLLVMPlugin_SadikovD",
          LLVM_VERSION_STRING, [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "LoopLLVMPlugin_SadikovD") {
                    PM.addPass(LoopLLVMPlugin_SadikovD());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLoopLLVMPlugin_SadikovDPluginInfo();
}
