; RUN: opt -load-pass-plugin %llvmshlibdir/kosarev_egor_inline_pass_plugin%pluginext\
; RUN: -passes=custom-inlining -no-loop-inline -S %s | FileCheck %s

; void foo() {
;     int a = 0;
;     int i = 0;
;     for (int i = 0; i < 10; i++) {
;         a += 3;
;         int b = 2 + i;
;         i++;
;     }
; }
;
; int func() {
;     int a = 3;
;     foo();
;     a += 4;
;     return 0;
; }

define dso_local void @_Z3foov() #0 {
entry:
  %a = alloca i32, align 4
  %i = alloca i32, align 4
  %i1 = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 0, ptr %a, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %i1, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, ptr %i1, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32, ptr %a, align 4
  %add = add nsw i32 %1, 3
  store i32 %add, ptr %a, align 4
  %2 = load i32, ptr %i1, align 4
  %add2 = add nsw i32 2, %2
  store i32 %add2, ptr %b, align 4
  %3 = load i32, ptr %i1, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, ptr %i1, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %4 = load i32, ptr %i1, align 4
  %inc3 = add nsw i32 %4, 1
  store i32 %inc3, ptr %i1, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}

define dso_local noundef i32 @_Z4funcv() #0 {
entry:
  %a = alloca i32, align 4
  store i32 3, ptr %a, align 4
  call void @_Z3foov()
  %0 = load i32, ptr %a, align 4
  %add = add nsw i32 %0, 4
  store i32 %add, ptr %a, align 4
  ret i32 0
}

; CHECK: define dso_local noundef i32 @_Z4funcv() {
; CHECK-NEXT: entry:
; CHECK-NEXT: %a = alloca i32, align 4
; CHECK-NEXT: store i32 3, ptr %a, align 4
; CHECK-NEXT: call void @_Z3foov()
; CHECK-NEXT: %0 = load i32, ptr %a, align 4
; CHECK-NEXT: %add = add nsw i32 %0, 4
; CHECK-NEXT: store i32 %add, ptr %a, align 4
; CHECK-NEXT: ret i32 0
; CHECK-NEXT: }

; void foo_inline() {
;     float a = 3.0;
;     a += 2.0;
; }
;
; int func1() {
;     int a = 2;
;     foo_inline();
;     int b = 3 + a;
;     return 0;
; }

define dso_local void @_Z10foo_inlinev() #0 {
entry:
  %a = alloca float, align 4
  store float 3.000000e+00, ptr %a, align 4
  %0 = load float, ptr %a, align 4
  %conv = fpext float %0 to double
  %add = fadd double %conv, 2.000000e+00
  %conv1 = fptrunc double %add to float
  store float %conv1, ptr %a, align 4
  ret void
}

define dso_local noundef i32 @_Z5func1v() #0 {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 2, ptr %a, align 4
  call void @_Z10foo_inlinev()
  %0 = load i32, ptr %a, align 4
  %add = add nsw i32 3, %0
  store i32 %add, ptr %b, align 4
  ret i32 0
}

; CHECK: define dso_local noundef i32 @_Z5func1v() {
; CHECK-NEXT: entry:
; CHECK-NEXT: %a = alloca i32, align 4
; CHECK-NEXT: %b = alloca i32, align 4
; CHECK-NEXT: store i32 2, ptr %a, align 4
; CHECK-NEXT: br label %entry.inlined.0
; CHECK-EMPTY:
; CHECK: entry.inlined.0:
; CHECK-NEXT: %0 = alloca float, align 4
; CHECK-NEXT: store float 3.000000e+00, ptr %0, align 4
; CHECK-NEXT: %1 = load float, ptr %0, align 4
; CHECK-NEXT: %2 = fpext float %1 to double
; CHECK-NEXT: %3 = fadd double %2, 2.000000e+00
; CHECK-NEXT: %4 = fptrunc double %3 to float
; CHECK-NEXT: store float %4, ptr %0, align 4
; CHECK-NEXT: br label %entry.splited.0
; CHECK-EMPTY:
; CHECK: entry.splited.0:
; CHECK-NEXT: %5 = load i32, ptr %a, align 4
; CHECK-NEXT: %add = add nsw i32 3, %5
; CHECK-NEXT: store i32 %add, ptr %b, align 4
; CHECK-NEXT: ret i32 0
; CHECK-NEXT: }
