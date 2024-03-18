// RUN: split-file %s %t
// RUN: %clang_cc1 -load %llvmshlibdir/RenamedIdPlugin%pluginext -add-plugin renamed-id\
// RUN: -plugin-arg-renamed-id OldName="A"\
// RUN: -plugin-arg-renamed-id NewName="B" %t/test_1.cpp
// RUN: FileCheck %s < %t/test_1.cpp --check-prefix=CLASS-CHECK

// CLASS-CHECK: class B {
// CLASS-CHECK-NEXT: public:
// CLASS-CHECK-NEXT:     B() {};
// CLASS-CHECK-NEXT:     ~B();
// CLASS-CHECK-NEXT: };
// CLASS-CHECK-NEXT: void func() {
// CLASS-CHECK-NEXT:     B* a = new B;
// CLASS-CHECK-NEXT:     B b;
// CLASS-CHECK-NEXT:     delete a;
// CLASS-CHECK-NEXT: };

//--- test_1.cpp
class A {
public:
    A() {};
    ~A();
};
void func() {
    A* a = new A;
    A b;
    delete a;
};

// RUN: %clang_cc1 -load %llvmshlibdir/RenamedIdPlugin%pluginext -add-plugin renamed-id\
// RUN: -plugin-arg-renamed-id OldName="A"\
// RUN: -plugin-arg-renamed-id NewName="B" %t/test_2.cpp
// RUN: FileCheck %s < %t/test_2.cpp --check-prefix=SUM-CHECK

// SUM-CHECK: int sum(int a, int b) {
// SUM-CHECK-NEXT:     int c = sum(1, 2);
// SUM-CHECK-NEXT:     c++; 
// SUM-CHECK-NEXT:     a += b;
// SUM-CHECK-NEXT:     return a+b;
// SUM-CHECK-NEXT: };

//--- test_2.cpp
int sum(int a, int b) {
    int c = sum(1, 2);
    c++; 
    a += b;
    return a+b;
};

// RUN: %clang_cc1 -load %llvmshlibdir/RenamedIdPlugin%pluginext -add-plugin renamed-id\
// RUN: -plugin-arg-renamed-id OldName="C"\
// RUN: -plugin-arg-renamed-id NewName="Renamed_C" %t/test_3.cpp
// RUN: FileCheck %s < %t/test_3.cpp --check-prefix=CLASS2-CHECK

// CLASS2-CHECK: class Renamed_C {
// CLASS2-CHECK-NEXT: private:
// CLASS2-CHECK-NEXT:     int a;
// CLASS2-CHECK-NEXT:     int b;
// CLASS2-CHECK-NEXT: public:
// CLASS2-CHECK-NEXT:     Renamed_C() {}
// CLASS2-CHECK-NEXT:     Renamed_C(int a, int b): a(a), b(b) {}
// CLASS2-CHECK-NEXT:     ~Renamed_C();
// CLASS2-CHECK-NEXT: };
// CLASS2-CHECK-NEXT: void func() {
// CLASS2-CHECK-NEXT:     Renamed_C a;
// CLASS2-CHECK-NEXT:     Renamed_C* b = new Renamed_C(1, 2);
// CLASS2-CHECK-NEXT:     delete b;
// CLASS2-CHECK-NEXT: }

//--- test_3.cpp
class C {
private:
    int a;
    int b;
public:
    C() {}
    C(int a, int b): a(a), b(b) {}
    ~C();
};
void func() {
    C a;
    C* b = new C(1, 2);
    delete b;
};