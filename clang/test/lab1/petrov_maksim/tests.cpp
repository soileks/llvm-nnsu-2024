// RUN: %clang -cc1 -load %llvmshlibdir/WarningDeprecatedPlugin%pluginext -plugin WarningDeprecatedPlugin %s 2>&1 | FileCheck %s

// CHECK: warning: Function 'deprecated_foo1' contains 'deprecated' in its name
void deprecated_foo1();

// CHECK-NOT: warning: Function 'Deprecated_foo2' contains 'deprecated' in its name
void Deprecated_foo2();

struct St {
// CHECK: warning: Function 'Func_deprecated3' contains 'deprecated' in its name
	void Func_deprecated3() {}
};
