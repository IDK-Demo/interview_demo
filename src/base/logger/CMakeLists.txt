file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE py_sources "py_module.cpp")
list(REMOVE_ITEM sources ${py_sources})
message(STATUS "sources: ${sources}")
file(GLOB_RECURSE headers "*.h")

if(idk_BUILD_PYTHON_LIBS)
    message(STATUS "idk_BUILD_PYTHON_LIBS is set to ON. Building logger as a shared lib")
    add_library(base_logger SHARED)
    set_target_properties(base_logger PROPERTIES
            INSTALL_RPATH ${idk_RPATH}
            BUILD_WITH_INSTALL_RPATH TRUE
    )
    install(TARGETS base_logger
            LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
    )
else()
    add_library(base_logger STATIC)
endif()

add_library(base::logger ALIAS base_logger)
target_sources(base_logger PUBLIC ${headers} PRIVATE ${sources})

target_link_libraries(base_logger PUBLIC fmt::fmt quill::quill reflectcpp::reflectcpp)
base_add_options(base_logger PUBLIC)

if(idk_BUILD_PYTHON_LIBS)
    idk_add_python_module(_CppLogger_ ${py_sources})
    target_link_libraries(_CppLogger_ PRIVATE base::logger)
endif()
