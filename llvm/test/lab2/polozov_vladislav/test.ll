; RUN: opt -load-pass-plugin=%llvmshlibdir/PolozovLoopPass%pluginext -passes=polozov-loop-plugin -S %s | FileCheck %s

; CHECK-LABEL: @main
; CHECK-NOT: call void @loop_
define dso_local i32 @main() {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 10, i32* %a, align 4
  ret i32 0
}


define dso_local i32 @foo() {
entry:
  %retval = alloca i32, align 4
  %n = alloca i32, align 4
  %t = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 10, i32* %n, align 4
  store i32 30, i32* %t, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %while.cond
  br label %while.cond

while.cond:                                       
  %0 = load i32, i32* %n, align 4
  %cmp = icmp sgt i32 %0, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:
  %1 = load i32, i32* %t, align 4
  %mul = mul nsw i32 %1, 30
  store i32 %mul, i32* %t, align 4
  %2 = load i32, i32* %t, align 4
  %rem = srem i32 %2, 7
  store i32 %rem, i32* %t, align 4
  %3 = load i32, i32* %n, align 4
  %dec = add nsw i32 %3, -1
  store i32 %dec, i32* %n, align 4
  br label %while.cond

; CHECK: while.end:
; CHECK-NEXT: call void @loop_end()
while.end:
  ret i32 0
}

define dso_local i32 @bar() {
entry:
  %retval = alloca i32, align 4
  %n = alloca i32, align 4
  %t = alloca i32, align 4
  %i = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 10, i32* %n, align 4
  store i32 30, i32* %t, align 4
  store i32 0, i32* %i, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %for.cond
  br label %for.cond

for.cond:                                         
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %n, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         
  %2 = load i32, i32* %t, align 4
  %mul = mul nsw i32 %2, 25
  store i32 %mul, i32* %x, align 4
  %3 = load i32, i32* %t, align 4
  %add = add nsw i32 %3, 3
  store i32 %add, i32* %t, align 4
  %4 = load i32, i32* %t, align 4
  %div = sdiv i32 %4, 5
  store i32 %div, i32* %t, align 4
  br label %for.inc

for.inc:                                          
  %5 = load i32, i32* %i, align 4
  %inc = add nsw i32 %5, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

; CHECK: for.end:
; CHECK-NEXT: call void @loop_end()
for.end:                                          
  ret i32 0
}

; CHECK: declare void @loop_start()
; CHECK: declare void @loop_end() 


;int main(){
;    int a = 10;
;    return 0;
;}

;int foo() {
;  int n = 10;
;  int t = 30;
;  while (n > 0) {
;    t *= 30;
;    t %= 7;
;    n--;
;  }
;  return 0;
;}

;int bar() {
;  int n = 10;
;  int t = 30;
;  for(int i = 0;i<n;i++)
;  {
;    int x = t * 25;
;    t += 3;
;    t /= 5;
;  }
;  return 0;
;}
