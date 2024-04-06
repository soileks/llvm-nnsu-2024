// RUN: %clang_cc1 -load %llvmshlibdir/LysanovaDepWarnPlugin%pluginext -plugin depWarning %s 2>&1 | FileCheck %s

// CHECK: warning: Function have deprecated in its name!
void deprecated();

// CHECK: warning: Function have deprecated in its name!
void deprecated123();

// CHECK-NOT: warning: Function have deprecated in its name!
void abcdf();

// CHECK-NOT: warning: Function have deprecated in its name!
void eprecated();

// RUN: %clang_cc1 -load %llvmshlibdir/LysanovaDepWarnPlugin%pluginext -plugin depWarning %s -plugin-arg-depWarning help 2>&1 | FileCheck %s --check-prefix=HELP

// HELP: This plugin throws warning if func name contains 'deprecated'
