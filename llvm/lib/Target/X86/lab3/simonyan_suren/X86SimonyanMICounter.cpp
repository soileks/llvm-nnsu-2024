#include "X86.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

class X86SimonyanMICounterPass : public MachineFunctionPass {
public:
  static char ID;

  X86SimonyanMICounterPass() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    DebugLoc DL = MF.front().begin()->getDebugLoc();
    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
    Module &M = *MF.getFunction().getParent();
    GlobalVariable *globalCounter = M.getNamedGlobal("ic");

    if (!globalCounter) {
      LLVMContext &context = M.getContext();
      globalCounter =
          new GlobalVariable(M, IntegerType::get(context, 64), false,
                             GlobalValue::ExternalLinkage, nullptr, "ic");
    }

    for (auto &MBB : MF) {
      int instructionCount = std::distance(MBB.begin(), MBB.end());
      auto insertPoint = MBB.getFirstTerminator();

      if (insertPoint != MBB.end() && insertPoint != MBB.begin() &&
          insertPoint->getOpcode() >= X86::JCC_1 &&
          insertPoint->getOpcode() <= X86::JCC_4) {
        --insertPoint;
      }

      BuildMI(MBB, insertPoint, DL, TII->get(X86::ADD64mi32))
          .addReg(0)
          .addImm(1)
          .addReg(0)
          .addGlobalAddress(globalCounter)
          .addReg(0)
          .addImm(instructionCount);
    }

    return true;
  }
};

char X86SimonyanMICounterPass::ID = 0;

} // end anonymous namespace

static RegisterPass<X86SimonyanMICounterPass>
    X("x86-simonyan-mi-counter", "X86 Count of machine instructions pass",
      false, false);