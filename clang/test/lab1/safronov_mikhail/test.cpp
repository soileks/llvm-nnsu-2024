// RUN: %clang_cc1 -load %llvmshlibdir/PrintClassesPlugin%pluginext -plugin print-classes %s 2>&1 | FileCheck %s
// REQUIRES: plugins

// CHECK: Empty
class Empty {};

// CHECK: MyStruct
struct MyStruct{
    // CHECK-NEXT: |_a
    int a;
    // CHECK-NEXT: |_b
    int b;
};

// CHECK: MyClass
class MyClass {
    // CHECK-NEXT: |_a_c
    int a_c;
    // CHECK-NEXT: |_b_c
    float b_c;
    // CHECK-NEXT: |_c_c
    double c_c;
};

// CHECK: outerClass
class outerClass {
    // CHECK: innerClass
    class innerClass {
        //CHECK-NEXT: |_var
        float var;
    };
};

// CHECK: TemplateClass1
template<typename T> class TemplateClass1 {
    //CHECK-NEXT: |_Tvariable
    T Tvariable;
};

// CHECK: TemplateClass2
template<typename T> class TemplateClass2 {
    //CHECK-NEXT: |_Tvariable1
    T Tvariable1;
    //CHECK-NEXT: |_Tvariable2
    static T Tvariable2;
    //CHECK-NEXT: |_TemplateClass2<T>
    TemplateClass2() {};
};