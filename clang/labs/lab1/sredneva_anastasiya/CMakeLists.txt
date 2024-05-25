add_llvm_library(Deprecated_sa MODULE Deprecated_sa.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(Deprecated_sa PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "Deprecated_sa" ${CLANG_TEST_DEPS} PARENT_SCOPE)