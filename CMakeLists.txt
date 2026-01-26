cmake_minimum_required(VERSION 3.14)
project(CPPTaiko VERSION 0.1.0 LANGUAGES C CXX)

cmake_policy(SET CMP0002 NEW)  # Logical target names must be globally unique
cmake_policy(SET CMP0079 NEW)  # Allow target_link_libraries() in different directories

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Dependency management
include(FetchContent)

### Dependencies

# raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(PLATFORM "SDL" CACHE STRING "" FORCE)
set(SUPPORT_MODULE_RAUDIO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# RapidJSON
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(rapidjson)

# tomlplusplus
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tomlplusplus)

# libsndfile
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ENABLE_EXTERNAL_LIBS ON CACHE BOOL "" FORCE)
set(ENABLE_MPEG ON CACHE BOOL "" FORCE)
set(ENABLE_OPUS ON CACHE BOOL "" FORCE)
set(ENABLE_FLAC ON CACHE BOOL "" FORCE)
set(ENABLE_VORBIS ON CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    libsndfile
    GIT_REPOSITORY https://github.com/libsndfile/libsndfile.git
    GIT_TAG 1.2.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libsndfile)

# libsamplerate
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(LIBSAMPLERATE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIBSAMPLERATE_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    libsamplerate
    GIT_REPOSITORY https://github.com/libsndfile/libsamplerate.git
    GIT_TAG 0.2.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libsamplerate)

# digestpp
FetchContent_Declare(
    digestpp
    GIT_REPOSITORY https://github.com/kerukuro/digestpp.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(digestpp)

### PortAudio (local prebuilt library)
# Determine platform-specific library name
if(WIN32)
    set(PORTAUDIO_LIB_NAME "libportaudio-windows.a")
elseif(APPLE)
    set(PORTAUDIO_LIB_NAME "libportaudio-macos.a")
elseif(UNIX)
    set(PORTAUDIO_LIB_NAME "libportaudio-linux.a")
endif()

set(PORTAUDIO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/libs/audio")
set(PORTAUDIO_LIBRARY "${CMAKE_SOURCE_DIR}/src/libs/audio/${PORTAUDIO_LIB_NAME}")

# Check if PortAudio library exists
if(NOT EXISTS ${PORTAUDIO_LIBRARY})
    message(FATAL_ERROR "PortAudio library not found: ${PORTAUDIO_LIBRARY}")
endif()

# Create imported library target for PortAudio
add_library(portaudio STATIC IMPORTED)
set_target_properties(portaudio PROPERTIES
    IMPORTED_LOCATION ${PORTAUDIO_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${PORTAUDIO_INCLUDE_DIR}
)

### Source files

file(GLOB_RECURSE CppFiles src/*.cpp)
file(GLOB_RECURSE HeaderFiles
    src/*.h
    src/*.hpp
)
message(STATUS "C++ files found: ${CppFiles}")

add_executable(${PROJECT_NAME} ${CppFiles} ${HeaderFiles})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/libs/audio
    ${PORTAUDIO_INCLUDE_DIR}
    ${rapidjson_SOURCE_DIR}/include
    ${digestpp_SOURCE_DIR}
    ${tomlplusplus_SOURCE_DIR}/include
    ${samplerate_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    sndfile
    samplerate
    portaudio
    tomlplusplus::tomlplusplus
)

# Platform specific (Thanks GPT)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
        gdi32
        winmm
    )
    # Static linking on Windows
    target_link_options(${PROJECT_NAME} PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-Wl,--allow-multiple-definition"
    )
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GLUT"
        "-framework OpenGL"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AudioUnit"
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GL
        m
        pthread
        dl
        rt
        X11
        asound
    )
    # Set rpath for audio library
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/src/libs/audio"
        INSTALL_RPATH "${CMAKE_SOURCE_DIR}/src/libs/audio"
    )
    find_library(JACK_LIB jack)
    if(JACK_LIB)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${JACK_LIB})
        target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_JACK)
    endif()
    find_library(PULSE_LIB pulse)
        if(PULSE_LIB)
            target_link_libraries(${PROJECT_NAME} PUBLIC ${PULSE_LIB})
            target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_PULSE)
        endif()
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    -O0  # No optimization for debug
    -g   # Debug symbols
    -fmax-errors=0
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Running ${PROJECT_NAME}..."
)

message(STATUS "")
message(STATUS "===============================================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "PortAudio Library: ${PORTAUDIO_LIBRARY}")
message(STATUS "===============================================================")
message(STATUS "")
