// RUN: split-file %s %t
// RUN: %clang_cc1 -load %llvmshlibdir/deprWarnPlugin%pluginext -plugin plugin_for_deprecated_functions %t/with_-i.cpp -plugin-arg-plugin_for_deprecated_functions -i 2>&1 | FileCheck %t/with_-i.cpp
// RUN: %clang_cc1 -load %llvmshlibdir/deprWarnPlugin%pluginext -plugin plugin_for_deprecated_functions %t/without_-i.cpp 2>&1 | FileCheck %t/without_-i.cpp


//--- with_-i.cpp

// CHECK: warning: The function name contains "deprecated"
void deprecatedFunction();

// CHECK: warning: The function name contains "deprecated"
void functionWithDeprecatedWord();

// CHECK-NOT: warning: The function name contains "deprecated"
void regularFunction();

class SomeClass {
// CHECK: warning: The function name contains "deprecated"
  void functionWithDePrEcAtEdWord();
// CHECK-NOT: warning: The function name contains "deprecated"
  void regularFunctionAgain();
};

//--- without_-i.cpp

// CHECK: warning: The function name contains "deprecated"
void deprecatedFunction2();

// CHECK-NOT: warning: The function name contains "deprecated"
void functionWithDeprecatedWord2();

// CHECK-NOT: warning: The function name contains "deprecated"
void regularFunction2();

class SomeClass2 {
// CHECK-NOT: warning: The function name contains "deprecated"
  void functionWithDePrEcAtEdWord2();
// CHECK-NOT: warning: The function name contains "deprecated"
  void regularFunctionAgain2();
};
