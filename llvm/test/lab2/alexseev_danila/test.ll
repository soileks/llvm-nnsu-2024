; RUN: opt -load-pass-plugin=%llvmshlibdir/AlexseevLoopPlugin%shlibext -passes=alexseev-loop-plugin -S %s | FileCheck %s

; void forFoo() {
;     int i = 5;
;     for (int i = 0; i < 5; i++) {
;         i++;
;     }
; }

; void foo() {
;     if (1) {
;         return;
;     }
; }

define dso_local void @forFoo() {
entry:
  %i = alloca i32, align 4
  store i32 5, ptr %i, align 4

  ; CHECK: call void @loop_start()
  ; CHECK-NEXT: br label %for.cond

  br label %for.cond

for.cond:
  %0 = load i32, ptr %i, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %1 = load i32, ptr %i, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr %i, align 4
  br label %for.inc

for.inc:
  br label %for.cond

  ; CHECK: for.end:
  ; CHECK-NEXT: call void @loop_end()

for.end:
  ret void
}

; CHECK-NOT: call void @loop_start()
; CHECK-NOT: call void @loop_end()

define dso_local void @foo() {
entry:
  br label %if.then

if.then:
  ret void
}
