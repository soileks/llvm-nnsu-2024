// RUN: %clang_cc1 -load %llvmshlibdir/DeprecatedPlug%pluginext -plugin deprecated-function %s 2>&1 | FileCheck %s --check-prefix=DEPRECATED
// REQUIRES: plugins

// DEPRECATED: warning: Deprecated function name
void Deprecated(int a, int b);

// DEPRECATED: warning: Deprecated function name
void dEpReCaTeD(int c);

// DEPRECATED-NOT: warning: Deprecated function name
void sqrt();

// DEPRECATED: warning: Deprecated function name
class A {
  int method_deprecated();
};

// RUN: %clang_cc1 -load %llvmshlibdir/DeprecatedPlug%pluginext -plugin deprecated-function %s -plugin-arg-deprecated-function help 2>&1 | FileCheck %s --check-prefix=HELP
// REQUIRES: plugins

// HELP: Deprecated Plugin version 1.0
