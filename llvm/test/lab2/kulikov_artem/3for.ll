; RUN: opt -load-pass-plugin=%llvmshlibdir/KulikovWrapPlugin%shlibext -passes=kulikov-wrap-plugin -S %s | FileCheck %s

; int j = 2;

; void a() {
;     int c = 10;
;     for (int i = 0; i < 10; i++) {
;         c++;
;     }
;     int q = c + 42;
; }

; int b(int a) {
;     int c = 10;
;     for (; j < a; j++) {
;         c++;
;     }
;     int q = c + 42;
;     return q;
; }

; int v() {
;     int c = 10;
;     a();
;     for (; ; j++) {
;         if (j > 5)
;             break;
;     }
;     b(c);
;     int q = c + 42;
;     return q;
; }

; void foo() {
;     while (1) {
;         int i = 2;
;         int j = i * i + 3 * i;
;         if (!j) {
;             return;
;         } else {
;             i++;
;             if (i > 3) {
;                 break;
;             }
;         }
;     }
;     int j = 2;
; }

; void bar() {
;     int i;
;     if (i == 0) {
;         i++;
;         return;
;     } else {
;         i--;
;         return;
;     }
; }

@j = dso_local global i32 2, align 4

define dso_local void @a() {
entry:
  %c = alloca i32, align 4
  %i = alloca i32, align 4
  %q = alloca i32, align 4
  store i32 10, ptr %c, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond
; CHECK:    call void @loop_start()
; CHECK-NEXT:    br label %for.cond

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

; CHECK:       for.end:
; CHECK-NEXT:    call void @loop_end()
for.end:
  %3 = load i32, ptr %c, align 4
  %add = add nsw i32 %3, 42
  store i32 %add, ptr %q, align 4
  ret void
}

define dso_local i32 @b(i32 noundef %a) {
entry:
  %a.addr = alloca i32, align 4
  %c = alloca i32, align 4
  %q = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 10, ptr %c, align 4
  br label %for.cond
; CHECK:    call void @loop_start()
; CHECK-NEXT:    br label %for.cond

for.cond:
  %0 = load i32, ptr @j, align 4
  %1 = load i32, ptr %a.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %2 = load i32, ptr %c, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %c, align 4
  br label %for.inc

for.inc:
  %3 = load i32, ptr @j, align 4
  %inc1 = add nsw i32 %3, 1
  store i32 %inc1, ptr @j, align 4
  br label %for.cond

; CHECK:       for.end:
; CHECK-NEXT:    call void @loop_end()
for.end:
  %4 = load i32, ptr %c, align 4
  %add = add nsw i32 %4, 42
  store i32 %add, ptr %q, align 4
  %5 = load i32, ptr %q, align 4
  ret i32 %5
}

define dso_local i32 @v() {
entry:
  %c = alloca i32, align 4
  %q = alloca i32, align 4
  store i32 10, ptr %c, align 4
  call void @a()
  br label %for.cond
; CHECK:      call void @loop_start()
; CHECK-NEXT:    br label %for.cond

for.cond:
  %0 = load i32, ptr @j, align 4
  %cmp = icmp sgt i32 %0, 5
  br i1 %cmp, label %if.then, label %if.end

; CHECK:       if.then:
; CHECK-NEXT:    call void @loop_end()
if.then:
  br label %for.end

if.end:
  br label %for.inc

for.inc:
  %1 = load i32, ptr @j, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr @j, align 4
  br label %for.cond

for.end:
  %2 = load i32, ptr %c, align 4
  %call = call i32 @b(i32 noundef %2)
  %3 = load i32, ptr %c, align 4
  %add = add nsw i32 %3, 42
  store i32 %add, ptr %q, align 4
  %4 = load i32, ptr %q, align 4
  ret i32 %4
}

define dso_local void @foo() {
entry:
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %j4 = alloca i32, align 4
  br label %while.body
; CHECK:      call void @loop_start()
; CHECK-NEXT:    br label %while.body

while.body:
  store i32 2, ptr %i, align 4
  %0 = load i32, ptr %i, align 4
  %1 = load i32, ptr %i, align 4
  %mul = mul nsw i32 %0, %1
  %2 = load i32, ptr %i, align 4
  %mul1 = mul nsw i32 3, %2
  %add = add nsw i32 %mul, %mul1
  store i32 %add, ptr %j, align 4
  %3 = load i32, ptr %j, align 4
  %tobool = icmp ne i32 %3, 0
  br i1 %tobool, label %if.else, label %if.then

; CHECK:       if.then:
; CHECK-NEXT:    call void @loop_end()
if.then:
  br label %return

if.else:
  %4 = load i32, ptr %i, align 4
  %inc = add nsw i32 %4, 1
  store i32 %inc, ptr %i, align 4
  %5 = load i32, ptr %i, align 4
  %cmp = icmp sgt i32 %5, 3
  br i1 %cmp, label %if.then2, label %if.end

; CHECK:       if.then2:
; CHECK-NEXT:    call void @loop_end()
if.then2:
  br label %while.end

if.end:
  br label %if.end3

if.end3:
  br label %while.body

while.end:
  store i32 2, ptr %j4, align 4
  br label %return

return:
  ret void
}
; CHECK-LABEL:  @bar
; CHECK-NOT:  call void @loop_start()
; CHECK-NOT:    call void @loop_end()
; CHECK:        ret void
define dso_local void @bar() {
entry:
  %i = alloca i32, align 4
  %0 = load i32, ptr %i, align 4
  %cmp = icmp eq i32 %0, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:
  %1 = load i32, ptr %i, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr %i, align 4
  br label %return

if.else:
  %2 = load i32, ptr %i, align 4
  %dec = add nsw i32 %2, -1
  store i32 %dec, ptr %i, align 4
  br label %return

return:
  ret void
}
