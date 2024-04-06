; RUN: opt -load-pass-plugin %llvmshlibdir/Replace-Mult-Shift_Korablev-Nikita_FIIT3%pluginext\
; RUN: -passes=korablev-replace-mul-shift -S %s | FileCheck %s

; int test1() {
;     int b = 3;
;     int res = b * 4;
;     return res;
; }

; int test2() {
;     int b = 3;
;     int res = 4 * b;
;     return res;
; }

; int test3(int a) {
;     int res = 4 * a;
;     return res;
; }

; int test4() {
;     int a = 3;
;     int res = 4 + a;
;     return res;
; }

; int test5() {
;     int a = 3;
;     return a * 4;
; }

; int test6() {
;     int a = 3;
;     return 4 * a;
; }

define dso_local noundef i32 @_Z5test1v() #0 {
entry:
  %b = alloca i32, align 4
  %res = alloca i32, align 4
  store i32 3, ptr %b, align 4
  %0 = load i32, ptr %b, align 4
  %mul = mul nsw i32 %0, 4
  store i32 %mul, ptr %res, align 4
  %1 = load i32, ptr %res, align 4
  ret i32 %1
}

; CHECK-LABEL: @_Z5test1v
; CHECK: %shiftInst = shl i32 %0, 2
; CHECK-NEXT: store i32 %shiftInst, ptr %res, align 4

define dso_local noundef i32 @_Z5test2v() #0 {
entry:
  %b = alloca i32, align 4
  %res = alloca i32, align 4
  store i32 3, ptr %b, align 4
  %0 = load i32, ptr %b, align 4
  %mul = mul nsw i32 4, %0
  store i32 %mul, ptr %res, align 4
  %1 = load i32, ptr %res, align 4
  ret i32 %1
}

; CHECK-LABEL: @_Z5test2v
; CHECK: %shiftInst = shl i32 %0, 2
; CHECK-NEXT: store i32 %shiftInst, ptr %res, align 4

define dso_local noundef i32 @_Z5test3i(i32 noundef %a) #0 {
entry:
  %a.addr = alloca i32, align 4
  %res = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  %0 = load i32, ptr %a.addr, align 4
  %mul = mul nsw i32 4, %0
  store i32 %mul, ptr %res, align 4
  %1 = load i32, ptr %res, align 4
  ret i32 %1
}

; CHECK-LABEL: @_Z5test3i
; CHECK: %shiftInst = shl i32 %0, 2
; CHECK-NEXT: store i32 %shiftInst, ptr %res, align 4

define dso_local noundef i32 @_Z5test4v() #0 {
entry:
  %a = alloca i32, align 4
  %res = alloca i32, align 4
  store i32 3, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
  %add = add nsw i32 4, %0
  store i32 %add, ptr %res, align 4
  %1 = load i32, ptr %res, align 4
  ret i32 %1
}

; CHECK-LABEL: @_Z5test4v
; CHECK: %add = add nsw i32 4, %0
; CHECK-NEXT: store i32 %add, ptr %res, align 4

define dso_local noundef i32 @_Z5test5v() #0 {
entry:
  %a = alloca i32, align 4
  store i32 3, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
  %mul = mul nsw i32 %0, 4
  ret i32 %mul
}

; CHECK-LABEL: @_Z5test5v
; CHECK: %shiftInst = shl i32 %0, 2
; CHECK-NEXT: ret i32 %shiftInst

define dso_local noundef i32 @_Z5test6v() #0 {
entry:
  %a = alloca i32, align 4
  store i32 3, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
  %mul = mul nsw i32 4, %0
  ret i32 %mul
}

; CHECK-LABEL: @_Z5test6v
; CHECK: %shiftInst = shl i32 %0, 2
; CHECK-NEXT: ret i32 %shiftInst