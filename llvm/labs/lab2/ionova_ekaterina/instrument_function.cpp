#include "llvm/IR/Attributes.inc"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

namespace {
struct InstrumentationStrt : llvm::PassInfoMixin<InstrumentationStrt> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &) {
    llvm::LLVMContext &context = F.getContext();
    llvm::IRBuilder<> builder(context);
    llvm::Module *module = F.getParent();

    llvm::FunctionType *type =
        llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::FunctionCallee func_f =
        (*module).getOrInsertFunction("instrument_start", type);
    llvm::FunctionCallee func_l =
        (*module).getOrInsertFunction("instrument_end", type);

    bool foundInstrument_start = false;
    bool foundInstrument_end = false;

    for (auto &block : F) {
      for (auto &instruction : block) {
        if (llvm::isa<llvm::CallInst>(&instruction)) {
          llvm::CallInst *callInst = llvm::cast<llvm::CallInst>(&instruction);
          if (callInst->getCalledFunction() == func_f.getCallee()) {
            foundInstrument_start = true;
          } else if (callInst->getCalledFunction() == func_l.getCallee()) {
            foundInstrument_end = true;
          }
        }
      }
    }

    if (!foundInstrument_start) {
      builder.SetInsertPoint(&F.getEntryBlock().front());
      builder.CreateCall(func_f);
    }
    if (!foundInstrument_end) {
      for (llvm::BasicBlock &BB : F) {
        if (llvm::dyn_cast<llvm::ReturnInst>(BB.getTerminator())) {
          builder.SetInsertPoint(BB.getTerminator());
          builder.CreateCall(func_l);
        }
      }
    }

    return llvm::PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "instrumentation_function", "0.1",
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) -> bool {
                  if (name == "instrumentation_function") {
                    FPM.addPass(InstrumentationStrt{});
                    return true;
                  }
                  return false;
                });
          }};
};