add_llvm_library(LebedevaRenamePlugin MODULE LebedevaRenameId.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(LebedevaRenamePlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "LebedevaRenamePlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)
