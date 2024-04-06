; RUN: opt -load-pass-plugin=%llvmshlibdir/AtikinLoopPass%shlibext -passes=ivanov-loop-pass -S %s | FileCheck %s

; void foo(int n, int m) {
;  int c0;
;  int c1;
;  for (c0 = n; c0 > 0; c0--) {
;   c1++;
;  }
; }

; void while_test(){
;     int i = 0;
;     while(i < 10){
;         i++;
;     }
; }

; void do_while_test(){
;     int i = 0;
;     do {
;         i++;
;     } while(i < 10);
; }

; void no_loop(){
;     int a = 2;
;     if (a > 0){
;         a = 0;
;     } else {
;         a = 10;
;     }
; }

; void loop_start();
; void loop_end();

; void with_lopps_func(){
;     int i = 0;
;     loop_start();
;     while(i < 10){
;         i++;
;     } 
;     loop_end();
; }

; void LoopWithRet(int &a) {
;   int i = 0;
;   while(i < 100){
;          i++;
;          if (i > 10) {
;             return;
;          }
;          a = a + i;
;      }
;      a *= i -1;
;      return;
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
; CHECK:    call void @_Z10loop_startv()
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
; CHECK:    call void @_Z8loop_endv()
  ret void
}

define dso_local void @_Z10while_testv() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
; CHECK:    call void @_Z10loop_startv()
  br label %while.cond

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
; CHECK:    call void @_Z8loop_endv()
  ret void
}

define dso_local void @_Z13do_while_testv() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
; CHECK:    call void @_Z10loop_startv()
  br label %do.body

do.body:
  %0 = load i32, ptr %i, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, ptr %i, align 4
  br label %do.cond

do.cond:
  %1 = load i32, ptr %i, align 4
  %cmp = icmp slt i32 %1, 10
  br i1 %cmp, label %do.body, label %do.end

do.end:
; CHECK:    call void @_Z8loop_endv()
  ret void
}

define dso_local void @_Z7no_loopv() {
entry:
  %a = alloca i32, align 4
  store i32 2, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
; CHECK-NOT:    call void @_Z10loop_startv()
  %cmp = icmp sgt i32 %0, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:
  store i32 0, ptr %a, align 4
  br label %if.end

if.else:
  store i32 10, ptr %a, align 4
  br label %if.end

if.end:
; CHECK-NOT:    call void @_Z8loop_endv()
  ret void
}

define dso_local void @_Z15with_lopps_funcv() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
; CHECK:    call void @_Z10loop_startv()
; CHECK-NOT:    call void @_Z10loop_startv()
  br label %while.cond

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
; CHECK:    call void @_Z8loop_endv()
; CHECK-NOT:    call void @_Z8loop_endv()
  ret void
}

declare dso_local void @_Z10loop_startv()

declare dso_local void @_Z8loop_endv()

define dso_local void @_Z11LoopWithRetRi(ptr noundef nonnull align 4 dereferenceable(4) %a) {
entry:
  %a.addr = alloca ptr, align 8
  %i = alloca i32, align 4
  store ptr %a, ptr %a.addr, align 8
  store i32 0, ptr %i, align 4
; CHECK:  call void @_Z10loop_startv()
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
; CHECK:  call void @_Z8loop_endv()
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
; CHECK:  call void @_Z8loop_endv()
  %7 = load i32, ptr %i, align 4
  %sub = sub nsw i32 %7, 1
  %8 = load ptr, ptr %a.addr, align 8
  %9 = load i32, ptr %8, align 4
  %mul = mul nsw i32 %9, %sub
  store i32 %mul, ptr %8, align 4
  br label %return

return:
  ret void
}
