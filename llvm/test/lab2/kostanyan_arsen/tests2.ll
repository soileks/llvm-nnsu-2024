; RUN: opt -load-pass-plugin %llvmshlibdir/InstrumentFunc_Kostanyan_Arsen_FIIT2%pluginext\
; RUN: -passes=instr_func -S %s | FileCheck %s


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

