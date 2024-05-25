#include "X86.h"
#include "X86InstrInfo.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Register.h"
#include <utility>
#include <vector>

using namespace llvm;

namespace {
class PushkarevX86PassFMA : public MachineFunctionPass {
public:
  static char ID;

  PushkarevX86PassFMA() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    bool modified = false;
    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
    std::vector<std::pair<MachineInstr *, MachineInstr *>> toReplace;

    for (MachineBasicBlock &MBB : MF) {
      for (auto Instr = MBB.begin(); Instr != MBB.end(); ++Instr) {
        MachineInstr *MulInstr = &(*Instr);

        if (MulInstr->getOpcode() == X86::MULPDrr ||
            MulInstr->getOpcode() == X86::MULPDrm) {
          MachineInstr *AddInstr = nullptr;
          Register MulDestReg = MulInstr->getOperand(0).getReg();

          for (auto NextInstr = std::next(Instr); NextInstr != MBB.end();
               ++NextInstr) {
            if ((NextInstr->getOpcode() == X86::ADDPDrr ||
                 NextInstr->getOpcode() == X86::ADDPDrm) &&
                MulDestReg == NextInstr->getOperand(1).getReg()) {
              AddInstr = &(*NextInstr);
              break;
            }
          }

          if (AddInstr && MulDestReg != AddInstr->getOperand(2).getReg()) {
            toReplace.emplace_back(MulInstr, AddInstr);
          }
        }
      }
    }

    for (auto &[MulInstr, AddInstr] : toReplace) {
      MachineBasicBlock &MBB = *MulInstr->getParent();
      MIMetadata MetaData(*MulInstr);

      BuildMI(MBB, MulInstr, MetaData, TII->get(X86::VFMADD213PDr),
              AddInstr->getOperand(0).getReg())
          .addReg(MulInstr->getOperand(1).getReg())  // a
          .addReg(MulInstr->getOperand(2).getReg())  // b
          .addReg(AddInstr->getOperand(2).getReg()); // c

      MulInstr->eraseFromParent();
      AddInstr->eraseFromParent();

      modified = true;
    }

    return modified;
  }
};

char PushkarevX86PassFMA::ID = 0;
} // namespace

static RegisterPass<PushkarevX86PassFMA>
    X("pushkarev-x86-pass-fma", "Multiply-Add Optimization Pass", false, false);
