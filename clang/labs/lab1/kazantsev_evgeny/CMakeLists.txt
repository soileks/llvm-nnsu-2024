add_llvm_library(DeprecatedPlug MODULE DeprecatedPlug.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
	set(LLVM_LINK_COMPONENTS SUpport)
	clang_target_link_libraries(DeprecatedPlug PRIVATE
		clangAST
		clangBasic
		clangFrontend)
endif()

set(CLANG_TEST_DEPS "DeprecatedPlug" ${CLANG_TEST_DEPS} PARENT_SCOPE)
