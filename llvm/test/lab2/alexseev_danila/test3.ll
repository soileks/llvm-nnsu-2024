; RUN: opt -load-pass-plugin=%llvmshlibdir/AlexseevLoopPlugin%shlibext -passes=alexseev-loop-plugin -S %s | FileCheck %s

; void whileFoo() {
;     int i = 10;
;     while (i > 0) {
;         i--;
;         if (i == 3)
;             break;
;     }
; }

define dso_local void @whileFoo() {
entry:
  %i = alloca i32, align 4
  store i32 10, ptr %i, align 4
  br label %while.cond
  ;CHECK: call void @loop_start

while.cond:
  %0 = load i32, ptr %i, align 4
  %cmp = icmp sgt i32 %0, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:
  %1 = load i32, ptr %i, align 4
  %dec = add nsw i32 %1, -1
  store i32 %dec, ptr %i, align 4
  %2 = load i32, ptr %i, align 4
  %cmp1 = icmp eq i32 %2, 3
  br i1 %cmp1, label %if.then, label %if.end

if.then:
  br label %while.end

if.end:
  br label %while.cond

while.end:
  ret void
}