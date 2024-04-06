// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=SUM
// SUM: __attribute__((always_inline)) int sum(int A, int B) {
// SUM-NEXT:   return A + B;
// SUM-NEXT: }
int sum(int A, int B) { return A + B; }

// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=EMPTY
// EMPTY: __attribute__((always_inline)) void checkEmpty() {
// EMPTY-NEXT: }
void checkEmpty() {}

// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=MIN-NESTED
// MIN-NESTED: int minNested(int A, int B) {
// MIN-NESTED-NEXT:     {
// MIN-NESTED-NEXT:         if (A < B) {
// MIN-NESTED-NEXT:             return A;
// MIN-NESTED-NEXT:         }
// MIN-NESTED-NEXT:         return B;
// MIN-NESTED-NEXT:     }
// MIN-NESTED-NEXT: }
int minNested(int A, int B) {
  {
    if (A < B) {
      return A;
    }
    return B;
  }
}


// CHECK: int min(int A, int B) {
// CHECK-NEXT:     if (A < b) {
// CHECK-NEXT:         return A;
// CHECK-NEXT:     }
// CHECK-NEXT:     return B;
// CHECK-NEXT: }
int min(int A, int B) {
  if (A < B) {
    return A;
  }
  return B;
}


// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=FOR-LOOP
// FOR-LOOP: int forLoop() {
// FOR-LOOP-NEXT:     int Counter = 0;
// FOR-LOOP-NEXT:     for (int I = 0; I < 2; I++) {
// FOR-LOOP-NEXT:         Counter += I;
// FOR-LOOP-NEXT:     }
// FOR-LOOP-NEXT:     return Counter;
// FOR-LOOP-NEXT: }
int forLoop() {
  int Counter = 0;
  for (int I = 0; I < 2; I++) {
    Counter += I;
  }
  return Counter;
}


// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=WHILE-LOOP
// WHILE-LOOP: int whileLoop() {
// WHILE-LOOP-NEXT:     int I = 0;
// WHILE-LOOP-NEXT:     while (I < 5)
// WHILE-LOOP-NEXT:         {
// WHILE-LOOP-NEXT:             I++;
// WHILE-LOOP-NEXT:         }
// WHILE-LOOP-NEXT:     return I;
// WHILE-LOOP-NEXT: }
int whileLoop() {
  int I = 0;
  while (I < 5) {
    I++;
  }
  return I;
}

// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=DO-WHILE-LOOP
// DO-WHILE-LOOP: int doWhileLoop() {
// DO-WHILE-LOOP-NEXT:     int I = 0;
// DO-WHILE-LOOP-NEXT:     do {
// DO-WHILE-LOOP-NEXT:         I++;
// DO-WHILE-LOOP-NEXT:     } while (I < 5);
// DO-WHILE-LOOP-NEXT:     return I;
// DO-WHILE-LOOP-NEXT: }
int doWhileLoop() {
  int I = 0;
  do {
    I++;
  } while (I < 5);
  return I;
}


// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=SWITCH-CASE
// SWITCH-CASE: int switchCondition() {
// SWITCH-CASE-NEXT:     int I = 0;
// SWITCH-CASE-NEXT:     int Result = 0;
// SWITCH-CASE-NEXT:     switch (I) {
// SWITCH-CASE-NEXT:       case 0:
// SWITCH-CASE-NEXT:         Result = 10;
// SWITCH-CASE-NEXT:         break;
// SWITCH-CASE-NEXT:       default:
// SWITCH-CASE-NEXT:         Result = 100;
// SWITCH-CASE-NEXT:         break;
// SWITCH-CASE-NEXT:     }
// SWITCH-CASE-NEXT:     return Result;
// SWITCH-CASE-NEXT: }
int switchCondition() {
  int I = 0;
  int Result = 0;
  switch (I) {
  case 0:
    Result = 10;
    break;
  default:
  Result = 100;
    break;
  }
  return Result;
}


// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline %s 1>&1 | FileCheck %s --check-prefix=ALREADY-HAVE
// ALREADY-HAVE: void someFoo() __attribute__((always_inline)) {
// ALREADY-HAVE-NEXT: }
void __attribute__((always_inline)) someFoo(){}


// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline -plugin-arg-AddAlwaysInline --help %s 2>&1 | FileCheck %s --check-prefix=HELP
// HELP: This plugin adds the always_inline attribute to functions if they do not have conditions!

// RUN: %clang_cc1 -load %llvmshlibdir/AlwaysInlineAttributePlugin%pluginext -plugin AddAlwaysInline -plugin-arg-AddAlwaysInline --awdawdawd %s 2>&1 | FileCheck %s --check-prefix=WRONG-HELP
// WRONG-HELP: Use the --help argument to understand the plugin's purpose!
