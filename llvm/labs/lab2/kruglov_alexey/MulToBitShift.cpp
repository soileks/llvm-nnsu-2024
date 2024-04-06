#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <stack>
#include <string>

namespace {
struct MulToBitShift : llvm::PassInfoMixin<MulToBitShift> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    std::stack<llvm::Instruction *> worklist;
    for (llvm::BasicBlock &BB : F) {
      for (llvm::Instruction &Inst : BB) {
        if (static_cast<std::string>(Inst.getOpcodeName()) != "mul") {
          continue;
        }
        llvm::BinaryOperator *op = llvm::dyn_cast<llvm::BinaryOperator>(&Inst);
        llvm::IRBuilder<> builder(op);
        llvm::Value *lhs = op->getOperand(0);
        llvm::Value *rhs = op->getOperand(1);
        int lg1 = -2, lg2 = -2;
        if (llvm::ConstantInt *CIl = llvm::dyn_cast<llvm::ConstantInt>(lhs)) {
          lg1 = CIl->getValue().exactLogBase2();
        }
        if (llvm::ConstantInt *CIr = llvm::dyn_cast<llvm::ConstantInt>(rhs)) {
          lg2 = CIr->getValue().exactLogBase2();
        }
        if (lg1 < lg2) {
          std::swap(lg1, lg2);
          std::swap(lhs, rhs);
        }
        if (lg1 > -1) {
          llvm::Value *val;
          if (lg1 == 0)
            val = rhs;
          else {
            val = builder.CreateShl(rhs,
                                    llvm::ConstantInt::get(op->getType(), lg1));
          }
          op->replaceAllUsesWith(val);
          worklist.push(&Inst);
        }
      }
      while (!worklist.empty()) {
        llvm::Instruction *I = worklist.top();
        I->eraseFromParent();
        worklist.pop();
      }
    }

    auto PA = llvm::PreservedAnalyses::all();
    return PA;
  }
}; // end of struct
} // end of anonymous namespace

bool registerPipeline(llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                      llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
  if (Name == "MulToBitShift") {
    FPM.addPass(MulToBitShift());
    return true;
  }
  return false;
}

llvm::PassPluginLibraryInfo getMulToBitShiftPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MulToBitShift", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(registerPipeline);
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMulToBitShiftPluginInfo();
}