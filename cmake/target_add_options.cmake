function(target_add_global_options target mode)
    target_add_project_options(${target} ${mode} "")
endfunction()

function(target_add_project_options target mode project)
    set_target_properties(${target}
            PROPERTIES
            LINKER_LANGUAGE CXX
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${project}"
            EXPORT_COMPILE_COMMANDS ON
    )

    target_compile_features(${target} ${mode}
            cxx_std_20
    )

    target_compile_options(${target} ${mode}
            -Wunreachable-code -Wno-missing-braces
            # FIXME
    )
endfunction()