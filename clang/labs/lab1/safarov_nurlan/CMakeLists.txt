add_llvm_library(identifierRenamer MODULE identifierRenamer.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(identifierRenamer PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "identifierRenamer" ${CLANG_TEST_DEPS} PARENT_SCOPE)
