add_llvm_library(KosarevAlwaysInlinePlugin MODULE always_inline.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(KosarevAlwaysInlinePlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )

endif()
set(CLANG_TEST_DEPS "KosarevAlwaysInlinePlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)