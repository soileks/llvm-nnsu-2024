; RUN: opt -load-pass-plugin=%llvmshlibdir/LoopLLVMPlugin_SadikovD%pluginext -passes=LoopLLVMPlugin_SadikovD -S %s | FileCheck %s

; source code:

; int func(int n) {
; 	int a = 1;
; 	for (int i = 0; i < n; i++) {
; 		a++;
; 	}
; 	return a;
; }

; void while_runs_forever() {
; 	int t = 42;
; 	while (t == 42) {
; 		t = t / 42 - 1 + 37 / 37 * 42;
; 	}
; }

; int func_without_cycle(int a, int b) {
; 	if (a > b)
; 		return a + b;
; 	else
; 		return b + a;
; }

; int while_func() {
; 	int a = 42, b = 42 * 2;
; 	int t = 0;
; 	while (a < b) {
; 		a++;
; 		t++;
; 	}
; 	return t;
; }

; void do_while_func() {
; 	int a = 42, b = 42;
; 	int t = 0;
; 	do {
; 		t++;
; 		a++;
; 	} while (a < b);
; }

define dso_local i32 @func(i32 noundef %n) #0 {
entry:
  %n.addr = alloca i32, align 4
  %a = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %n, ptr %n.addr, align 4
  store i32 1, ptr %a, align 4
  store i32 0, ptr %i, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %for.cond
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, ptr %i, align 4
  %1 = load i32, ptr %n.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %2 = load i32, ptr %a, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %a, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %3 = load i32, ptr %i, align 4
  %inc1 = add nsw i32 %3, 1
  store i32 %inc1, ptr %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
; CHECK: call void @loop_end()
; CHECK-NEXT: %4 = load i32, ptr %a, align 4
  %4 = load i32, ptr %a, align 4
  ret i32 %4
}

define dso_local void @while_runs_forever() #0 {
entry:
  %t = alloca i32, align 4
  store i32 42, ptr %t, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %while.cond
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %0 = load i32, ptr %t, align 4
  %cmp = icmp eq i32 %0, 42
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %1 = load i32, ptr %t, align 4
  %div = sdiv i32 %1, 42
  %sub = sub nsw i32 %div, 1
  %add = add nsw i32 %sub, 42
  store i32 %add, ptr %t, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
; CHECK: call void @loop_end()
; CHECK-NEXT: ret void
  ret void
}

define dso_local i32 @func_without_cycle(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %retval = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %a.addr = alloca i32, align 4
  store i32 %b, ptr %b.addr, align 4
  store i32 %a, ptr %a.addr, align 4
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %cmp = icmp sgt i32 %0, %1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %2 = load i32, ptr %a.addr, align 4
  %3 = load i32, ptr %b.addr, align 4
  %add = add nsw i32 %2, %3
  store i32 %add, ptr %retval, align 4
  br label %return

if.else:                                          ; preds = %entry
  %4 = load i32, ptr %b.addr, align 4
  %5 = load i32, ptr %a.addr, align 4
  %add1 = add nsw i32 %4, %5
  store i32 %add1, ptr %retval, align 4
  br label %return

return:                                           ; preds = %if.else, %if.then
  %6 = load i32, ptr %retval, align 4
  ret i32 %6
}

define dso_local i32 @while_func() #0 {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %t = alloca i32, align 4
  store i32 42, ptr %a, align 4
  store i32 84, ptr %b, align 4
  store i32 0, ptr %t, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %while.cond
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %0 = load i32, ptr %a, align 4
  %1 = load i32, ptr %b, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %2 = load i32, ptr %a, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %a, align 4
  %3 = load i32, ptr %t, align 4
  %inc1 = add nsw i32 %3, 1
  store i32 %inc1, ptr %t, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
; CHECK: call void @loop_end()
; CHECK-NEXT: %4 = load i32, ptr %t, align 4
  %4 = load i32, ptr %t, align 4
  ret i32 %4
}

define dso_local void @do_while_func() #0 {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %t = alloca i32, align 4
  store i32 42, ptr %a, align 4
  store i32 42, ptr %b, align 4
  store i32 0, ptr %t, align 4
; CHECK: call void @loop_start()
; CHECK-NEXT: br label %do.body
  br label %do.body

do.body:                                          ; preds = %do.cond, %entry
  %0 = load i32, ptr %t, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, ptr %t, align 4
  %1 = load i32, ptr %a, align 4
  %inc1 = add nsw i32 %1, 1
  store i32 %inc1, ptr %a, align 4
  br label %do.cond

do.cond:                                          ; preds = %do.body
  %2 = load i32, ptr %a, align 4
  %3 = load i32, ptr %b, align 4
  %cmp = icmp slt i32 %2, %3
  br i1 %cmp, label %do.body, label %do.end

do.end:                                           ; preds = %do.cond
; CHECK: call void @loop_end()
; CHECK-NEXT: ret void
  ret void
}

; CHECK: declare void @loop_start()

; CHECK: declare void @loop_end()
