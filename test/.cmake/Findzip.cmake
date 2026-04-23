# zip
include(FetchContent)
FetchContent_Declare(
    zip
    GIT_REPOSITORY https://github.com/kuba--/zip.git
    GIT_TAG v0.3.2
)
set(CMAKE_ENABLE_EXPORTS ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zip)
