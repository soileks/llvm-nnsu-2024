add_llvm_library(AttrubuteAlwaysPlugin MODULE AttrubuteAlways.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(AttrubuteAlwaysPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "AttrubuteAlwaysPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)