add_llvm_library(LabRenameIdenPlugin MODULE rename_identifiers.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
    set(LLVM_LINK_COMPONENTS
            Support
    )
    clang_target_link_libraries(LabRenameIdenPlugin PRIVATE
            clangAST
            clangBasic
            clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "LabRenameIdenPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)
