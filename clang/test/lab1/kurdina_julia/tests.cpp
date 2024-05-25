// RUN: split-file %s %t
// RUN: %clang_cc1 -load %llvmshlibdir/addWarning%pluginext -plugin warn_dep %t/with_notCheckClass.cpp -plugin-arg-warn_dep -notCheckClass 2>&1 | FileCheck %t/with_notCheckClass.cpp
// RUN: %clang_cc1 -load %llvmshlibdir/addWarning%pluginext -plugin warn_dep %t/without_notCheckClass.cpp 2>&1 | FileCheck %t/without_notCheckClass.cpp

//--- with_notCheckClass.cpp

// CHECK: warning: Function or method is deprecated
void deprecated();

// CHECK: warning: Function or method is deprecated
void function_name_is_deprecated();

// CHECK-NOT: warning: Function or method is deprecated
void function();

// CHECK-NOT: warning: Function or method is deprecated
void function_depr();

class CheckClass {
	// CHECK-NOT: warning: Function or method is deprecated
	void deprecated();
	// CHECK-NOT: warning: Function or method is deprecated
	void function();
};

//--- without_notCheckClass.cpp

// CHECK: warning: Function or method is deprecated
void deprecated();

// CHECK: warning: Function or method is deprecated
void function_name_is_deprecated();

// CHECK-NOT: warning: Function or method is deprecated
void function();

// CHECK-NOT: warning: Function or method is deprecated
void function_depr();

class CheckClass {
	// CHECK: warning: Function or method is deprecated
	void deprecated();
	// CHECK-NOT: warning: Function or method is deprecated
	void function();
};
