; RUN: opt -load-pass-plugin %llvmshlibdir/VetoshnikovaEInstrumentFunctions%pluginext\
; RUN: -passes=instrument_functions -S %s | FileCheck %s

define dso_local void @instrument_start() {
entry:
  ret void
}

define dso_local void @instrument_end() {
entry:
  ret void
}

; CHECK-LABEL: @func
; CHECK: call void @instrument_start()
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret void

define dso_local void @func() {
entry:
  ret void
}

; CHECK-LABEL: @funcInstrEnd
; CHECK: call void @instrument_start()
; CHECK-NEXT: call void @instrument_end()
; CHECK-NEXT: ret void

define dso_local void @funcInstrEnd() {
entry:
  call void @instrument_end()
  ret void
}

; CHECK-LABEL: @FuncSum
; CHECK: call void @instrument_start()
; CHECK-NEXT: %x.addr = alloca i32, align 4
; CHECK-NEXT: %y.addr = alloca i32, align 4
; CHECK-NEXT: store i32 %x, ptr %x.addr, align 4
; CHECK-NEXT: store i32 %y, ptr %y.addr, align 4
; CHECK-NEXT: %0 = load i32, ptr %x.addr, align 4
; CHECK-NEXT: %1 = load i32, ptr %y.addr, align 4
; CHECK-NEXT: %add = add nsw i32 %0, %1
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %add

define dso_local noundef i32 @FuncSum(i32 noundef %x, i32 noundef %y) {
entry:
  %x.addr = alloca i32, align 4
  %y.addr = alloca i32, align 4
  store i32 %x, ptr %x.addr, align 4
  store i32 %y, ptr %y.addr, align 4
  %0 = load i32, ptr %x.addr, align 4
  %1 = load i32, ptr %y.addr, align 4
  %add = add nsw i32 %0, %1
  ret i32 %add
}
