#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <vector>

using namespace llvm;

namespace {

bool runPlugin(Function &F) {
  std::vector<Instruction *> instrDeleteList;
  for (llvm::BasicBlock &funcBlock : F) {
    for (llvm::Instruction &instr : funcBlock) {
      auto *binOper = dyn_cast<BinaryOperator>(&instr);
      if (!binOper ||
          binOper->getOpcode() != llvm::Instruction::BinaryOps::Mul) {
        continue;
      }
      auto *constant = dyn_cast<ConstantInt>(binOper->getOperand(0));
      int notConstantOperandNumber;
      if (constant) {
        notConstantOperandNumber = 1;
      } else {
        constant = dyn_cast<ConstantInt>(binOper->getOperand(1));
        notConstantOperandNumber = 0;
      }
      if (constant) {
        llvm::APInt value = constant->getUniqueInteger();
        if (value.isPowerOf2()) {
          IRBuilder<> Builder(binOper);
          Value *newInstr = Builder.CreateShl(
              binOper->getOperand(notConstantOperandNumber), value.logBase2());
          binOper->replaceAllUsesWith(newInstr);
          instrDeleteList.push_back(&instr);
        }
      }
    }
  }
  for (auto *instr : instrDeleteList)
    instr->eraseFromParent();

  return false;
}

struct RunOrNonePass : PassInfoMixin<RunOrNonePass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    if (!runPlugin(F))
      return PreservedAnalyses::all();
    return PreservedAnalyses::none();
  }
};

} // namespace
llvm::PassPluginLibraryInfo getRunOrNonePassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "replaceMulWithBitShift",
          LLVM_VERSION_STRING, [](PassBuilder &PB) {
            PB.registerVectorizerStartEPCallback(
                [](llvm::FunctionPassManager &PM, OptimizationLevel Level) {
                  PM.addPass(RunOrNonePass());
                });
            PB.registerPipelineParsingCallback(
                [](StringRef Name, llvm::FunctionPassManager &PM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "replaceMulWithBitShift") {
                    PM.addPass(RunOrNonePass());
                    return true;
                  }
                  return false;
                });
          }};
}

#ifndef LLVM_MY_PLUGIN_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getRunOrNonePassPluginInfo();
}
#endif
