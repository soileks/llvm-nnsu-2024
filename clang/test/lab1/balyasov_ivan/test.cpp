// RUN: split-file %s %t

// RUN: %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds type=var\
// RUN: -plugin-arg-renameIds cur-name=a\
// RUN: -plugin-arg-renameIds new-name=new_var %t/rename_var.cpp
// RUN: FileCheck %s < %t/rename_var.cpp --check-prefix=ReVar

// ReVar: int func() {
// ReVar-NEXT: int new_var = 2, b = 2;
// ReVar-NEXT: new_var = b + new_var;
// ReVar-NEXT: new_var++;
// ReVar-NEXT:  return new_var;
// ReVar-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds type=var\
// RUN: -plugin-arg-renameIds cur-name=c\
// RUN: -plugin-arg-renameIds new-name=new_var %t/rename_non_existent_var.cpp
// RUN: FileCheck %s < %t/rename_non_existent_var.cpp --check-prefix=NON_EXIST_VAR

// NON_EXIST_VAR: int func() {
// NON_EXIST_VAR-NEXT: int a = 2;
// NON_EXIST_VAR-NEXT: int b = 3;
// NON_EXIST_VAR-NEXT: b += a;
// NON_EXIST_VAR-NEXT: a++;
// NON_EXIST_VAR-NEXT:  return b - a;
// NON_EXIST_VAR-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds type=func\
// RUN: -plugin-arg-renameIds cur-name=function\
// RUN: -plugin-arg-renameIds new-name=new_func %t/rename_func.cpp
// RUN: FileCheck %s < %t/rename_func.cpp --check-prefix=FUNC

// FUNC: int new_func(int param) {
// FUNC-NEXT: int a;
// FUNC-NEXT: a = 2;
// FUNC-NEXT: return param + a;
// FUNC-NEXT: }
// FUNC-NEXT: int other_func(){
// FUNC-NEXT: new_func(3);
// FUNC-NEXT: int a = new_func(2) + 3;
// FUNC-NEXT: return a;
// FUNC-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds type=func\
// RUN: -plugin-arg-renameIds cur-name=function\
// RUN: -plugin-arg-renameIds new-name=f %t/rename_non_existent_func.cpp
// RUN: FileCheck %s < %t/rename_non_existent_func.cpp --check-prefix=NON_EXIST_FUNC

// NON_EXIST_FUNC: int func(int a) {
// NON_EXIST_FUNC-NEXT: int b = 2;
// NON_EXIST_FUNC-NEXT: return a + b;
// NON_EXIST_FUNC-NEXT: }
// NON_EXIST_FUNC: void func2() {
// NON_EXIST_FUNC-NEXT: int c = func(2);
// NON_EXIST_FUNC-NEXT: int b = func(c) + func(3);
// NON_EXIST_FUNC-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds type=class\
// RUN: -plugin-arg-renameIds cur-name=Base\
// RUN: -plugin-arg-renameIds new-name=SimpleClass %t/rename_class.cpp
// RUN: FileCheck %s < %t/rename_class.cpp --check-prefix=ReClass

// ReClass: class SimpleClass{
// ReClass-NEXT: private:
// ReClass-NEXT: int a;
// ReClass-NEXT: int b;
// ReClass-NEXT: public:
// ReClass-NEXT: SimpleClass() {}
// ReClass-NEXT: SimpleClass(int a, int b): a(a), b(b) {}
// ReClass-NEXT: ~SimpleClass();
// ReClass-NEXT: };
// ReClass: void func() {
// ReClass-NEXT: SimpleClass a;
// ReClass-NEXT: SimpleClass* var = new SimpleClass(1, 2);
// ReClass-NEXT: delete var;
// ReClass-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds help\
// RUN: 2>&1 | FileCheck %s --check-prefix=HELP

// HELP: Specify three required arguments:
// HELP-NEXT: -plugin-arg-renameIds type=["Var", "Func", "Class"]
// HELP-NEXT: -plugin-arg-renameIds cur-name="Current identifier name"
// HELP-NEXT: -plugin-arg-renameIds new-name="New identifier name"

// RUN: not %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds cur-name=B\
// RUN: -plugin-arg-renameIds new-name=C\
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

// RUN: not %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds cur-name=B\
// RUN: -plugin-arg-renameIds new-name=C\
// RUN: -plugin-arg-renameIds param=val\
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

// RUN: not %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: -plugin-arg-renameIds type=undefined\
// RUN: -plugin-arg-renameIds cur-name=B\
// RUN: -plugin-arg-renameIds new-name=C\
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

// RUN: not %clang_cc1 -load %llvmshlibdir/LabRenameIdenPlugin%pluginext\
// RUN: -add-plugin renameIds\
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

//ERROR: Invalid arguments
//ERROR-NEXT: Specify "-plugin-arg-renameIds help" for usage

//--- rename_var.cpp
int func() {
  int a = 2, b = 2;
  a = b + a;
  a++;
  return a;
}
//--- rename_non_existent_var.cpp
int func() {
  int a = 2;
  int b = 3;
  b += a;
  a++;
  return b - a;
}
//--- rename_func.cpp
int function(int param) {
    int a;
    a = 2;
    return param + a;
}
int other_func(){
  function(3);
  int a = function(2) + 3;
  return a;
}
//--- rename_non_existent_func.cpp
int func(int a) {
  int b = 2;
  return a + b;
}

void func2() {
  int c = func(2);
  int b = func(c) + func(3);
}
//--- rename_class.cpp
class Base{
 private:
  int a;
  int b;
 public:
  Base() {}
  Base(int a, int b): a(a), b(b) {}
  ~Base();
};

void func() {
  Base a;
  Base* var = new Base(1, 2);
  delete var;
}

