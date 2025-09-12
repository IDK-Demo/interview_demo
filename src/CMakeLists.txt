macro(idk_add_subproject subproject)
    function(${subproject}_add_options target mode)
        target_add_project_options(${target} ${mode} ${subproject})
        target_include_directories(${target} ${mode} ${CMAKE_CURRENT_FUNCTION_LIST_DIR})
    endfunction()

    add_subdirectory(${subproject})
    target_include_directories(${subproject}_${subproject} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

    install(
            DIRECTORY ${subproject}
            DESTINATION "${CMAKE_BINARY_DIR}/include/"
            FILES_MATCHING PATTERN "*.h"
    )
endmacro()

idk_add_subproject(base)
idk_add_subproject(network)
idk_add_subproject(gateway)

