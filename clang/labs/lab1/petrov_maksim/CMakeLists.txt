add_llvm_library(WarningDeprecatedPlugin MODULE WarningDeprecatedPlugin.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(WarningDeprecatedPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "WarningDeprecatedPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)
