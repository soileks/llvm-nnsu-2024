#include "llvm/IR/Function.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <stack>

namespace {

struct ReplaceMultToShift : llvm::PassInfoMixin<ReplaceMultToShift> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    std::stack<llvm::Instruction *> inst_list;

    for (llvm::BasicBlock &BB : F) {
      for (llvm::Instruction &I : BB) {
        if (llvm::BinaryOperator *BO =
                llvm::dyn_cast<llvm::BinaryOperator>(&I)) {
          if (BO->getOpcode() == llvm::Instruction::BinaryOps::Mul) {
            llvm::ConstantInt *LOp =
                llvm::dyn_cast<llvm::ConstantInt>(BO->getOperand(0));
            llvm::ConstantInt *ROp =
                llvm::dyn_cast<llvm::ConstantInt>(BO->getOperand(1));

            if (LOp || ROp) {
              if (LOp && LOp->getValue().isPowerOf2()) {
                llvm::Value *llvmValue = llvm::ConstantInt::get(
                    llvm::IntegerType::get(F.getContext(), 32),
                    llvm::APInt(32, LOp->getValue().exactLogBase2()));
                BO->replaceAllUsesWith(llvm::BinaryOperator::Create(
                    llvm::Instruction::Shl, BO->getOperand(1), llvmValue,
                    "shiftInst", BO));
              } else if (ROp && ROp->getValue().isPowerOf2()) {
                llvm::Value *llvmValue = llvm::ConstantInt::get(
                    llvm::IntegerType::get(F.getContext(), 32),
                    llvm::APInt(32, ROp->getValue().exactLogBase2()));
                BO->replaceAllUsesWith(llvm::BinaryOperator::Create(
                    llvm::Instruction::Shl, BO->getOperand(0), llvmValue,
                    "shiftInst", BO));
              }
              inst_list.push(&I);
            }
          }
        }
      }
      while (!inst_list.empty()) {
        inst_list.top()->eraseFromParent();
        inst_list.pop();
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
  return {LLVM_PLUGIN_API_VERSION, "Replace-Mult-Shift", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "korablev-replace-mul-shift") {
                    FPM.addPass(ReplaceMultToShift());
                    return true;
                  }
                  return false;
                });
          }};
}