; RUN: opt -load-pass-plugin %llvmshlibdir/InstrumentFunc_Kostanyan_Arsen_FIIT2%pluginext\
; RUN: -passes=instr_func -S %s | FileCheck %s

define dso_local void @instrument_start() {
entry:
  ret void
}

define dso_local void @instrument_end() {
entry:
  ret void
}

; CHECK-LABEL:  @_Z3gcdii
; CHECK: call void @instrument_start()
; CHECK-NEXT: %retval = alloca i32, align 4
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %5

define dso_local noundef i32 @_Z3gcdii(i32 noundef %a, i32 noundef %b) #1 {
entry:
  %retval = alloca i32, align 4
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  %0 = load i32, ptr %b.addr, align 4
  %cmp = icmp eq i32 %0, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:
  %1 = load i32, ptr %a.addr, align 4
  store i32 %1, ptr %retval, align 4
  br label %return

if.else:
  %2 = load i32, ptr %b.addr, align 4
  %3 = load i32, ptr %a.addr, align 4
  %4 = load i32, ptr %b.addr, align 4
  %rem = srem i32 %3, %4
  %call = call noundef i32 @_Z3gcdii(i32 noundef %2, i32 noundef %rem)
  store i32 %call, ptr %retval, align 4
  br label %return

return:
  %5 = load i32, ptr %retval, align 4
  ret i32 %5
}

; CHECK-LABEL:  @_Z3sumii
; CHECK: call void @instrument_start()
; CHECK-NEXT: %0 = load i32, ptr %a.addr, align 4
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %add

define dso_local noundef i32 @_Z3sumii(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  call void @instrument_start()
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %add = add nsw i32 %0, %1
  ret i32 %add
}

; CHECK-LABEL:  @_Z4multii
; CHECK: call void @instrument_start()
; CHECK-NEXT: %a.addr = alloca i32, align 4
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret i32 %mul

define dso_local noundef i32 @_Z4multii(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %mul = mul nsw i32 %0, %1
  ret i32 %mul
}

; CHECK-LABEL:  @_Z4swapRiS_
; CHECK: call void @instrument_start()
; CHECK-NEXT: %a.addr = alloca ptr, align 8
; CHECK: call void @instrument_end()
; CHECK-NEXT: ret void

define dso_local void @_Z4swapRiS_(ptr noundef nonnull align 4 dereferenceable(4) %a, ptr noundef nonnull align 4 dereferenceable(4) %b) #0 {
entry:
  %a.addr = alloca ptr, align 8
  %b.addr = alloca ptr, align 8
  %temp = alloca i32, align 4
  store ptr %a, ptr %a.addr, align 8
  store ptr %b, ptr %b.addr, align 8
  %0 = load ptr, ptr %a.addr, align 8
  %1 = load i32, ptr %0, align 4
  store i32 %1, ptr %temp, align 4
  %2 = load ptr, ptr %b.addr, align 8
  %3 = load i32, ptr %2, align 4
  %4 = load ptr, ptr %a.addr, align 8
  store i32 %3, ptr %4, align 4
  %5 = load i32, ptr %temp, align 4
  %6 = load ptr, ptr %b.addr, align 8
  store i32 %5, ptr %6, align 4
  ret void
}

; CHECK-LABEL:  @_Z4testii
; CHECK: call void @instrument_start()
; CHECK-NEXT: %0 = load i32, ptr %a.addr, align 4
; CHECK: call void @instrument_end()
; CHECK-NEXT: %2 = load i32, ptr %result, align 4

define dso_local noundef i32 @_Z4testii(i32 noundef %a, i32 noundef %b) {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %result = alloca i32, align 4
  store i32 %a, ptr %a.addr, align 4
  store i32 %b, ptr %b.addr, align 4
  call void @instrument_start()
  %0 = load i32, ptr %a.addr, align 4
  %1 = load i32, ptr %b.addr, align 4
  %add = add nsw i32 %0, %1
  store i32 %add, ptr %result, align 4
  call void @instrument_end()
  %2 = load i32, ptr %result, align 4
  ret i32 %2
}
