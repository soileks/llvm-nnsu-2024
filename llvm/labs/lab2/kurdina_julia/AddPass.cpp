#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

struct AddPass : public PassInfoMixin<AddPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {

    LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
    auto &context = F.getContext();
    Module *parent = F.getParent();
    FunctionType *FT = FunctionType::get(Type::getVoidTy(context), false);

    for (auto &l : LI) {
      BasicBlock *Header = l->getHeader();
      BasicBlock *EntryBlock = l->getLoopPreheader();
      IRBuilder<> Builder(Header->getContext());

      if (EntryBlock != nullptr) {
        bool loop_func = false;
        for (auto &I : *EntryBlock) {
          if (auto *CI = dyn_cast<CallInst>(&I)) {
            if (CI->getCalledFunction() &&
                CI->getCalledFunction()->getName() == "loop_start") {
              loop_func = true;
              break;
            }
          }
        }
        if (!loop_func) {
          Builder.SetInsertPoint(EntryBlock->getTerminator());
          Builder.CreateCall(parent->getOrInsertFunction("loop_start", FT));
        }
      }
      SmallVector<BasicBlock *, 8> ExitBlock;
      l->getExitBlocks(ExitBlock);
      bool loop_func_end = false;
      for (auto *e : ExitBlock) {
        loop_func_end = false;
        for (auto &I : *e) {
          if (auto *CI = dyn_cast<CallInst>(&I)) {
            if (CI->getCalledFunction() &&
                CI->getCalledFunction()->getName() == "loop_end") {
              loop_func_end = true;
              break;
            }
          }
        }
        if (!loop_func_end) {
          Builder.SetInsertPoint(&*e->getFirstInsertionPt());
          Builder.CreateCall(parent->getOrInsertFunction("loop_end", FT));
        }
      }
    }
    return PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AddNewPlugin", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "loop_func") {
                    PM.addPass(AddPass());
                    return true;
                  }
                  return false;
                });
          }};
}
