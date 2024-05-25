// RUN: mlir-opt -load-pass-plugin=%mlir_lib_dir/KulaginAleksandrFMAPass%shlibext --pass-pipeline="builtin.module(KulaginAleksandrFMA)" %s | FileCheck %s

//------------C

//double f1(double a, double b, double c) {
//  double d = a + b * c;
//  return d;
//}
//
//double f2(double a, double b, double c) {
//  double d = a - b * c;
//  return d;
//}
//
//double f3(double a, double b, double c) {
//  double d = b * c - a;
//  return d;
//}
//
//double f4(double a, double b, double c) {
//  double d = a + b / c;
//  return d;
//}
//
//double f5(double a, double b, double c) {
//  double d = a + b + c;
//  return d;
//}
//
//double f6(double a, double b, double c) {
//  double e = a * b;
//  double d = e + c;
//  double f = d / e + 1;
//  return f;
//}
//
//double f7(double a, double b) {
//  double c = a * b;
//  double d = c + c;
//  return d;
//}

//------------llvm ir

//define dso_local double @_Z2f1ddd(double %0, double %1, double %2) local_unnamed_addr {
//  %4 = fmul double %1, %2
//  %5 = fadd double %4, %0
//  ret double %5
//}
//
//define dso_local double @_Z2f2ddd(double %0, double %1, double %2) local_unnamed_addr {
//  %4 = fmul double %1, %2
//  %5 = fsub double %0, %4
//  ret double %5
//}
//
//define dso_local double @_Z2f3ddd(double %0, double %1, double %2) local_unnamed_addr {
//  %4 = fmul double %1, %2
//  %5 = fsub double %4, %0
//  ret double %5
//}
//
//define dso_local double @_Z2f4ddd(double %0, double %1, double %2) local_unnamed_addr {
//  %4 = fdiv double %1, %2
//  %5 = fadd double %4, %0
//  ret double %5
//}
//
//define dso_local double @_Z2f5ddd(double %0, double %1, double %2) local_unnamed_addr {
//  %4 = fadd double %0, %1
//  %5 = fadd double %4, %2
//  ret double %5
//}
//
//define dso_local double @_Z2f6ddd(double %0, double %1, double %2) local_unnamed_addr {
//  %4 = fmul double %0, %1
//  %5 = fadd double %4, %2
//  %6 = fdiv double %5, %4
//  %7 = fadd double %6, 1.000000e+00
//  ret double %7
//}
//
//define dso_local double @_Z2f7dd(double %0, double %1) local_unnamed_addr {
//  %3 = fmul double %0, %1
//  %4 = fadd double %3, %3
//  ret double %4
//}

module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i64, dense<[32, 64]> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<"dlti.endianness", "little">>} {
  llvm.func local_unnamed_addr @_Z2f1ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
    %0 = llvm.fmul %arg1, %arg2  : f64
    %1 = llvm.fadd %0, %arg0  : f64
    llvm.return %1 : f64
  }
  llvm.func local_unnamed_addr @_Z2f2ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
    %0 = llvm.fmul %arg1, %arg2  : f64
    %1 = llvm.fsub %arg0, %0  : f64
    llvm.return %1 : f64
  }
  llvm.func local_unnamed_addr @_Z2f3ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
    %0 = llvm.fmul %arg1, %arg2  : f64
    %1 = llvm.fsub %0, %arg0  : f64
    llvm.return %1 : f64
  }
  llvm.func local_unnamed_addr @_Z2f4ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
    %0 = llvm.fdiv %arg1, %arg2  : f64
    %1 = llvm.fadd %0, %arg0  : f64
    llvm.return %1 : f64
  }
  llvm.func local_unnamed_addr @_Z2f5ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
    %0 = llvm.fadd %arg0, %arg1  : f64
    %1 = llvm.fadd %0, %arg2  : f64
    llvm.return %1 : f64
  }
  llvm.func local_unnamed_addr @_Z2f6ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
    %0 = llvm.mlir.constant(1.000000e+00 : f64) : f64
    %1 = llvm.fmul %arg0, %arg1  : f64
    %2 = llvm.fadd %1, %arg2  : f64
    %3 = llvm.fdiv %2, %1  : f64
    %4 = llvm.fadd %3, %0  : f64
    llvm.return %4 : f64
  }
  llvm.func local_unnamed_addr @_Z2f7dd(%arg0: f64, %arg1: f64) -> f64 {
    %0 = llvm.fmul %arg0, %arg1  : f64
    %1 = llvm.fadd %0, %0  : f64
    llvm.return %1 : f64
  }
}

// COM: f1
// COM: expect fma
// CHECK: llvm.func local_unnamed_addr @_Z2f1ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
// CHECK-NEXT: %0 = llvm.intr.fma(%arg1, %arg2, %arg0)  : (f64, f64, f64) -> f64
// CHECK-NEXT: llvm.return %0 : f64

// COM: f2
// COM: expect fma
// COM: fma(-b, c, a)
// CHECK: llvm.func local_unnamed_addr @_Z2f2ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
// CHECK-NEXT: %0 = llvm.fneg %arg1  : f64
// CHECK-NEXT: %1 = llvm.intr.fma(%0, %arg2, %arg0)  : (f64, f64, f64) -> f64
// CHECK-NEXT: llvm.return %1 : f64

// COM: f3
// COM: expect fma
// COM: fma(b, c, -a)
// CHECK: llvm.func local_unnamed_addr @_Z2f3ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
// CHECK-NEXT: %0 = llvm.fneg %arg0  : f64
// CHECK-NEXT: %1 = llvm.intr.fma(%arg1, %arg2, %0)  : (f64, f64, f64) -> f64
// CHECK-NEXT: llvm.return %1 : f64

// COM: f4
// COM: no fma
// CHECK: llvm.func local_unnamed_addr @_Z2f4ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
// CHECK-NEXT: %0 = llvm.fdiv %arg1, %arg2  : f64
// CHECK-NEXT: %1 = llvm.fadd %0, %arg0  : f64
// CHECK-NEXT: llvm.return %1 : f64

// COM: f5
// COM: no fma
// CHECK: llvm.func local_unnamed_addr @_Z2f5ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
// CHECK-NEXT: %0 = llvm.fadd %arg0, %arg1  : f64
// CHECK-NEXT: %1 = llvm.fadd %0, %arg2  : f64
// CHECK-NEXT: llvm.return %1 : f64

// COM: f6
// COM: expect no fma (don't just replace fadd with fma)
// CHECK: llvm.func local_unnamed_addr @_Z2f6ddd(%arg0: f64, %arg1: f64, %arg2: f64) -> f64 {
// CHECK-NEXT: %0 = llvm.mlir.constant(1.000000e+00 : f64) : f64
// CHECK-NEXT: %1 = llvm.fmul %arg0, %arg1  : f64
// CHECK-NEXT: %2 = llvm.fadd %1, %arg2  : f64
// CHECK-NEXT: %3 = llvm.fdiv %2, %1  : f64
// CHECK-NEXT: %4 = llvm.fadd %3, %0  : f64
// CHECK-NEXT: llvm.return %4 : f64

// COM: f7
// COM: expect no fma (don't just replace fadd with fma)
// CHECK: llvm.func local_unnamed_addr @_Z2f7dd(%arg0: f64, %arg1: f64) -> f64 {
// CHECK-NEXT: %0 = llvm.fmul %arg0, %arg1  : f64
// CHECK-NEXT: %1 = llvm.fadd %0, %0  : f64
// CHECK-NEXT: llvm.return %1 : f64
