; RUN: opt -load-pass-plugin=%llvmshlibdir/MrrrBend%shlibext -passes=MrrrBend -S %s | FileCheck %s

; void foo() {
;     return;
; }

; int bar(int z) {
;     return z + z;
; }

; int baz(int k) {
;     if (k % 2) {
;         return k / 2;
;     } else {
;         return (k + 1) / 2;
;     }
; }

; int Mrrr(int x){
;     instrument_start();
;     x=x+5;
;     x=x+10;
;     instrument_end();
;     return x;
; }

; CHECK-LABEL: @foo
; CHECK: call void @instrument_start()
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret void

define dso_local void @foo() {
entry:
  ret void
}

; CHECK-LABEL: @bar
; CHECK: %z.addr = alloca i32, align 4
; CHECK-NEXT: call void @instrument_start()
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %add

define dso_local i32 @bar(i32 noundef %z) {
entry:
  %z.addr = alloca i32, align 4
  store i32 %z, ptr %z.addr, align 4
  %0 = load i32, ptr %z.addr, align 4
  %1 = load i32, ptr %z.addr, align 4
  %add = add nsw i32 %0, %1
  ret i32 %add
}

; CHECK-LABEL: @baz
; CHECK: %retval = alloca i32, align 4
; CHECK-NEXT: %k.addr = alloca i32, align 4
; CHECK-NEXT: call void @instrument_start()
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %3

define dso_local i32 @baz(i32 noundef %k) {
entry:
  %retval = alloca i32, align 4
  %k.addr = alloca i32, align 4
  store i32 %k, ptr %k.addr, align 4
  %0 = load i32, ptr %k.addr, align 4
  %rem = srem i32 %0, 2
  %tobool = icmp ne i32 %rem, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:
  %1 = load i32, ptr %k.addr, align 4
  %div = sdiv i32 %1, 2
  store i32 %div, ptr %retval, align 4
  br label %return

if.else:
  %2 = load i32, ptr %k.addr, align 4
  %add = add nsw i32 %2, 1
  %div1 = sdiv i32 %add, 2
  store i32 %div1, ptr %retval, align 4
  br label %return

return:
  %3 = load i32, ptr %retval, align 4
  ret i32 %3
}

; CHECK-LABEL: @Mrrr
; CHECK-NOT: call void @instrument_start()
; CHECK: store i32 %0, ptr %2, align 4
; CHECK: %7 = load i32, ptr %2, align 4
; CHECK-NOT: call void @instrument_end()
; CHECK: ret i32 %7

define dso_local noundef i32 @Mrrr(i32 noundef %0) {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  call void @instrument_start()
  %3 = load i32, ptr %2, align 4
  %4 = add nsw i32 %3, 5
  store i32 %4, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = add nsw i32 %5, 10
  store i32 %6, ptr %2, align 4
  call void @instrument_end()
  %7 = load i32, ptr %2, align 4
  ret i32 %7
}

declare void @instrument_start()

declare void @instrument_end()
