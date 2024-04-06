; RUN: opt -load-pass-plugin=%llvmshlibdir/PolozovLoopPass%pluginext -passes=polozov-loop-plugin -S %s | FileCheck %s

define dso_local i32 @bar() {
entry:
  %retval = alloca i32, align 4
  %n = alloca i32, align 4
  %t = alloca i32, align 4
  %i = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  store i32 10, ptr %n, align 4
  store i32 30, ptr %t, align 4
  store i32 0, ptr %i, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %for.cond

  call void @loop_start()
  br label %for.cond

for.cond:                                         
  %0 = load i32, ptr %i, align 4
  %1 = load i32, ptr %n, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         
  %2 = load i32, ptr %t, align 4
  %mul = mul nsw i32 %2, 25
  store i32 %mul, ptr %x, align 4
  %3 = load i32, ptr %t, align 4
  %add = add nsw i32 %3, 3
  store i32 %add, ptr %t, align 4
  %4 = load i32, ptr %t, align 4
  %div = sdiv i32 %4, 5
  store i32 %div, ptr %t, align 4
  br label %for.inc

for.inc:                                          
  %5 = load i32, ptr %i, align 4
  %inc = add nsw i32 %5, 1
  store i32 %inc, ptr %i, align 4
  br label %for.cond

for.end:
; CHECK: call void @loop_end()
; CHECK-NEXT: ret i32 0

  call void @loop_end()
  ret i32 0
}

declare void @loop_start()

declare void @loop_end()
