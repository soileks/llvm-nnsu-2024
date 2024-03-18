// RUN: %clang -cc1 -load %llvmshlibdir/WarningDeprecatedPlugin%pluginext -plugin WarningDeprecatedPlugin %s 2>&1 | FileCheck %s

// CHECK: warning: Function 'deprecatedFoo1' contains 'deprecated' in its name
void deprecatedFoo1();

// CHECK-NOT: warning: Function 'DeprecatedFoo2' contains 'deprecated' in its name
void DeprecatedFoo2();

struct St {
// CHECK: warning: Function 'deprecatedFunc3' contains 'deprecated' in its name
  void deprecatedFunc3() {}
};

// RUN: %clang_cc1 -load %llvmshlibdir/WarningDeprecatedPlugin%pluginext -plugin WarningDeprecatedPlugin %s -plugin-arg-WarningDeprecatedPlugin help 2>&1 | FileCheck %s --check-prefix=HELP
// HELP: #Clang Plugin Help
