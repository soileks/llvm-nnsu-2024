; RUN: opt -load-pass-plugin %llvmshlibdir/Instrumentation_Soloninko_Andrey_FIIT2%pluginext\
; RUN: -passes=instrumentation -S %s | FileCheck %s

; CHECK-LABEL: @_Z3fooii
; CHECK: call void @instrument_start()
; CHECK-NEXT: %a.addr = alloca i32, align 4
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %2

define dso_local noundef i32 @_Z3fooii(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %add = add nsw i32 %0, %1
  store i32 %add, ptr %c, align 4
  %2 = load i32, ptr %c, align 4
  ret i32 %2
}

; CHECK-LABEL: @_Z5foo_vv
; CHECK: call void @instrument_start()
; CHECK-NEXT: call void @instrument_end()
define dso_local void @_Z5foo_vv() #1 {
entry:
  call void @instrument_start()
  ret void
}

; CHECK-LABEL: @_Z8func_endii
; CHECK: call void @instrument_start()
; CHECK-NEXT: %a.addr = alloca i32, align 4
; CHECK: call void @instrument_end()
; CHECK-NEXT: %0 = load i32, ptr %a.addr, align 4

define dso_local noundef i32 @_Z8func_endii(i32 noundef %a, i32 noundef %b) #1 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  call void @instrument_end()
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %add = add nsw i32 %0, %1
  ret i32 %add
}

; CHECK-LABEL: @_Z6func_fv
; CHECK: call void @instrument_start()
; CHECK-NEXT: call void @instrument_end()

define dso_local void @_Z6func_fv() #1 {
entry:
  call void @instrument_start()
  call void @instrument_end()
  ret void
}

; CHECK-LABEL: @_Z3foo_ifii
; CHECK: call void @instrument_start()
; CHECK-NEXT: %a.addr = alloca i32, align 4
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %4
define dso_local noundef i32 @_Z3foo_ifii(i32 noundef %a, i32 noundef %b) {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %2 = load i32, ptr %b.addr, align 4
  %3 = load i32, ptr %a.addr, align 4
  %sub = sub nsw i32 %2, %3
  store i32 %sub, ptr %c, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %4 = load i32, ptr %c, align 4
  ret i32 %4
}

declare void @instrument_start()

declare void @instrument_end()