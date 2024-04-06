add_llvm_library(VetoPluginPrintClasses MODULE print_classes.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(VetoPluginPrintClasses PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "VetoPluginPrintClasses" ${CLANG_TEST_DEPS} PARENT_SCOPE)