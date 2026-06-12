set(CEF_ROOT "${CMAKE_SOURCE_DIR}/third_party/cef" CACHE PATH "CEF distribution root")

if(NOT EXISTS "${CEF_ROOT}/include/cef_version.h")
    message(FATAL_ERROR
        "CEF not found at ${CEF_ROOT}.\n"
        "Download from https://cef-builds.spotifycdn.com/index.html\n"
        "Extract into third_party/cef/")
endif()

add_library(cef_import INTERFACE)
target_include_directories(cef_import INTERFACE "${CEF_ROOT}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CEF_LIB_DIR "${CEF_ROOT}/Debug")
else()
    set(CEF_LIB_DIR "${CEF_ROOT}/Release")
endif()

target_link_directories(cef_import INTERFACE "${CEF_LIB_DIR}")
target_link_libraries(cef_import INTERFACE
    libcef.lib
    "${CEF_ROOT}/libcef_dll_wrapper/libcef_dll_wrapper.lib")

add_library(CEF::cef ALIAS cef_import)
