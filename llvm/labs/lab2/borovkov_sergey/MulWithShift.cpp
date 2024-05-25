#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <optional>

namespace {
struct MulShifts : llvm::PassInfoMixin<MulShifts> {
public:
  llvm::PreservedAnalyses run(llvm::Function &Func,
                              llvm::FunctionAnalysisManager &FAM) {
    std::vector<llvm::Instruction *> toRemove;

    for (llvm::BasicBlock &BB : Func) {
      for (llvm::Instruction &Inst : BB) {
        if (!llvm::BinaryOperator::classof(&Inst)) {
          continue;
        }
        llvm::BinaryOperator *op = llvm::cast<llvm::BinaryOperator>(&Inst);
        if (op->getOpcode() != llvm::Instruction::BinaryOps::Mul) {
          continue;
        }

        llvm::Value *lhs = op->getOperand(0);
        llvm::Value *rhs = op->getOperand(1);

        auto lg1 = getLogBase2(lhs);
        auto lg2 = getLogBase2(rhs);
        if (!lg1 && !lg2) {
          continue;
        }
        if (lg1 < lg2) {
          std::swap(lg1, lg2);
          std::swap(lhs, rhs);
        }

        if (lg1) {
          llvm::Value *lg_val = llvm::ConstantInt::get(
              llvm::IntegerType::get(Func.getContext(), 32),
              llvm::APInt(32, *lg1));

          llvm::Value *val = llvm::BinaryOperator::Create(
              llvm::Instruction::Shl, rhs, lg_val, op->getName(), op);

          op->replaceAllUsesWith(val);
          toRemove.push_back(op);
        }
      }
      for (auto *I : toRemove) {
        I->eraseFromParent();
      }
    }

    auto PA = llvm::PreservedAnalyses::all();
    return PA;
  }

private:
  std::optional<int> getLogBase2(llvm::Value *val) {
    if (llvm::ConstantInt *CI = llvm::dyn_cast<llvm::ConstantInt>(val)) {
      if (CI->getValue().isPowerOf2()) {
        return CI->getValue().exactLogBase2();
      }
    }
    if (auto *LI = llvm::dyn_cast<llvm::LoadInst>(val)) {
      llvm::Value *Op = LI->getPointerOperand();
      Op->reverseUseList();
      llvm::StoreInst *StInst = nullptr;
      for (auto *Inst : Op->users()) {
        if (auto *SI = llvm::dyn_cast<llvm::StoreInst>(Inst)) {
          StInst = SI;
        }
        if (Inst == LI) {
          break;
        }
      }
      Op->reverseUseList();
      if (!StInst) {
        return std::nullopt;
      }
      if (auto *CI =
              llvm::dyn_cast<llvm::ConstantInt>(StInst->getValueOperand())) {
        if (CI->getValue().isPowerOf2()) {
          return CI->getValue().exactLogBase2();
        }
      }
    }
    return std::nullopt;
  }
};
} // namespace

bool registerPlugin(llvm::StringRef Name, llvm::FunctionPassManager &FPM,
                    llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
  if (Name == "borovkovmulshifts") {
    FPM.addPass(MulShifts());
    return true;
  }
  return false;
}

llvm::PassPluginLibraryInfo getMulShiftsBorovkovPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MulShifts", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(registerPlugin);
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMulShiftsBorovkovPluginInfo();
}
