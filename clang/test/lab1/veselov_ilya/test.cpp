// RUN: %clang_cc1 -load %llvmshlibdir/PrintClassDescPlugin%pluginext -plugin print-class %s 2>&1 | FileCheck %s


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

// CHECK: TemplateClass
template<typename T> class TemplateClass {
    //CHECK-NEXT: Tvariable
    T Tvariable;
};

// CHECK: StaticClass
class StaticClass {
    // CHECK-NEXT: |_staticField
    static int staticField;
    // CHECK-NEXT: |_instantiationInt
    TemplateClass<int> instantiationInt;
};
