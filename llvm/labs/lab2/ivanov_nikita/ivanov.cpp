#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

class MyLoopPass : public PassInfoMixin<MyLoopPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    auto *Ty = FunctionType::get(llvm::Type::getVoidTy(F.getContext()),
                                 false); // get func type
    auto *M = F.getParent();

    auto LoopStartFunc = M->getOrInsertFunction("_Z10loop_startv",
                                                Ty); // get or create func start
    auto LoopEndFunc =
        M->getOrInsertFunction("_Z8loop_endv", Ty); // get or create func end

    auto &LI = FAM.getResult<LoopAnalysis>(F);

    for (auto &L : LI) { // for by loop
      auto *Preheader =
          L->getLoopPreheader(); // get preheader block of the loop

      SmallVector<BasicBlock *, 1> ExitBlocks;
      L->getExitBlocks(ExitBlocks);

      if (!Preheader || ExitBlocks.empty())
        continue;

      int loop_start_inside = 0;
      int loop_end_inside = 0;

      for (Instruction &Instr : *Preheader) {
        if (CallInst *CI = dyn_cast<CallInst>(&Instr)) {
          if (CI->getCalledFunction() &&
              CI->getCalledFunction()->getName() == "_Z10loop_startv") {
            loop_start_inside = 1;
            break;
          }
        }
      }

      IRBuilder<> Builder(Preheader->getTerminator()); // api for basic block

      if (!loop_start_inside)
        Builder.CreateCall(LoopStartFunc); // paste loop_start

      for (auto &ExitBlock : ExitBlocks) {
        loop_end_inside = 0;
        for (Instruction &Instr : *ExitBlock) {
          if (CallInst *CI = dyn_cast<CallInst>(&Instr)) {
            if (CI->getCalledFunction() &&
                CI->getCalledFunction()->getName() == "_Z8loop_endv") {
              loop_end_inside = 1;
              break;
            }
          }
        }
        Builder.SetInsertPoint(
            ExitBlock->getFirstNonPHI()); // set pointer to exit block

        if (!loop_end_inside)
          Builder.CreateCall(LoopEndFunc); // paste loop_start
      }
    }
    return PreservedAnalyses::all();
  }
};

/* New PM Registration */
llvm::PassPluginLibraryInfo getAtikinLoopPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AtikinLoopPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "ivanov-loop-pass") {
                    PM.addPass(MyLoopPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getAtikinLoopPassPluginInfo();
}
