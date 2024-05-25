// RUN: split-file %s %t

// RUN: %clang_cc1 -load %llvmshlibdir/LebedevaRenamePlugin%pluginext -add-plugin lebedeva-rename-plugin\
// RUN: -plugin-arg-lebedeva-rename-plugin OldName="A" -plugin-arg-lebedeva-rename-plugin NewName="B" %t/test1.cpp 
// RUN: FileCheck %s < %t/test1.cpp --check-prefix=FIRST-CHECK

// FIRST-CHECK: class B {
// FIRST-CHECK-NEXT: public:
// FIRST-CHECK-NEXT:     B() {};
// FIRST-CHECK-NEXT:     ~B();
// FIRST-CHECK-NEXT: };

//--- test1.cpp
class A {
public:
    A() {};
    ~A();
};

// RUN: %clang_cc1 -load %llvmshlibdir/LebedevaRenamePlugin%pluginext -add-plugin lebedeva-rename-plugin\
// RUN: -plugin-arg-lebedeva-rename-plugin OldName="C" -plugin-arg-lebedeva-rename-plugin NewName="D" %t/test2.cpp
// RUN: FileCheck %s < %t/test2.cpp --check-prefix=SECOND-CHECK

// SECOND-CHECK: class D {
// SECOND-CHECK-NEXT: private:
// SECOND-CHECK-NEXT:     int a;
// SECOND-CHECK-NEXT: public:
// SECOND-CHECK-NEXT:     D() {}
// SECOND-CHECK-NEXT:     D(int a): a(a) {}
// SECOND-CHECK-NEXT:     ~D();
// SECOND-CHECK-NEXT:     int getA() {
// SECOND-CHECK-NEXT:         return a;
// SECOND-CHECK-NEXT:     }
// SECOND-CHECK-NEXT: };
// SECOND-CHECK-NEXT: void func() {
// SECOND-CHECK-NEXT:     D* c = new D();
// SECOND-CHECK-NEXT:     delete c;
// SECOND-CHECK-NEXT: }

//--- test2.cpp
class C {
private:
    int a;
public:
    C() {}
    C(int a): a(a) {}
    ~C();
    int getA() {
        return a;
    }
};
void func() {
    C* c = new C();
    delete c;
}

// RUN: %clang_cc1 -load %llvmshlibdir/LebedevaRenamePlugin%pluginext -add-plugin lebedeva-rename-plugin\
// RUN: -plugin-arg-lebedeva-rename-plugin OldName="func1" -plugin-arg-lebedeva-rename-plugin NewName="newFunc" %t/test3.cpp 
// RUN: FileCheck %s < %t/test3.cpp --check-prefix=THIRD-CHECK

// THIRD-CHECK: double newFunc(int a) {
// THIRD-CHECK-NEXT:     int c = a + 10;
// THIRD-CHECK-NEXT:     double d = c * 6;
// THIRD-CHECK-NEXT:     return c + d;
// THIRD-CHECK-NEXT: };

//--- test3.cpp
double func1(int a) {
    int c = a + 10;
    double d = c * 6;
    return c + d;
};

// RUN: %clang_cc1 -load %llvmshlibdir/LebedevaRenamePlugin%pluginext -add-plugin lebedeva-rename-plugin\
// RUN: -plugin-arg-lebedeva-rename-plugin OldName="notFunc" -plugin-arg-lebedeva-rename-plugin NewName="newFunc" %t/test4.cpp 
// RUN: FileCheck %s < %t/test4.cpp --check-prefix=FOURTH-CHECK

// FOURTH-CHECK: int func2(int a, int b) {
// FOURTH-CHECK-NEXT:     int c = a + 10;
// FOURTH-CHECK-NEXT:     int d = c * 10;
// FOURTH-CHECK-NEXT:     return c + d;
// FOURTH-CHECK-NEXT: };

//--- test4.cpp
int func2(int a, int b) {
    int c = a + 10;
    int d = c * 10;
    return c + d;
};
