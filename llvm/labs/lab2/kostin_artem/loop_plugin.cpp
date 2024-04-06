#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

struct LoopStartEnd : public PassInfoMixin<LoopStartEnd> {
  PreservedAnalyses run(Function &Pfunction,
                        FunctionAnalysisManager &Panalysis) {

    FunctionType *LoopFuncType =
        FunctionType::get(Type::getVoidTy(Pfunction.getContext()), false);
    Module *ModuleOfPfuncParent = Pfunction.getParent();

    LoopAnalysis::Result &GetAnalysis =
        Panalysis.getResult<LoopAnalysis>(Pfunction);
    for (auto &Loop : GetAnalysis) {
      BasicBlock *EntryBlock = Loop->getLoopPreheader();
      IRBuilder<> FuncBuilder(Loop->getHeader()->getContext());
      if (EntryBlock != nullptr) {
        bool LoopStartInFunc = false;
        for (Instruction &Instr : *EntryBlock) {
          if (CallInst *CI = dyn_cast<CallInst>(&Instr)) {
            if (CI->getCalledFunction() &&
                CI->getCalledFunction()->getName() == "loop_start") {
              LoopStartInFunc = true;
              break;
            }
          }
        }
        if (!LoopStartInFunc) {
          FuncBuilder.SetInsertPoint(EntryBlock->getTerminator());
          FuncBuilder.CreateCall(ModuleOfPfuncParent->getOrInsertFunction(
              "loop_start", LoopFuncType));
        }
      }
      SmallVector<BasicBlock *, 4> ExitBlocks;
      Loop->getExitBlocks(ExitBlocks);
      bool LoopEndInFunc = false;
      for (BasicBlock *ExitBlock : ExitBlocks) {
        LoopEndInFunc = false;
        for (Instruction &Instr : *ExitBlock) {
          if (CallInst *CI = dyn_cast<CallInst>(&Instr)) {
            if (CI->getCalledFunction() &&
                CI->getCalledFunction()->getName() == "loop_end") {
              LoopEndInFunc = true;
              break;
            }
          }
        }
        if (!LoopEndInFunc) {
          FuncBuilder.SetInsertPoint(&*ExitBlock->getFirstInsertionPt());
          FuncBuilder.CreateCall(ModuleOfPfuncParent->getOrInsertFunction(
              "loop_end", LoopFuncType));
        }
      }
    }
    return PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopStartEndPlugin", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "loop-start-end") {
                    PM.addPass(LoopStartEnd());
                    return true;
                  }
                  return false;
                });
          }};
}
