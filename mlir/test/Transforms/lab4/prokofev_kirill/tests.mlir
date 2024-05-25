// RUN: mlir-opt -load-pass-plugin=%mlir_lib_dir/ProkofevFuncCall%shlibext --pass-pipeline="builtin.module(ProkofevFuncCallCounter)" %s | FileCheck %s
module {
  llvm.func @_Z5func1v() -> (i32 {llvm.noundef}) attributes {passthrough = ["mustprogress", "noinline", "nounwind", "optnone", ["uwtable", "2"], ["frame-pointer", "all"], ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["stack-protector-buffer-size", "8"], ["target-cpu", "x86-64"], ["target-features", "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87"], ["tune-cpu", "generic"]]} {
    // CHECK: llvm.func @_Z5func1v() {{.*}}attributes {{.*}}CallCount = 6 : i32
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(5 : i32) : i32
    %2 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %1, %2 {alignment = 4 : i64} : i32, !llvm.ptr
    %3 = llvm.load %2 {alignment = 4 : i64} : !llvm.ptr -> i32
    llvm.return %3 : i32
  }
  llvm.func @_Z5func2i(%arg0: i32 {llvm.noundef}) -> (i32 {llvm.noundef}) attributes {passthrough = ["mustprogress", "noinline", "nounwind", "optnone", ["uwtable", "2"], ["frame-pointer", "all"], ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["stack-protector-buffer-size", "8"], ["target-cpu", "x86-64"], ["target-features", "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87"], ["tune-cpu", "generic"]]} {
    // CHECK: llvm.func @_Z5func2i(%arg0: i32 {llvm.noundef}) -> (i32 {llvm.noundef}) attributes {{.*}}CallCount = 2 : i32
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %arg0, %1 {alignment = 4 : i64} : i32, !llvm.ptr
    %2 = llvm.call @_Z5func1v() : () -> i32
    %3 = llvm.call @_Z5func1v() : () -> i32
    %4 = llvm.add %2, %3  : i32
    llvm.store %4, %1 {alignment = 4 : i64} : i32, !llvm.ptr
    %5 = llvm.load %1 {alignment = 4 : i64} : !llvm.ptr -> i32
    %6 = llvm.call @_Z5func1v() : () -> i32
    llvm.return %5 : i32
  }
  llvm.func @_Z5func3ii(%arg0: i32 {llvm.noundef}, %arg1: i32 {llvm.noundef}) attributes {passthrough = ["mustprogress", "noinline", "nounwind", "optnone", ["uwtable", "2"], ["frame-pointer", "all"], ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["stack-protector-buffer-size", "8"], ["target-cpu", "x86-64"], ["target-features", "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87"], ["tune-cpu", "generic"]]} {
    // CHECK: llvm.func @_Z5func3ii(%arg0: i32 {llvm.noundef}, %arg1: i32 {llvm.noundef}) attributes {{.*}}CallCount = 1 : i32
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(5 : i32) : i32
    %2 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    %3 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %arg0, %2 {alignment = 4 : i64} : i32, !llvm.ptr
    llvm.store %arg1, %3 {alignment = 4 : i64} : i32, !llvm.ptr
    %4 = llvm.load %3 {alignment = 4 : i64} : !llvm.ptr -> i32
    %5 = llvm.add %4, %1  : i32
    llvm.store %5, %2 {alignment = 4 : i64} : i32, !llvm.ptr
    %6 = llvm.load %2 {alignment = 4 : i64} : !llvm.ptr -> i32
    %7 = llvm.call @_Z5func2i(%6) : (i32) -> i32
    %8 = llvm.call @_Z5func1v() : () -> i32
    llvm.return
  }
  llvm.func @main() -> (i32 {llvm.noundef}) attributes {passthrough = ["mustprogress", "noinline", "norecurse", "nounwind", "optnone", ["uwtable", "2"], ["frame-pointer", "all"], ["min-legal-vector-width", "0"], ["no-trapping-math", "true"], ["stack-protector-buffer-size", "8"], ["target-cpu", "x86-64"], ["target-features", "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87"], ["tune-cpu", "generic"]]} {
    // CHECK: llvm.func @main() -> (i32 {llvm.noundef}) attributes {{.*}}CallCount = 0 : i32
    %0 = llvm.mlir.constant(1 : i32) : i32
    %1 = llvm.mlir.constant(0 : i32) : i32
    %2 = llvm.mlir.constant(2 : i32) : i32
    %3 = llvm.mlir.constant(3 : i32) : i32
    %4 = llvm.alloca %0 x i32 {alignment = 4 : i64} : (i32) -> !llvm.ptr
    llvm.store %1, %4 {alignment = 4 : i64} : i32, !llvm.ptr
    %5 = llvm.call @_Z5func1v() : () -> i32
    %6 = llvm.call @_Z5func2i(%0) : (i32) -> i32
    llvm.call @_Z5func3ii(%2, %3) : (i32, i32) -> ()
    %7 = llvm.call @_Z5func1v() : () -> i32
    llvm.return %1 : i32
  }
}
