; RUN: opt -load-pass-plugin=%llvmshlibdir/loopStartEndPlugin%shlibext -passes=loop-start-end -S %s | FileCheck %s

; void foo(int n, int m) {
;     int c0;
;     int c1;
;     for (c0 = n; c0 > 0; c0--) {
;         c1++;
;     }
; }
;
; void a() {
;     int c = 10;
;     for (int i = 0; i < 10; i++) {
;         c++;
;     }
;     int q = c + 42;
; }
;
; void SomeFunction() {
;     int k = 0;
;     while (k < 10) {
;         k++;
;     }
; }
;
; void SomeFunction_123() {
;     int k = 0;
;     do {
;         k++;
;     } while (k < 10);
; }
;
; void SomeFunctionWithSwitch() {
;     int k = 0;
;     switch (k){
;         case(1): break;
;         default: break;
;     }
; }
;
; void LoopWithRet() {
;     int i = 0;
;     while(true){
;         i++;
;         if (i > 10) {
;             return;
;         }
;     }
; }
;
; int LoopWith2Exit(int &a) {
;     int i = 0;
;     while(i < 100){
;         i++;
;         if (i > 10) {
;             return 1;
;         }
;         a = a + i;
;     }
;     a *= i - 1;
;     return 2;
; }
;
; void loop_start();
; void loop_end();
;
; void FunctionWithLoop_(){
;     int i = 0;
;     loop_start();
;     while(i < 10){
;         i++;
;     }
;     loop_end();
; }


define dso_local void @_Z3fooii(i32 noundef %n, i32 noundef %m) {
entry:
  %n.addr = alloca i32, align 4
  %m.addr = alloca i32, align 4
  %c0 = alloca i32, align 4
  %c1 = alloca i32, align 4
  store i32 %n, ptr %n.addr, align 4
  store i32 %m, ptr %m.addr, align 4
  %0 = load i32, ptr %n.addr, align 4
  store i32 %0, ptr %c0, align 4
; CHECK: call void @loop_start()
  br label %for.cond

for.cond:
  %1 = load i32, ptr %c0, align 4
  %cmp = icmp sgt i32 %1, 0
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %2 = load i32, ptr %c1, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %c1, align 4
  br label %for.inc

for.inc:
  %3 = load i32, ptr %c0, align 4
  %dec = add nsw i32 %3, -1
  store i32 %dec, ptr %c0, align 4
  br label %for.cond

for.end:
; CHECK: call void @loop_end()
  ret void
}


define dso_local void @a() {
entry:
  %c = alloca i32, align 4
  %i = alloca i32, align 4
  %q = alloca i32, align 4
  store i32 10, ptr %c, align 4
  store i32 0, ptr %i, align 4
; CHECK:    call void @loop_start()
  br label %for.cond

for.cond:
  %0 = load i32, ptr %i, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %1 = load i32, ptr %c, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr %c, align 4
  br label %for.inc

for.inc:
  %2 = load i32, ptr %i, align 4
  %inc1 = add nsw i32 %2, 1
  store i32 %inc1, ptr %i, align 4
  br label %for.cond

for.end:
; CHECK:    call void @loop_end()
  %3 = load i32, ptr %c, align 4
  %add = add nsw i32 %3, 42
  store i32 %add, ptr %q, align 4
  ret void
}

define dso_local void @SomeFunction() {
entry:
  %k = alloca i32, align 4
  store i32 0, ptr %k, align 4
; CHECK: call void @loop_start()
  br label %while.cond

while.cond:
  %0 = load i32, ptr %k, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %while.body, label %while.end

while.body:
  %1 = load i32, ptr %k, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr %k, align 4
  br label %while.cond

while.end:
; CHECK: call void @loop_end()
  ret void
}

define dso_local void @SomeFunction_123() {
entry:
  %k = alloca i32, align 4
  store i32 0, ptr %k, align 4
; CHECK: call void @loop_start()
  br label %do.body

do.body:
  %0 = load i32, ptr %k, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, ptr %k, align 4
  br label %do.cond

do.cond:
  %1 = load i32, ptr %k, align 4
  %cmp = icmp slt i32 %1, 10
  br i1 %cmp, label %do.body, label %do.end

do.end:
; CHECK: call void @loop_end()
  ret void
}

; CHECK-LABEL:  @SomeFunctionWithSwitch
; CHECK-NOT: call void @loop_start()
; CHECK-NOT: call void @loop_end()
; CHECK: ret void
define dso_local void @SomeFunctionWithSwitch() {
entry:
  %k = alloca i32, align 4
  store i32 0, ptr %k, align 4
  %0 = load i32, ptr %k, align 4
  switch i32 %0, label %sw.default [
    i32 1, label %sw.bb
  ]

sw.bb:
  br label %sw.epilog

sw.default:
  br label %sw.epilog

sw.epilog:
  ret void
}

define dso_local void @LoopWithRet() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
; CHECK: call void @loop_start()
  br label %while.body

while.body:
  %0 = load i32, ptr %i, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, ptr %i, align 4
  %1 = load i32, ptr %i, align 4
  %cmp = icmp sgt i32 %1, 10
  br i1 %cmp, label %if.then, label %if.end

if.then:
; CHECK: call void @loop_end()
  ret void

if.end:
  br label %while.body
}

define dso_local noundef i32 @_Z13LoopWith2ExitRi(ptr noundef nonnull align 4 dereferenceable(4) %a) {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca ptr, align 8
  %i = alloca i32, align 4
  store ptr %a, ptr %a.addr, align 8
  store i32 0, ptr %i, align 4
; CHECK: call void @loop_start()
  br label %while.cond

while.cond:
  %0 = load i32, ptr %i, align 4
  %cmp = icmp slt i32 %0, 100
  br i1 %cmp, label %while.body, label %while.end

while.body:
  %1 = load i32, ptr %i, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr %i, align 4
  %2 = load i32, ptr %i, align 4
  %cmp1 = icmp sgt i32 %2, 10
  br i1 %cmp1, label %if.then, label %if.end

if.then:
  store i32 1, ptr %retval, align 4
; CHECK: call void @loop_end()
  br label %return

if.end:
  %3 = load ptr, ptr %a.addr, align 8
  %4 = load i32, ptr %3, align 4
  %5 = load i32, ptr %i, align 4
  %add = add nsw i32 %4, %5
  %6 = load ptr, ptr %a.addr, align 8
  store i32 %add, ptr %6, align 4
  br label %while.cond

while.end:
  %7 = load i32, ptr %i, align 4
  %sub = sub nsw i32 %7, 1
  %8 = load ptr, ptr %a.addr, align 8
  %9 = load i32, ptr %8, align 4
  %mul = mul nsw i32 %9, %sub
  store i32 %mul, ptr %8, align 4
  store i32 2, ptr %retval, align 4
; CHECK: call void @loop_end()
  br label %return

return:
  %10 = load i32, ptr %retval, align 4
  ret i32 %10
}

define dso_local void @FunctionWithLoop_() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  call void @loop_start()
  br label %while.cond
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %while.cond

while.cond:
  %0 = load i32, ptr %i, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %while.body, label %while.end

while.body:
  %1 = load i32, ptr %i, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr %i, align 4
  br label %while.cond

while.end:
; CHECK: call void @loop_end()
; CHECK-NEXT: ret void
  call void @loop_end()
  ret void
}

declare void @loop_start()

declare void @loop_end()
