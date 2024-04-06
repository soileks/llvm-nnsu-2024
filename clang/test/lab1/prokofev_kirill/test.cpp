// RUN: split-file %s %t
// RUN: %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext -add-plugin rename\
// RUN: -plugin-arg-rename type=class\
// RUN: -plugin-arg-rename oldName=Alpha\
// RUN: -plugin-arg-rename newName=Beta %t/class_test.cpp
// RUN: FileCheck %s < %t/class_test.cpp --check-prefix=CLASS

// CLASS: class Beta {
// CLASS-NEXT: public:
// CLASS-NEXT:  Beta(){};
// CLASS-NEXT:  Beta(int a, int b): var1(a), var2(b) {}
// CLASS-NEXT:  ~Beta();
// CLASS-NEXT:protected:
// CLASS-NEXT:  int var1;
// CLASS-NEXT:  int var2;
// CLASS-NEXT: };
// CLASS-NEXT: void SomeFunc(){
// CLASS-NEXT:  Beta obj1;
// CLASS-NEXT:  Beta *obj2 = new Beta(5,6);
// CLASS-NEXT:  Beta AplphaVar;
// CLASS-NEXT:  delete obj2;
// CLASS-NEXT: }

//--- class_test.cpp
class Alpha {
public:
  Alpha(){};
  Alpha(int a, int b): var1(a), var2(b) {}
  ~Alpha();
protected:
  int var1;
  int var2;
};
void SomeFunc(){
  Alpha obj1;
  Alpha *obj2 = new Alpha(5,6);
  Alpha AplphaVar;
  delete obj2;
}

// RUN: %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext -add-plugin rename\
// RUN: -plugin-arg-rename type=func\
// RUN: -plugin-arg-rename oldName=Foo\
// RUN: -plugin-arg-rename newName=Function %t/function_test.cpp
// RUN: FileCheck %s < %t/function_test.cpp --check-prefix=FUNCTION

// FUNCTION: int Function(int digit) { return digit * 2; };

//--- function_test.cpp
int Foo(int digit) { return digit * 2; };

// RUN: %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext -add-plugin rename\
// RUN: -plugin-arg-rename type=var\
// RUN: -plugin-arg-rename oldName=oldVar\
// RUN: -plugin-arg-rename newName=newVar %t/variable_test.cpp
// RUN: FileCheck %s < %t/variable_test.cpp --check-prefix=VARIABLE

// VARIABLE: Foo(int newVar) { return newVar * 2; }

//--- variable_test.cpp
int Foo(int oldVar) { return oldVar * 2; }

// RUN: %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext\
// RUN: -add-plugin rename\
// RUN: -plugin-arg-rename help\
// RUN: 2>&1 | FileCheck %s --check-prefix=HELP

// RUN: %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext\
// RUN: -add-plugin rename\
// RUN: -plugin-arg-rename type=func\
// RUN: -plugin-arg-rename oldName=Foo\
// RUN: -plugin-arg-rename newName=Function\
// RUN: -plugin-arg-rename help\
// RUN: 2>&1 | FileCheck %s --check-prefix=HELP

// HELP: Specify three required arguments:
// HELP-NEXT: -plugin-arg-rename type=["var", "func", "class"]
// HELP-NEXT: -plugin-arg-rename oldName="Current identifier name"
// HELP-NEXT: -plugin-arg-rename newName="New identifier name"

// RUN: not %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext\
// RUN: -add-plugin rename\
// RUN: -plugin-arg-rename oldName=var1\
// RUN: -plugin-arg-rename newName=variable\
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

// RUN: not %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext\
// RUN: -add-plugin rename\
// RUN: -plugin-arg-rename type=undefined\
// RUN: -plugin-arg-rename oldName=var1\
// RUN: -plugin-arg-rename newName=variable\
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

// RUN: not %clang_cc1 -load %llvmshlibdir/ProkofevRenamePlugin%pluginext\
// RUN: -add-plugin rename\
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

//ERROR: Invalid arguments
//ERROR-NEXT: Specify "-plugin-arg-rename help" for usage


