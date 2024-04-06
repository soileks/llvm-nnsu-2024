// RUN: split-file %s %t

// RUN: %clang_cc1 -load %llvmshlibdir/RenameIDPlugin%pluginext\
// RUN: -add-plugin RenameID\
// RUN: -plugin-arg-RenameID OldName=a\
// RUN: -plugin-arg-RenameID NewName=new_var %t/rename_var.cpp
// RUN: FileCheck %s < %t/rename_var.cpp --check-prefix=VAR

// VAR: int func() {
// VAR-NEXT: int *new_var, b = 2;
// VAR-NEXT: new_var = &b;
// VAR-NEXT: new_var = b + new_var;
// VAR-NEXT: new_var++;
// VAR-NEXT: return *new_var;
// VAR-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/RenameIDPlugin%pluginext\
// RUN: -add-plugin RenameID\
// RUN: -plugin-arg-RenameID OldName=int\
// RUN: -plugin-arg-RenameID NewName=long %t/rename_type.cpp
// RUN: FileCheck %s < %t/rename_type.cpp --check-prefix=TYPE

// TYPE: long* func(long x, long y) {
// TYPE-NEXT: long *a, *c;
// TYPE-NEXT: long b = 2, d, e;
// TYPE-NEXT: a = &b;
// TYPE-NEXT: a = b + a;
// TYPE-NEXT: a++;
// TYPE-NEXT: return a;
// TYPE-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/RenameIDPlugin%pluginext\
// RUN: -add-plugin RenameID\
// RUN: -plugin-arg-RenameID OldName=func\
// RUN: -plugin-arg-RenameID NewName=new_func %t/rename_func.cpp
// RUN: FileCheck %s < %t/rename_func.cpp --check-prefix=FUNC

// FUNC: int new_func(int b) {
// FUNC-NEXT: return 1 + 1;
// FUNC-NEXT: }
// FUNC-NEXT: int func2(){
// FUNC-NEXT: new_func(new_func(2));
// FUNC-NEXT: return new_func(2) + 3;
// FUNC-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/RenameIDPlugin%pluginext\
// RUN: -add-plugin RenameID\
// RUN: -plugin-arg-RenameID OldName=func\
// RUN: -plugin-arg-RenameID NewName=f %t/rename_recursive_func.cpp
// RUN: FileCheck %s < %t/rename_recursive_func.cpp --check-prefix=REC_FUNC

// REC_FUNC: int f(int a) {
// REC_FUNC-NEXT: return f(a - 1);
// REC_FUNC-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/RenameIDPlugin%pluginext\
// RUN: -add-plugin RenameID\
// RUN: -plugin-arg-RenameID OldName=OldClass\
// RUN: -plugin-arg-RenameID NewName=NewClass %t/rename_class.cpp
// RUN: FileCheck %s < %t/rename_class.cpp --check-prefix=CLASS

// CLASS: class NewClass{
// CLASS-NEXT: private:
// CLASS-NEXT: int a;
// CLASS-NEXT: int b;
// CLASS-NEXT: public:
// CLASS-NEXT: NewClass() {}
// CLASS-NEXT: NewClass(int a, int b): a(a), b(b) {}
// CLASS-NEXT: NewClass(NewClass &b) {}
// CLASS-NEXT: NewClass func(NewClass c){ return c; };
// CLASS-NEXT: ~NewClass();
// CLASS-NEXT: };
// CLASS: void func(NewClass c) {
// CLASS-NEXT: NewClass a;
// CLASS-NEXT: NewClass *var = new NewClass(1, 2);
// CLASS-NEXT: delete var;
// CLASS-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/RenameIDPlugin%pluginext\
// RUN: -add-plugin RenameID\
// RUN: -plugin-arg-RenameID SomeOldName=SomeOldName\
// RUN: -plugin-arg-RenameID NewName=NewClass \
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

// ERROR: Error in parameters input.
// ERROR-NEXT: Format of input:
// ERROR-NEXT: OldName='MyOldName'
// ERROR-NEXT: NewName='MyNewName'


//--- rename_var.cpp
int func() {
  int *a, b = 2;
  a = &b;
  a = b + a;
  a++;
  return *a;
}
//--- rename_type.cpp
int* func(int x, int y) {
  int *a, *c;
  int b = 2, d, e;
  a = &b;
  a = b + a;
  a++;
  return a;
}
//--- rename_func.cpp
int func(int b) {
    return 1 + 1;
}
int func2(){
  func(func(2));
  return func(2) + 3;
}
//--- rename_recursive_func.cpp
int func(int a) {
  return func(a - 1);
}
//--- rename_class.cpp
class OldClass{
 private:
  int a;
  int b;
 public:
  OldClass() {}
  OldClass(int a, int b): a(a), b(b) {}
  OldClass(OldClass &b) {}
  OldClass func(OldClass c){ return c; };
  ~OldClass();
};
void func(OldClass c) {
  OldClass a;
  OldClass *var = new OldClass(1, 2);
  delete var;
}
