cmake_minimum_required(VERSION 3.11)
project(raylib-zip
    DESCRIPTION "raylib-zip: Zip archive support for raylib"
    HOMEPAGE_URL "https://github.com/robloach/raylib-zip"
    VERSION 0.1.0
    LANGUAGES C
)

# raylib-zip
add_library(raylib_zip INTERFACE)
target_include_directories(raylib_zip INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(RAYLIB_ZIP_IS_MAIN TRUE)
else()
    set(RAYLIB_ZIP_IS_MAIN FALSE)
endif()
option(RAYLIB_ZIP_BUILD_TESTS "Build tests" ${RAYLIB_ZIP_IS_MAIN})

# Testing
if (RAYLIB_ZIP_BUILD_TESTS)
    include(CTest)
    enable_testing()
    if (BUILD_TESTING)
        set(CTEST_CUSTOM_TESTS_IGNORE
            pkg-config--static
        )
        list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
        add_subdirectory(test)
    endif()
endif()
