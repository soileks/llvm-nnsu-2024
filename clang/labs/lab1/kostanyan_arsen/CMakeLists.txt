add_llvm_library(PrintNamesPlugin MODULE PrintNames.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(PrintNamesPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "PrintNamesPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)