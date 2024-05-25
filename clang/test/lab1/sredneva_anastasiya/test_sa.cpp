// RUN: split-file %s %t
// RUN: not %clang_cc1 -load %llvmshlibdir/Deprecated_sa%pluginext -plugin deprecated_plugin %t/with_err.cpp -plugin-arg-deprecated_plugin -err 2>&1 | FileCheck %t/with_err.cpp
// RUN: %clang_cc1 -load %llvmshlibdir/Deprecated_sa%pluginext -plugin deprecated_plugin %t/without_err.cpp 2>&1 | FileCheck %t/without_err.cpp

//--- with_err.cpp

// CHECK: error: The function name has 'deprecated'
void deprecated2();

// CHECK: error: The function name has 'deprecated'
void deprecatedFunc2();

// CHECK: error: The function name has 'deprecated'
int deprecatedSumm2(int a, int b) {
	return a + b;
}

// CHECK-NOT: error: The function name has 'deprecated'
void deprecation2();

// CHECK-NOT: error: The function name has 'deprecated'
void deprfunction2();

// CHECK-NOT: error: The function name has 'deprecated'
void foo2();

class Test2 {
	// CHECK: error: The function name has 'deprecated'
	void is_deprecated_function2();
	// CHECK-NOT: error: The function name has 'deprecated'
	void depfunc2();
};


//--- without_err.cpp

// CHECK: warning: The function name has 'deprecated'
void deprecated();

// CHECK: warning: The function name has 'deprecated'
void deprecatedFunc();

// CHECK: warning: The function name has 'deprecated'
int deprecatedSumm(int a, int b) {
	return a + b;
}

// CHECK-NOT: warning: The function name has 'deprecated'
void deprecation();

// CHECK-NOT: warning: The function name has 'deprecated'
void deprfunction();

// CHECK-NOT: warning: The function name has 'deprecated'
void foo();

class Test {
	// CHECK: warning: The function name has 'deprecated'
	void is_deprecated_function();
	// CHECK-NOT: warning: The function name has 'deprecated'
	void depfunc();
};
