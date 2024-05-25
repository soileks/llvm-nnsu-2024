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
  PreservedAnalyses run(Function &Func, FunctionAnalysisManager &FAM) {
    auto *Ty =
        FunctionType::get(llvm::Type::getVoidTy(Func.getContext()), false);
    auto *M = Func.getParent();

    auto LoopStartFunc = M->getOrInsertFunction("_Z10loop_startv", Ty);
    auto LoopEndFunc = M->getOrInsertFunction("_Z8loop_endv", Ty);

    auto &LI = FAM.getResult<LoopAnalysis>(Func);

    for (auto &Loop : LI) {
      auto *Preheader = Loop->getLoopPreheader();

      SmallVector<BasicBlock *, 4> ExitBlocks;
      Loop->getExitBlocks(ExitBlocks);

      if (!Preheader || ExitBlocks.empty())
        continue;

      llvm::Function *StartFunc =
          llvm::cast<llvm::Function>(LoopStartFunc.getCallee());

      int loop_start = 0;
      int loop_end = 0;

      for (User *U : LoopStartFunc.getCallee()->users()) {
        if (CallInst *CI = dyn_cast<CallInst>(U)) {
          if (CI->getParent() == Preheader) {
            loop_start = 1;
            break;
          }
        }
      }

      IRBuilder<> Builder(Preheader->getTerminator());

      if (!loop_start)
        Builder.CreateCall(StartFunc);

      llvm::Function *EndFunc =
          llvm::cast<llvm::Function>(LoopEndFunc.getCallee());
      for (auto &ExitBlock : ExitBlocks) {
        for (auto *U : LoopEndFunc.getCallee()->users()) {
          if (auto *CI = dyn_cast<CallInst>(U)) {
            if (CI->getParent() == ExitBlock) {
              loop_end = true;
              break;
            }
          }
        }
        Builder.SetInsertPoint(ExitBlock->getFirstNonPHI());

        if (!loop_end)
          Builder.CreateCall(EndFunc);
      }
    }
    return PreservedAnalyses::all();
  }
};

llvm::PassPluginLibraryInfo VasilevGuardingcalls() {
  return {LLVM_PLUGIN_API_VERSION, "VasilevGuardingcalls", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "vasilev-loop-pass") {
                    PM.addPass(MyLoopPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return VasilevGuardingcalls();
}
