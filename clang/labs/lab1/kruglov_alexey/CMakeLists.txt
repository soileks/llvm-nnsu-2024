add_llvm_library(RenameIDPlugin MODULE RenameID.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(RenameIDPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "RenameIDPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)