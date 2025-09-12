if (${SETUP_DONE})
    message(STATUS "Setup was already done. Skipping")
    return()
endif ()
set(SETUP_DONE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Setting CMAKE_BUILD_TYPE to default value")
endif ()
message(STATUS "CMAKE_BUILD_TYPE is set to ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")
    add_compile_options(-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer)
endif ()
message(STATUS "CMAKE_BUILD_TYPE is set to: ${CMAKE_BUILD_TYPE}")

if (DEFINED SANITIZE)
    # address,undefined
    # address,undefined,leak
    # memory
    # thread
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZE}")
    message(STATUS "-fsanitize=${SANITIZE}")
endif ()

add_compile_options(-mavx -mavx2)
include(${CMAKE_CURRENT_LIST_DIR}/target_add_options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/idk_add_dependency_subdirectory.cmake)
