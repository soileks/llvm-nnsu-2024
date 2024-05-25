add_llvm_library(PrintikClassPlugin MODULE PrintikClass.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(PrintikClassPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "PrintikClassPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)


