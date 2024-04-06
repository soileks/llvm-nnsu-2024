#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Compiler.h"

namespace {

struct ForWrapper : llvm::PassInfoMixin<ForWrapper> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    auto *Ty =
        llvm::FunctionType::get(llvm::Type::getVoidTy(F.getContext()), false);
    auto *M = F.getParent();
    auto LoopStartFunc = M->getOrInsertFunction("loop_start", Ty);
    auto LoopEndFunc = M->getOrInsertFunction("loop_end", Ty);
    auto &Ctx = F.getContext();
    llvm::IRBuilder<> Builder(Ctx);

    auto &LI = FAM.getResult<llvm::LoopAnalysis>(F);
    for (auto &L : LI) {
      auto *Header = L->getHeader();
      for (auto *const Pred :
           llvm::children<llvm::Inverse<llvm::BasicBlock *>>(Header)) {
        if (!L->contains(Pred)) {
          Builder.SetInsertPoint(Pred->getTerminator());
          Builder.CreateCall(LoopStartFunc);
        }
      }

      llvm::SmallVector<llvm::BasicBlock *, 8> ExitBBs;
      L->getExitBlocks(ExitBBs);
      for (auto *const Bb : ExitBBs) {
        Builder.SetInsertPoint(Bb->getFirstNonPHI());
        Builder.CreateCall(LoopEndFunc);
      }
    }

    auto PA = llvm::PreservedAnalyses::all();
    PA.abandon<llvm::LoopAnalysis>();
    return PA;
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ForWrapperPlugin", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "kulikov-wrap-plugin") {
                    PM.addPass(ForWrapper());
                    return true;
                  }
                  return false;
                });
          }};
}
