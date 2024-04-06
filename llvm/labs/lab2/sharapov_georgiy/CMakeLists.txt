# set(LLVM_LOOPFRAMER_LINK_INTO_TOOLS ON)
if (NOT WIN32 AND NOT CYGWIN)
    add_llvm_pass_plugin(LoopFramer LoopFramer.cpp
    DEPENDS
    intrinsics_gen
    BUILDTREE_ONLY)
    
    set(LLVM_TEST_DEPENDS LoopFramer ${LLVM_TEST_DEPENDS} PARENT_SCOPE)
endif()
