// RUN: %clang_cc1 -load %llvmshlibdir/Myfirstplugin%pluginext -plugin classprinter %s | FileCheck %s

// CHECK: Empty
class Empty {};

// CHECK: Cat
struct Cat {
    // CHECK-NEXT: |_ variable
    int variable;
};

// CHECK: Kitten
class Kitten {
    // CHECK-NEXT: |_ meow
    float meow;
    // CHECK-NEXT: |_ mrrr
    double mrrr;
    // CHECK: InCattery
    class InCattery {
        //CHECK-NEXT: |_ InCat
        int InCat;
    };
};

// CHECK: TClass
template<typename T> class TClass {
    // CHECK-NEXT: |_ TVar
    T TVar;
};

// CHECK: StaticCat
class StaticCat {
  // CHECK-NEXT: |_ variable
  static int variable;
};

// RUN: %clang_cc1 -load %llvmshlibdir/Myfirstplugin%pluginext -plugin classprinter -plugin-arg-classprinter help 2>&1 %s | FileCheck %s --check-prefix=HELP

// HELP: Plugin prints names of classes and their fields