add_llvm_library(renamedfilePlugin MODULE renamedfile.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(renamedfilePlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "renamedfilePlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)