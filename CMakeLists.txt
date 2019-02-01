cmake_minimum_required (VERSION 3.10 FATAL_ERROR)
project (morgoth)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_BUILD_TYPE Debug)
else ()
    set(CMAKE_BUILD_TYPE Release)
endif ()

# find version
if (NOT morgoth_VERSION)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION morgoth_VERSION)
    string (STRIP ${morgoth_VERSION} morgoth_VERSION)

    include (GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)

    if (NOT "${GIT_SHA1}" STREQUAL "GITDIR-NOTFOUND")
        # shorten SHA
        string (SUBSTRING "${GIT_SHA1}" 0 10 GIT_HASH)
        set (morgoth_VERSION "${morgoth_VERSION}-${GIT_HASH}-git")
    endif ()
endif ()

set (CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -Wall -Wextra -Wpedantic")
set (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined -Wl,--exclude-libs,ALL")
else ()
    set (CMAKE_SHARED_LINKER_FLAGS "-Wl,-undefined,error")
endif ()

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_subdirectory (src)

enable_testing ()
add_subdirectory (test)

# uninstall target
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target (
    uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

message (STATUS "morgoth-${morgoth_VERSION} build summary:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
