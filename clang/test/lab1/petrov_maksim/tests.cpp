// RUN: %clang -cc1 -load %llvmshlibdir/WarningDeprecatedPlugin%pluginext -plugin WarningDeprecatedPlugin %s 2>&1 | FileCheck %s

// CHECK: warning: Function 'deprecated_foo1' contains 'deprecated' in its name
void deprecatedFoo1();

// CHECK-NOT: warning: Function 'Deprecated_foo2' contains 'deprecated' in its name
void DeprecatedFoo2();

struct St {
// CHECK: warning: Function 'Func_deprecated3' contains 'deprecated' in its name
	void deprecatedFunc3() {}
};

// RUN: %clang_cc1 -load %llvmshlibdir/WarningDeprecatedPlugin%pluginext -plugin WarningDeprecatedPlugin %s -plugin-arg-WarningDeprecatedPlugin help 2>&1 | FileCheck %s --check-prefix=HELP
// HELP: #Clang Plugin Help
