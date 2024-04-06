#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class InstrFuncPass : public llvm::PassInfoMixin<InstrFuncPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &AM) {
    llvm::LLVMContext &context = F.getContext();

    auto module = F.getParent();

    llvm::FunctionType *instrFuncType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::FunctionCallee instrStartFunction =
        module->getOrInsertFunction("instrument_start", instrFuncType);
    llvm::FunctionCallee instrEndFunction =
        module->getOrInsertFunction("instrument_end", instrFuncType);

    bool instrStartFunctionAdd = false;
    bool instrEndFunctionAdd = false;

    for (auto &block : F) {
      for (auto &instruction : block) {
        if (llvm::isa<llvm::CallInst>(&instruction)) {
          llvm::CallInst *callInst = llvm::cast<llvm::CallInst>(&instruction);
          if (callInst->getCalledFunction() == instrStartFunction.getCallee()) {
            instrStartFunctionAdd = true;
          } else if (callInst->getCalledFunction() ==
                     instrEndFunction.getCallee()) {
            instrEndFunctionAdd = true;
          }
        }
      }
    }

    llvm::IRBuilder<> builder(context);

    if (!instrStartFunctionAdd) {
      llvm::Instruction *firstInstruction = &F.front().front();
      builder.SetInsertPoint(firstInstruction);
      builder.CreateCall(instrStartFunction);
    }

    if (!instrEndFunctionAdd) {
      for (auto &block : F) {
        if (llvm::isa<llvm::ReturnInst>(block.getTerminator())) {
          builder.SetInsertPoint(block.getTerminator());
          builder.CreateCall(instrEndFunction);
        }
      }
    }

    return llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

llvm::PassPluginLibraryInfo getInstrFuncVolodinEPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InstrFuncVolodinE", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "instr-func-volodin") {
                    PM.addPass(InstrFuncPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getInstrFuncVolodinEPluginInfo();
}
