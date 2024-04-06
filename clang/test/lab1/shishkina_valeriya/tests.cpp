// RUN: %clang_cc1 -load %llvmshlibdir/PrintClassesNamePlugin%pluginext -plugin print-classes %s 2>&1 | FileCheck %s
// CHECK: Test1
struct Test1
{
    // CHECK-NEXT: |_ A
    int A;
    // CHECK-NEXT: |_ B
    int B;
};

// CHECK: Test2
class Test2
{
    // CHECK-NEXT: |_ Arr
    double Arr;
    // CHECK-NEXT: |_ B
    const int B = 2;
};

// CHECK: Test3
class Test3
{
    // CHECK-NEXT: |_ Arg
    static int Arg;
public:
    // CHECK-NEXT: |_ Brr
    int Brr = 2;
};

// CHECK: Test4
struct Test4{};

// RUN: %clang_cc1 -load %llvmshlibdir/PrintClassesNamePlugin%pluginext -plugin print-classes -plugin-arg-print-classes --help 1>&1 | FileCheck %s --check-prefix=HELP

// HELP: This plugin displays the name of the class and it's fields.