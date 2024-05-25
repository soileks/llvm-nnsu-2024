add_llvm_library(addWarning MODULE AddWarning.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(addWarning PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "addWarning" ${CLANG_TEST_DEPS} PARENT_SCOPE)