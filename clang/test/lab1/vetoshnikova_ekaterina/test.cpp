// RUN: split-file %s %t
// RUN: %clang_cc1 -load %llvmshlibdir/VetoPluginPrintClasses%pluginext -plugin vetoshnikova-plugin-print-classes %t/tests.cpp 2>&1 | FileCheck %t/tests.cpp


//--- tests.cpp

// CHECK: MyStruct
struct MyStruct {
    // CHECK-NEXT: |_v
    int v;
    // CHECK-NEXT: |_w
    double w;
};

// CHECK: MyClass
class MyClass {
    // CHECK-NEXT: |_x
    int x;
    // CHECK-NEXT: |_y
    double y;
    // CHECK-NEXT: |_z
    float z;
};


// CHECK: Empty_class
class Empty_class {};

// CHECK: Class_pr
class Class_pr {
private:
    // CHECK-NEXT: |_r
    int r;
};

// CHECK: Circle
class Circle {
    // CHECK-NEXT: |_r
    int r;
    // CHECK: Center
    struct Center {
        //CHECK-NEXT: |_x
        float x;
        //CHECK-NEXT: |_y
        float y;
    };
};

// CHECK: Person
template<typename T> class Person {
    //CHECK-NEXT: |_id
    T id;
};

// CHECK: Class_st
class Class_st {
    // CHECK-NEXT: |_x
    static int x;
};

// RUN: %clang_cc1 -load %llvmshlibdir/VetoPluginPrintClasses%pluginext -plugin vetoshnikova-plugin-print-classes -plugin-arg-vetoshnikova-plugin-print-classes help 2>&1 %t/tests_help.cpp | FileCheck %t/tests_help.cpp --check-prefix=HELP

//--- tests_help.cpp

// HELP: This plugin prints the names of all classes and their fields
// HELP-NOT: |_