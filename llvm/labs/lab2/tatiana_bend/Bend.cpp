#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <string>

namespace {

struct Bend : llvm::PassInfoMixin<Bend> {

  template <typename Func>
  void insert_instruction(llvm::Function &F, const std::string name,
                          const Func &insert_instr) {
    llvm::FunctionCallee instrument_callee = F.getParent()->getOrInsertFunction(
        name, llvm::Type::getVoidTy(F.getContext()));
    auto *instrument_function =
        llvm::dyn_cast<llvm::Function>(instrument_callee.getCallee());
    for (auto *U : instrument_function->users()) {
      if (auto *callInst = llvm::dyn_cast<llvm::CallInst>(U)) {
        if (callInst->getFunction() == &F) {
          return;
        }
      }
    }

    insert_instr(F, instrument_callee);
  }

  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    llvm::LLVMContext &Context = F.getContext();
    llvm::IRBuilder<> Builder(Context);

    insert_instruction(
        F, "instrument_start",
        [&](llvm::Function &F, llvm::FunctionCallee instrument_start) {
          auto *firstInsertionPt =
              &*F.getEntryBlock().getFirstNonPHIOrDbgOrAlloca();
          Builder.SetInsertPoint(firstInsertionPt);
          Builder.CreateCall(instrument_start);
        });

    insert_instruction(
        F, "instrument_end",
        [&](llvm::Function &F, llvm::FunctionCallee instrument_start) {
          for (auto &Block : F) {
            if (llvm::isa<llvm::ReturnInst>(Block.getTerminator())) {
              Builder.SetInsertPoint(Block.getTerminator());
              Builder.CreateCall(instrument_start);
            }
          }
        });
    return llvm::PreservedAnalyses::all();
  }
};

} // namespace

/* New PM Registration */
llvm::PassPluginLibraryInfo getBendPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Bend", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "MrrrBend") {
                    PM.addPass(Bend());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getBendPluginInfo();
}