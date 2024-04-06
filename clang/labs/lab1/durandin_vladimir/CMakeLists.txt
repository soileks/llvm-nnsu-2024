add_llvm_library(AlwaysInlineAttributePlugin MODULE AlwaysInlineAttributePlugin.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(AlwaysInlineAttributePlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "AlwaysInlineAttributePlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)