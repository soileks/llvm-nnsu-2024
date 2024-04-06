// RUN: %clang_cc1 -ast-dump -ast-dump-filter test -load %llvmshlibdir/AddAlwaysInlinePlugin%pluginext -add-plugin add-always-inline %s | FileCheck %s

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]|[0-9]+), (line|col):([0-9]+:[0-9]|[0-9]+)> (line|col):([0-9]+:[0-9]|[0-9]+) test 'void \(\)'}}
void test();

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]|[0-9]+), (line|col):([0-9]+:[0-9]|[0-9]+)> (line|col):([0-9]+:[0-9]|[0-9]+) testEmpty 'void \(\)'}}
void testEmpty() {}

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]+), line:([0-9]+:[0-9]+)> line:([0-9]+:[0-9]+) testReturn 'int \(int\)'}}
int testReturn(int a) {
    return a;
}

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]+), line:([0-9]+:[0-9]+)> line:([0-9]+:[0-9]+) testSum 'int \(int, int\)'}}
int testSum(int a, int b) {
    {
        return a + b;
    }
}

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]+), line:([0-9]+:[0-9]+)> line:([0-9]+:[0-9]+) testAssign 'void \(int, int\)'}}
// CHECK: AlwaysInlineAttr {{0[xX][0-9a-fA-F]+ <line:23:16> always_inline}}
__attribute__((always_inline)) void testAssign(int a, int b) {
    {}
    {
        {
            a = b;
        }
    }
}
// CHECK-NOT: `-AlwaysInlineAttr {{0[xX][0-9a-fA-F]+ <(line|col):([0-9]+:[0-9]|[0-9]+)> Implicit always_inline}}

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]+), line:([0-9]+:[0-9]+)> line:([0-9]+:[0-9]+) testWhile 'void \(int, int\)'}}
void testWhile(int a, int b) {
    while (a < b) {
        a += b;
    }
}
// CHECK-NOT: `-AlwaysInlineAttr {{0[xX][0-9a-fA-F]+ <(line|col):([0-9]+:[0-9]|[0-9]+)> Implicit always_inline}}

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]+), line:([0-9]+:[0-9]+)> line:([0-9]+:[0-9]+) testIf 'void \(int, int\)'}}
void testIf(int a, int b) {
    {
        if (a > b) {
            a -= b;
        }
    }
}
// CHECK-NOT: `-AlwaysInlineAttr {{0[xX][0-9a-fA-F]+ <(line|col):([0-9]+:[0-9]|[0-9]+)> Implicit always_inline}}

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]+), line:([0-9]+:[0-9]+)> line:([0-9]+:[0-9]+) testFor 'void \(int\)'}}
void testFor(int a) {
    {}
    for (int i = 0; i < a; ++i) {
        a += i;
    }
}
// CHECK-NOT: `-AlwaysInlineAttr {{0[xX][0-9a-fA-F]+ <(line|col):([0-9]+:[0-9]|[0-9]+)> Implicit always_inline}}

// CHECK: FunctionDecl {{0[xX][0-9a-fA-F]+ <.+test\.cpp:([0-9]+:[0-9]+), line:([0-9]+:[0-9]+)> line:([0-9]+:[0-9]+) testSwitch 'int \(int, int\)'}}
int testSwitch(int a, int b) {
    {{}{{}}}
    switch(a) {
        case 1:
            return a;
        case 2:
            return b;
    }
    {}
    return 0;
}
// CHECK-NOT: `-AlwaysInlineAttr {{0[xX][0-9a-fA-F]+ <(line|col):([0-9]+:[0-9]|[0-9]+)> Implicit always_inline}}

// RUN: %clang_cc1 -load %llvmshlibdir/AddAlwaysInlinePlugin%pluginext -plugin add-always-inline -plugin-arg-add-always-inline --help 2>&1 | FileCheck %s --check-prefix=CHECK-HELP
// CHECK-HELP: This plugin adds an __attribute__((always_inline)) to functions without any conditions