function(idk_add_dependency_subdirectory repo_name)
    string(TOUPPER "${repo_name}" project)

    get_property(PROJECT_INCLUDED GLOBAL PROPERTY "PROJECT_INCLUDED_${project}")
    message(STATUS "Adding dependency ${repo_name} for project ${PROJECT_NAME}")
    if (PROJECT_INCLUDED)
        message(STATUS "${repo_name} was already included. Skipping")
        return()
    endif ()
    message(STATUS "${repo_name} was not yet included. Adding...")
    set_property(GLOBAL PROPERTY "PROJECT_INCLUDED_${project}" TRUE)

    if(NOT DEFINED ${project}_PATH)
        message(FATAL_ERROR "You have to provide -D${project}_PATH=[path to ${repo_name}]")
    endif()
    add_subdirectory(${${project}_PATH} ${CMAKE_BINARY_DIR}/${repo_name})

endfunction()