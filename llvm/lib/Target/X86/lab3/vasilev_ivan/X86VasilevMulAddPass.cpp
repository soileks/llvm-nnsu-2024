#include "X86.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Register.h"
#include <queue>
#include <unordered_set>

using namespace llvm;

namespace {
class X86VasilevMulAddPass : public MachineFunctionPass {
public:
  static char ID;
  X86VasilevMulAddPass() : MachineFunctionPass(ID) {}
  bool runOnMachineFunction(MachineFunction &MF) override;
};

char X86VasilevMulAddPass::ID = 0;

bool X86VasilevMulAddPass::runOnMachineFunction(MachineFunction &machineFunc) {
  const TargetInstrInfo *instrInfo = machineFunc.getSubtarget().getInstrInfo();
  bool changed = false;

  std::queue<MachineInstr *> worklist;
  std::unordered_set<MachineInstr *> addInstrs;

  for (auto &block : machineFunc) {
    for (auto I = block.begin(); I != block.end(); ++I) {
      if (I->getOpcode() == X86::MULPDrr) {
        worklist.push(&(*I));
      }
    }

    for (auto I = block.begin(); I != block.end(); ++I) {
      if (I->getOpcode() == X86::ADDPDrr) {
        addInstrs.insert(&(*I));
      }
    }

    while (!worklist.empty()) {
      MachineInstr *mulInstr = worklist.front();
      worklist.pop();

      for (auto I : addInstrs) {
        if (I->getOperand(1).getReg() == mulInstr->getOperand(0).getReg()) {
          MachineInstrBuilder MIB = BuildMI(
              *mulInstr->getParent(), *mulInstr, mulInstr->getDebugLoc(),
              instrInfo->get(X86::VFMADD213PDZ128r));

          MIB.addReg(I->getOperand(0).getReg(), RegState::Define);
          MIB.addReg(mulInstr->getOperand(1).getReg());
          MIB.addReg(mulInstr->getOperand(2).getReg());
          MIB.addReg(I->getOperand(2).getReg());

          mulInstr->eraseFromParent();
          I->eraseFromParent();
          changed = true;
          addInstrs.erase(I);
          break;
        }
      }
    }
  }

  return changed;
}
} // namespace
static RegisterPass<X86VasilevMulAddPass>
    X("x86-Vasilevmuladd", "X86 Vasilev muladd pass", false, false);
