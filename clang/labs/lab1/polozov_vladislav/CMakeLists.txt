add_llvm_library(PluginWarningDeprecated MODULE WarningForDeprecated.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(PluginWarningDeprecated PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "PluginWarningDeprecated" ${CLANG_TEST_DEPS} PARENT_SCOPE)