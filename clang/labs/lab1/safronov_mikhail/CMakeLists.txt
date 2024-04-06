add_llvm_library(PrintClassesPlugin MODULE PrintClasses.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(PrintClassesPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "PrintClassesPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)