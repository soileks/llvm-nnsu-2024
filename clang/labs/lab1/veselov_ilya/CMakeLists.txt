add_llvm_library(PrintClassDescPlugin MODULE printClass.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(PrintClassDescPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "PrintClassDescPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)