set(Title "Replace-Mult-Shift")
set(Student "Korablev-Nikita")
set(Group "FIIT3")
set(PluginName "${Title}_${Student}_${Group}")

# Result plugin name: Replace-Mult-Shift_Korablev-Nikita_FIIT3

if (NOT WIN32 AND NOT CYGWIN)
    message("-- ${PluginName} -- BUILD")

    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.hpp)
    add_llvm_pass_plugin(${PluginName} ${ALL_SOURCE_FILES}
    DEPENDS intrinsics_gen BUILDTREE_ONLY)
    
    set(LLVM_TEST_DEPENDS ${PluginName} ${LLVM_TEST_DEPENDS} PARENT_SCOPE)
else()
    message("-- ${PluginName} -- NOT BUILD")
endif()
