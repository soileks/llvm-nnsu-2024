#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

struct LoopPass : public llvm::PassInfoMixin<LoopPass> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    llvm::Module *ParentModule = F.getParent();
    llvm::FunctionType *myFuncType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(F.getContext()), false);
    llvm::FunctionCallee loopStartFunc =
        ParentModule->getOrInsertFunction("loop_start", myFuncType);
    llvm::FunctionCallee loopEndFunc =
        ParentModule->getOrInsertFunction("loop_end", myFuncType);
    llvm::LoopAnalysis::Result &LI = FAM.getResult<llvm::LoopAnalysis>(F);
    for (auto *L : LI) {
      insertIntoLoopFuncStartEnd(L, loopStartFunc, loopEndFunc);
    }
    return llvm::PreservedAnalyses::all();
  }

  void insertIntoLoopFuncStartEnd(llvm::Loop *L, llvm::FunctionCallee loopStart,
                                  llvm::FunctionCallee loopEnd) {
    llvm::IRBuilder<> Builder(L->getHeader()->getContext());
    llvm::SmallVector<llvm::BasicBlock *, 1> ExitBlocks;
    L->getExitBlocks(ExitBlocks);
    for (auto *const BB : ExitBlocks) {
      if (!isCalled(BB, loopEnd)) {
        Builder.SetInsertPoint(BB->getFirstNonPHI());
        Builder.CreateCall(loopEnd);
      }
    }
    llvm::BasicBlock *Header = L->getHeader();
    for (auto it = llvm::pred_begin(Header), et = llvm::pred_end(Header);
         it != et; ++it) {
      llvm::BasicBlock *Pred = *it;
      if (!L->contains(Pred) && !isCalled(Pred, loopStart)) {
        Builder.SetInsertPoint(Pred->getTerminator());
        Builder.CreateCall(loopStart);
      }
    }
  }

  bool isCalled(llvm::BasicBlock *const BB, llvm::FunctionCallee &callee) {
    for (auto &inst : *BB)
      if (auto *instCall = llvm::dyn_cast<llvm::CallInst>(&inst))
        if (instCall->getCalledFunction() == callee.getCallee())
          return true;
    return false;
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "PolozovLoopPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "polozov-loop-plugin") {
                    PM.addPass(LoopPass());
                    return true;
                  }
                  return false;
                });
          }};
}
