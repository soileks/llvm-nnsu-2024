// RUN: %clang_cc1 -load %llvmshlibdir/PrintClassesPlugin%pluginext -plugin print-classes -plugin-arg-print-classes --help %s 2>&1 | FileCheck %s --check-prefix=HELP

// HELP: Help text
// HELP-NOT: |_