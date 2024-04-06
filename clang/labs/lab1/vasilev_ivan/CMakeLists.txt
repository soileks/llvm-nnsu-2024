add_llvm_library(DeprecationPlugin MODULE Find_Deprecation.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(DeprecationPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "DeprecationPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)