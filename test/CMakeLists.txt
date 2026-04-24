
# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/.cmake)

find_package(raylib)
find_package(zip)
find_package(RaylibAssert)

# raylib_zip_test
add_executable(raylib_zip_test raylib_zip_test.c)
target_link_libraries(raylib_zip_test PUBLIC
    raylib
    raylib_zip
    zip
    raylib_assert
)

# C99
set_property(TARGET raylib_zip_test PROPERTY C_STANDARD 99)

# Warnings as errors
if(MSVC)
    target_compile_options(raylib_zip_test PRIVATE /W4 /WX)
else()
    target_compile_options(raylib_zip_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Copy the resources
file(GLOB resources resources/*)
set(test_resources)
list(APPEND test_resources ${resources})
file(COPY ${test_resources} DESTINATION "resources/")

# Set up the test
add_test(NAME raylib_zip_test COMMAND raylib_zip_test)
