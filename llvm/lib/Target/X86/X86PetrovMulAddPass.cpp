#include "X86.h"
#include "X86InstrInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

#define X86_MULADD_PASS_NAME "X86 Petrov muladd pass"

namespace {
class X86PetrovMulAddPass : public MachineFunctionPass {
public:
  static char ID;

  X86PetrovMulAddPass() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override { return X86_MULADD_PASS_NAME; }
};

char X86PetrovMulAddPass::ID = 0;

bool X86PetrovMulAddPass::runOnMachineFunction(llvm::MachineFunction &MF) {
  const llvm::TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
  bool Changed = false;

  for (auto &MBB : MF) {
    llvm::MachineBasicBlock::iterator PrevInstr;
    for (auto I = MBB.begin(); I != MBB.end();) {
      if (I->getOpcode() == llvm::X86::MULPDrr) {
        auto MulInstr = I;
        PrevInstr = I;
        ++I;
        while (I != MBB.end()) {
          if (I->getOpcode() == llvm::X86::ADDPDrr) {
            if (MulInstr->getOperand(0).getReg() == I->getOperand(1).getReg()) {
              llvm::MachineInstrBuilder MIB =
                  BuildMI(MBB, *MulInstr, MulInstr->getDebugLoc(),
                          TII->get(llvm::X86::VFMADD213PDr));
              MIB.addReg(I->getOperand(0).getReg(), llvm::RegState::Define);
              MIB.addReg(MulInstr->getOperand(1).getReg());
              MIB.addReg(MulInstr->getOperand(2).getReg());
              MIB.addReg(I->getOperand(2).getReg());
              I = MBB.erase(I);
              MulInstr->eraseFromParent();
              Changed = true;
              break;
            }
          } else {
            break;
          }
          ++I;
        }
      } else {
        ++I;
      }
    }
  }
  return Changed;
}

}

INITIALIZE_PASS(X86PetrovMulAddPass, "x86-petrov-muladd", X86_MULADD_PASS_NAME,
                false, false)

namespace llvm {
FunctionPass *createX86PetrovMulAddPass() { return new X86PetrovMulAddPass(); }
}
