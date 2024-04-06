; RUN: opt -load-pass-plugin=%llvmshlibdir/LoopLLVMPlugin_SadikovD%pluginext -passes=LoopLLVMPlugin_SadikovD -S %s | FileCheck %s

; source code:

; int func(int n) {
; 	int a = 1;
; 	for (int i = 0; i < n; i++) {
; 		a++;
; 	}
; 	return a;
; }

define dso_local i32 @func(i32 noundef %n) {
entry:
  %n.addr = alloca i32, align 4
  %a = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %n, ptr %n.addr, align 4
  store i32 1, ptr %a, align 4
  store i32 0, ptr %i, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %for.cond
  call void @loop_start()
  br label %for.cond

for.cond:
  %0 = load i32, ptr %i, align 4
  %1 = load i32, ptr %n.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %2 = load i32, ptr %a, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %a, align 4
  br label %for.inc

for.inc:
  %3 = load i32, ptr %i, align 4
  %inc1 = add nsw i32 %3, 1
  store i32 %inc1, ptr %i, align 4
  br label %for.cond

for.end:
; CHECK: call void @loop_end()
; CHECK-NEXT: %4 = load i32, ptr %a, align 4
  call void @loop_end()
  %4 = load i32, ptr %a, align 4
  ret i32 %4
}

declare void @loop_start()

declare void @loop_end()
