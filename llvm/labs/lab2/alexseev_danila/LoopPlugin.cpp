#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Debug.h"

struct LoopPlugin : public llvm::PassInfoMixin<LoopPlugin> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    llvm::dbgs() << "Заход в run\n";

    llvm::LLVMContext &Context = F.getContext();
    llvm::Module *ParentModule = F.getParent();

    llvm::FunctionType *funcType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(Context), false);

    llvm::LoopAnalysis::Result &LI = FAM.getResult<llvm::LoopAnalysis>(F);
    for (auto *Loop : LI) {
      llvm::dbgs() << "Заход в цикл\n";

      llvm::IRBuilder<> Builder(Loop->getHeader()->getContext());

      llvm::BasicBlock *Header = Loop->getHeader();
      for (auto *const Preheader :
           llvm::children<llvm::Inverse<llvm::BasicBlock *>>(Header)) {
        if (Loop->contains(Preheader) &&
            !isLoopCallPresent("loop_start", Preheader)) {
          Builder.SetInsertPoint(Preheader->getTerminator());
          Builder.CreateCall(
              ParentModule->getOrInsertFunction("loop_start", funcType));
        }
      }

      llvm::SmallVector<llvm::BasicBlock *, 4> ExitBlocks;
      Loop->getExitBlocks(ExitBlocks);
      for (auto *const ExitBlock : ExitBlocks) {
        if (!isLoopCallPresent("loop_end", ExitBlock) &&
            LastExitBlock(ExitBlock, ExitBlocks)) {
          Builder.SetInsertPoint(ExitBlock->getFirstNonPHI());
          Builder.CreateCall(
              ParentModule->getOrInsertFunction("loop_end", funcType));
        }
      }
    }
    return llvm::PreservedAnalyses::all();
  }

  bool isLoopCallPresent(const std::string &loopFunctionName,
                         llvm::BasicBlock *block) {
    if (!block)
      return false;
    for (auto &inst : *block) {
      if (auto *instCall = llvm::dyn_cast<llvm::CallInst>(&inst)) {
        if (auto *CalledFunction = instCall->getCalledFunction()) {
          if (CalledFunction->getName() == loopFunctionName) {
            return true;
          }
        }
      }
    }
    return false;
  }

  bool
  LastExitBlock(llvm::BasicBlock *const BB,
                const llvm::SmallVector<llvm::BasicBlock *, 4> &ExitBlocks) {
    auto *Branch = llvm::dyn_cast<llvm::BranchInst>(BB->getTerminator());
    if (!Branch || !Branch->isUnconditional())
      return true;
    llvm::BasicBlock *TargetBB = Branch->getSuccessor(0);
    for (llvm::BasicBlock *Block : ExitBlocks)
      if (Block == TargetBB)
        return false;
    return true;
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopPlugin", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "alexseev-loop-plugin") {
                    PM.addPass(LoopPlugin());
                    return true;
                  }
                  return false;
                });
          }};
}
