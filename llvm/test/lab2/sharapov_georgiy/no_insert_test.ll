; RUN: opt -load-pass-plugin=%llvmshlibdir/LoopFramer%pluginext -passes=LoopFramer -S %s | FileCheck %s

define dso_local i32 @while_func() {
entry:
  %j = alloca i32, align 4
  store i32 2, ptr %j, align 4
  ; CHECK: call void @loop_start()
  ; CHECK-NEXT: br label %while.cond
  call void @loop_start()
  br label %while.cond

while.cond:                                       
  %0 = load i32, ptr %j, align 4
  %cmp = icmp sge i32 %0, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       
  %1 = load i32, ptr %j, align 4
  %dec = add nsw i32 %1, -1
  store i32 %dec, ptr %j, align 4
  br label %while.cond

while.end:  
  ; CHECK: call void @loop_end()
  ; CHECK-NEXT: %2 = load i32, ptr %j, align 4                                
  call void @loop_end()
  %2 = load i32, ptr %j, align 4
  ret i32 %2
}

define dso_local i32 @do_while_func() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  ; CHECK: call void @loop_start()
  ; CHECK-NEXT: br label %do.body
  call void @loop_start()
  br label %do.body

do.body:                                          
  %0 = load i32, ptr %i, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, ptr %i, align 4
  br label %do.cond

do.cond:                                          
  %1 = load i32, ptr %i, align 4
  %cmp = icmp sle i32 %1, 2
  br i1 %cmp, label %do.body, label %do.end

do.end:
  ; CHECK: call void @loop_end()
  ; CHECK-NEXT: %2 = load i32, ptr %i, align 4                                            
  call void @loop_end()
  %2 = load i32, ptr %i, align 4
  ret i32 %2
}

define dso_local i32 @while_if_func() {
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %j, align 4
  ; CHECK: call void @loop_start()
  ; CHECK-NEXT: br label %while.cond
  call void @loop_start()
  br label %while.cond

while.cond:                                       
  %0 = load i32, ptr %i, align 4
  %cmp = icmp slt i32 %0, 5
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       
  %1 = load i32, ptr %i, align 4
  %cmp1 = icmp sgt i32 %1, 6
  br i1 %cmp1, label %if.then, label %if.end

if.then:
  store i32 0, ptr %retval, align 4
  br label %while.end

if.end:                                           
  %2 = load i32, ptr %j, align 4
  %cmp2 = icmp slt i32 %2, 3
  br i1 %cmp2, label %if.then3, label %if.else

if.then3:                                         
  %3 = load i32, ptr %j, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, ptr %j, align 4
  br label %if.end4

if.else:
  store i32 5, ptr %retval, align 4
  br label %while.end

if.end4:                                          
  %4 = load i32, ptr %i, align 4
  %inc5 = add nsw i32 %4, 1
  store i32 %inc5, ptr %i, align 4
  br label %while.cond

while.end:   
  ; CHECK: call void @loop_end()
  ; CHECK-NEXT: %5 = load i32, ptr %retval, align 4
  call void @loop_end()
  %5 = load i32, ptr %retval, align 4
  ret i32 %5
}

define dso_local i32 @for_func() {
entry:
  %k = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 0, ptr %k, align 4
  store i32 1, ptr %b, align 4
  store i32 0, ptr %k, align 4
  ; CHECK: call void @loop_start()
  ; CHECK-NEXT: br label %for.cond
  call void @loop_start()
  br label %for.cond

for.cond:                                         
  %0 = load i32, ptr %k, align 4
  %cmp = icmp slt i32 %0, 5
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         
  %1 = load i32, ptr %b, align 4
  %add = add nsw i32 %1, 1
  store i32 %add, ptr %b, align 4
  br label %for.inc

for.inc:                                          
  %2 = load i32, ptr %k, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %k, align 4
  br label %for.cond

for.end:             
  ; CHECK: call void @loop_end()
  ; CHECK-NEXT: %3 = load i32, ptr %b, align 4                             
  call void @loop_end()
  %3 = load i32, ptr %b, align 4
  ret i32 %3
}


define dso_local i32 @while_return_func(i32 noundef %x) #0 {
entry:
  %retval = alloca i32, align 4
  %x.addr = alloca i32, align 4
  store i32 %x, ptr %x.addr, align 4
  ; CHECK: call void @loop_start()
  ; CHECK-NEXT: br label %while.cond
  call void @loop_start()
  br label %while.cond

while.cond:                                       
  %0 = load i32, ptr %x.addr, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       
  %1 = load i32, ptr %x.addr, align 4
  %cmp1 = icmp eq i32 %1, 5
  br i1 %cmp1, label %if.then, label %if.end

if.then:
  ; CHECK: call void @loop_end()
  ; CHECK-NEXT: store i32 1, ptr %retval, align 4
  call void @loop_end()                                          
  store i32 1, ptr %retval, align 4
  br label %return

if.end:                                           
  %2 = load i32, ptr %x.addr, align 4
  %cmp2 = icmp eq i32 %2, 7
  br i1 %cmp2, label %if.then3, label %if.end4

if.then3:                                         
  br label %while.end

if.end4:                                          
  %3 = load i32, ptr %x.addr, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, ptr %x.addr, align 4
  br label %while.cond

while.end:                                        
  ; CHECK: call void @loop_end()
  ; CHECK-NEXT: store i32 0, ptr %retval, align 4
  call void @loop_end()
  store i32 0, ptr %retval, align 4
  br label %return

return:                                           
  %4 = load i32, ptr %retval, align 4
  ret i32 %4
}


; CHECK-LABEL: @if_else_func 
; CHECK-NOT: call void @loop_
define dso_local i32 @if_else_func(i32 noundef %b) {
entry:
  %retval = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %b, ptr %b.addr, align 4
  store i32 0, ptr %b.addr, align 4
  br i1 false, label %if.then, label %if.else

if.then:                                          
  store i32 1, ptr %retval, align 4
  br label %return

if.else:                                          
  store i32 0, ptr %retval, align 4
  br label %return

return:                                           
  %0 = load i32, ptr %retval, align 4
  ret i32 %0
}

declare void @loop_start()

declare void @loop_end()
