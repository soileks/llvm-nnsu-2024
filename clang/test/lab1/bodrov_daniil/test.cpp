// RUN: %clang_cc1 -load %llvmshlibdir/ClassFieldPrinter%pluginext -plugin class-field-printer %s 1>&1 | FileCheck %s

// CHECK: Empty (class)
class Empty {};

// CHECK: Data (class)
class Data {
public:
  int A; // CHECK-NEXT: |_ A (int|public)
  static int B; // CHECK-NEXT: |_ B (int|public|static)
  void func() {} // CHECK-NEXT: |_ func (void (void)|public|method)
};

// CHECK: Template (class|template)
template <typename T>
class Template {
public:
  T Value; // CHECK-NEXT: |_ Value (T|public)
};

// CHECK: TestClass (class)
class TestClass {
public:
  int PublicInt; // CHECK-NEXT: |_ PublicInt (int|public)
  static int PublicStaticInt; // CHECK-NEXT: |_ PublicStaticInt (int|public|static)
  void publicFunc() {} // CHECK-NEXT: |_ publicFunc (void (void)|public|method)

private:
  int PrivateInt; // CHECK-NEXT: |_ PrivateInt (int|private)
  static int PrivateStaticInt; // CHECK-NEXT: |_ PrivateStaticInt (int|private|static)
  void privateFunc() {} // CHECK-NEXT: |_ privateFunc (void (void)|private|method)
};

// CHECK: AnotherTestClass (class)
class AnotherTestClass {
public:
  double PublicDouble; // CHECK-NEXT: |_ PublicDouble (double|public)
  static double PublicStaticDouble; // CHECK-NEXT: |_ PublicStaticDouble (double|public|static)
  double publicFunc(); // CHECK-NEXT: |_ publicFunc (double (void)|public|method)

  Template<TestClass> TTestClass; // CHECK-NEXT: |_ TTestClass (Template<TestClass>|public)

private:
  double PrivateDouble; // CHECK-NEXT: |_ PrivateDouble (double|private)
  static double PrivateStaticDouble; // CHECK-NEXT: |_ PrivateStaticDouble (double|private|static)
  float privateFunc(const char* Str); // CHECK-NEXT: privateFunc (float (const char *)|private|method)
};

// CHECK: DerivedClass (class)
class DerivedClass : public TestClass {
public:
  int DerivedPublicInt; // CHECK-NEXT: |_ DerivedPublicInt (int|public)
  static int DerivedPublicStaticInt; // CHECK-NEXT: |_ DerivedPublicStaticInt (int|public|static)
  TestClass derivedPublicFunc(Template<int> Data, Template<const char*> DataStr); // CHECK-NEXT: |_ derivedPublicFunc (TestClass (Template<int>, Template<const char *>)|public|method)

  AnotherTestClass ClassField; // CHECK-NEXT: |_ ClassField (AnotherTestClass|public)

private:
  int DerivedPrivateInt; // CHECK-NEXT: |_ DerivedPrivateInt (int|private)
  static int DerivedPrivateStaticInt; // CHECK-NEXT: |_ DerivedPrivateStaticInt (int|private|static)
  void derivedPrivateFunc(); // CHECK-NEXT: |_ derivedPrivateFunc (void (void)|private|method)
  Template<Template<TestClass>> TTestClass; // CHECK-NEXT: |_ TTestClass (Template<Template<TestClass> >|private)
};

// CHECK: MyStruct (struct)
struct MyStruct {
  int X; // CHECK-NEXT: |_ X (int|public)
  double Y; // CHECK-NEXT: |_ Y (double|public)
};