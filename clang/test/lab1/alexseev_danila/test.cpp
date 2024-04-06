// RUN: %clang_cc1 -load %llvmshlibdir/depWarningPluginAlexseev%pluginext -plugin deprecated-warning -plugin-arg-deprecated-warning -exclude=deprecatedFunc %s 2>&1 | FileCheck %s

// CHECK: warning: Function contains 'deprecated' in its name
void deprecated();

// CHECK: warning: Function contains 'deprecated' in its name
void deprecated123();

// CHECK: warning: Function contains 'deprecated' in its name
void aaAdeprecatedOoo();

// CHECK-NOT: warning: Function contains 'deprecated' in its name
void something();

// CHECK-NOT: warning: Function contains 'deprecated' in its name
void deprecatend();

// CHECK-NOT: warning: Function contains 'deprecated' in its name
void deprecate();

// CHECK-NOT: warning: Function contains 'deprecated' in its name
void deprecatedFunc();