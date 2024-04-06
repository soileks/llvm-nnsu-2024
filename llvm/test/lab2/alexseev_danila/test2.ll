; RUN: opt -load-pass-plugin=%llvmshlibdir/AlexseevLoopPlugin%shlibext -passes=alexseev-loop-plugin -S %s | FileCheck %s

; void whileFooWLoopBorderingCall() {
;     int i = 10;
;     while (i > 0) {
;         i--;
;     }
; }

define dso_local void @whileFooWLoopBorderingCall() {
entry:
  %i = alloca i32, align 4
  store i32 10, ptr %i, align 4
  call void @loop_start()

  ; CHECK: call void @loop_start()
  ; CHECK-NEXT: br label %while.cond
  
  br label %while.cond

while.cond:
  %0 = load i32, ptr %i, align 4
  %cmp = icmp sgt i32 %0, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:
  %1 = load i32, ptr %i, align 4
  %dec = add nsw i32 %1, -1
  store i32 %dec, ptr %i, align 4
  br label %while.cond

  ; CHECK: call void @loop_end()
  ; CHECK-NEXT: ret void

while.end:
  call void @loop_end()
  ret void
}

declare void @loop_start()
declare void @loop_end()