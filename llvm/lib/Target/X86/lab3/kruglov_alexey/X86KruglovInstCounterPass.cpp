#include "X86.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

namespace {
class X86KruglovCntPass : public MachineFunctionPass {
public:
  static inline char ID = 0;

  X86KruglovCntPass() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    auto *GV = MF.getFunction().getParent()->getNamedGlobal("ic");
    if (!GV) {
      LLVMContext &context = MF.getFunction().getParent()->getContext();
      GV = new GlobalVariable(*MF.getFunction().getParent(),
                              IntegerType::get(context, 64), false,
                              GlobalValue::ExternalLinkage, nullptr, "ic");
      GV->setAlignment(Align(8));
      if (!GV) {
        return false;
      }
    }
    DebugLoc DebugLocation = MF.front().begin()->getDebugLoc();
    const TargetInstrInfo *InstrInfo = MF.getSubtarget().getInstrInfo();
    for (auto &MBB : MF) {
      BuildMI(MBB, MBB.getFirstTerminator(), DebugLocation,
              InstrInfo->get(X86::ADD64mi32))
          .addReg(0)
          .addImm(1)
          .addReg(0)
          .addGlobalAddress(GV)
          .addReg(0)
          .addImm(MBB.size());
    }
    return true;
  }
};

} // end anonymous namespace

static RegisterPass<X86KruglovCntPass>
    X("x86-kruglov-cnt-pass", "Instruction counter pass", false, false);