// RUN: %clang_cc1 -load %llvmshlibdir/PluginWarningDeprecated%pluginext -plugin warning-deprecated %s 2>&1 | FileCheck %s

// CHECK: warning: find 'deprecated' in function name
void deprEcated(int a, int b) { int c = a + b; };

// CHECK-NOT: warning: find 'deprecated' in function name
void Matrix_Mult() { ; };

// CHECK: warning: find 'deprecated' in function name
void DeprEcated_foo(int c) {
  int a = c - 10;
  int b = a + c;
};

// CHECK: warning: find 'deprecated' in function name
int function_with_deprecated(int a, int b) { return a - b; }

struct A {
  // CHECK: warning: find 'deprecated' in function name
  int foo_dEprecated_a(int a, int b) { return a + b; }
  // CHECK-NOT: warning: find 'deprecated' in function name
  int foo_deprecatd_a(int a, int b) { return a + b; }
};

// RUN: %clang_cc1 -load %llvmshlibdir/PluginWarningDeprecated%pluginext -plugin warning-deprecated %s -plugin-arg-warning-deprecated help 2>&1 | FileCheck %s --check-prefix=HELP

// HELP: Plugin Warning Deprecated prints a warning if a function name contains 'deprecated'