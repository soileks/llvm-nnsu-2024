; RUN: opt -load-pass-plugin=%llvmshlibdir/replaceMulWithBitShift%shlibext -passes=replaceMulWithBitShift -S %s | FileCheck %s

;int first_function() {
;  int x;
;  x = x * 16;
;  return 1;
;}

;int second_function(int t) {
;  int w = t * 8;
;  return w;
;}

;void third_function(int a) {
;  int r = 3 * 15;
;  a *= 32;
;}

;void four_function() {
;  int a;
;  int b = 5;
;  int c = c * 4;
;  c *= 64;
;  b = b * 7;
;  c *= 128;
;  b *= 256;
;  b = b * 15;
;}

;int five_function() {
;  int x = 4;
;  return x;
;}

define dso_local noundef i32 @_Z14first_functionv() #0 {
  %1 = alloca i32, align 4
  %2 = load i32, ptr %1, align 4
  %3 = mul nsw i32 %2, 16
  store i32 %3, ptr %1, align 4
  ret i32 1
}

define dso_local noundef i32 @_Z15second_functioni(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  %5 = mul nsw i32 %4, 8
  store i32 %5, ptr %3, align 4
  %6 = load i32, ptr %3, align 4
  ret i32 %6
}

define dso_local void @_Z14third_functioni(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  store i32 45, ptr %3, align 4
  %4 = load i32, ptr %2, align 4
  %5 = mul nsw i32 %4, 32
  store i32 %5, ptr %2, align 4
  ret void
}

define dso_local void @_Z13four_functionv() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 5, ptr %2, align 4
  %4 = load i32, ptr %3, align 4
  %5 = mul nsw i32 %4, 4
  store i32 %5, ptr %3, align 4
  %6 = load i32, ptr %3, align 4
  %7 = mul nsw i32 %6, 64
  store i32 %7, ptr %3, align 4
  %8 = load i32, ptr %2, align 4
  %9 = mul nsw i32 %8, 7
  store i32 %9, ptr %2, align 4
  %10 = load i32, ptr %3, align 4
  %11 = mul nsw i32 %10, 128
  store i32 %11, ptr %3, align 4
  %12 = load i32, ptr %2, align 4
  %13 = mul nsw i32 %12, 256
  store i32 %13, ptr %2, align 4
  %14 = load i32, ptr %2, align 4
  %15 = mul nsw i32 %14, 15
  store i32 %15, ptr %2, align 4
  ret void
}

define dso_local noundef i32 @_Z13five_functionv() #0 {
  %1 = alloca i32, align 4
  store i32 4, ptr %1, align 4
  %2 = load i32, ptr %1, align 4
  ret i32 %2
}

; CHECK-LABEL: @_Z14first_functionv
; CHECK: %1 = alloca i32, align 4
; CHECK: %2 = load i32, ptr %1, align 4
; CHECK: %3 = shl i32 %2, 4
; CHECK: store i32 %3, ptr %1, align 4
; CHECK: ret i32 1

; CHECK-LABEL: @_Z15second_functioni
; CHECK: %2 = alloca i32, align 4
; CHECK: %3 = alloca i32, align 4
; CHECK: store i32 %0, ptr %2, align 4
; CHECK: %4 = load i32, ptr %2, align 4
; CHECK: %5 = shl i32 %4, 3
; CHECK: store i32 %5, ptr %3, align 4
; CHECK: %6 = load i32, ptr %3, align 4
; CHECK: ret i32 %6


; CHECK-LABEL: @_Z14third_functioni
; CHECK: %2 = alloca i32, align 4
; CHECK: %3 = alloca i32, align 4
; CHECK: store i32 %0, ptr %2, align 4
; CHECK: store i32 45, ptr %3, align 4
; CHECK: %4 = load i32, ptr %2, align 4
; CHECK: %5 = shl i32 %4, 5
; CHECK: store i32 %5, ptr %2, align 4
; CHECK: ret void


; CHECK-LABEL: @_Z13four_functionv
; CHECK: %1 = alloca i32, align 4
; CHECK: %2 = alloca i32, align 4
; CHECK: %3 = alloca i32, align 4
; CHECK: store i32 5, ptr %2, align 4
; CHECK: %4 = load i32, ptr %3, align 4
; CHECK: %5 = shl i32 %4, 2
; CHECK: store i32 %5, ptr %3, align 4
; CHECK: %6 = load i32, ptr %3, align 4
; CHECK: %7 = shl i32 %6, 6
; CHECK: store i32 %7, ptr %3, align 4
; CHECK: %8 = load i32, ptr %2, align 4
; CHECK: %9 = mul nsw i32 %8, 7
; CHECK: store i32 %9, ptr %2, align 4
; CHECK: %10 = load i32, ptr %3, align 4
; CHECK: %11 = shl i32 %10, 7
; CHECK: store i32 %11, ptr %3, align 4
; CHECK: %12 = load i32, ptr %2, align 4
; CHECK: %13 = shl i32 %12, 8
; CHECK: store i32 %13, ptr %2, align 4
; CHECK: %14 = load i32, ptr %2, align 4
; CHECK: %15 = mul nsw i32 %14, 15
; CHECK: store i32 %15, ptr %2, align 4
; CHECK: ret void


; CHECK-LABEL: @_Z13five_functionv
; CHECK: %1 = alloca i32, align 4
; CHECK: store i32 4, ptr %1, align 4
; CHECK: %2 = load i32, ptr %1, align 4
; CHECK: ret i32 %2
