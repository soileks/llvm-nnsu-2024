if (NOT WIN32 AND NOT CYGWIN)
    add_llvm_pass_plugin(PolozovLoopPass LoopPass.cpp
    DEPENDS
    intrinsics_gen
    BUILDTREE_ONLY)
    set(LLVM_TEST_DEPENDS PolozovLoopPass ${LLVM_TEST_DEPENDS} PARENT_SCOPE)
endif()
