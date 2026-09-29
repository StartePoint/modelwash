# ModelWash
cmake_minimum_required(VERSION 3.21)
project(ModelWash VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Keep our libraries static even if Assimp toggles shared builds.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Allow -DQt6_DIR=... or CMAKE_PREFIX_PATH to point at Qt.
# Always prefer the local Qt 6.5.3 kit when present (CLion often misses it).
set(_mw_qt_home "C:/Qt/6.5.3/msvc2019_64")
if(EXISTS "${_mw_qt_home}/lib/cmake/Qt6/Qt6Config.cmake")
  list(PREPEND CMAKE_PREFIX_PATH "${_mw_qt_home}")
endif()
if(DEFINED ENV{CMAKE_PREFIX_PATH})
  list(APPEND CMAKE_PREFIX_PATH "$ENV{CMAKE_PREFIX_PATH}")
endif()
if(NOT Qt6_DIR AND NOT CMAKE_PREFIX_PATH)
  foreach(_qt_candidate
      "C:/Qt/6.5.3/msvc2022_64"
      "C:/Qt/6.6.3/msvc2019_64"
      "C:/Qt/6.7.3/msvc2019_64"
      "C:/Qt/6.8.0/msvc2019_64")
    if(EXISTS "${_qt_candidate}/lib/cmake/Qt6/Qt6Config.cmake")
      list(PREPEND CMAKE_PREFIX_PATH "${_qt_candidate}")
      break()
    endif()
  endforeach()
endif()
list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
message(STATUS "ModelWash CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Concurrent)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Assimp (importer)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.4.3
  GIT_SHALLOW TRUE
)

# Disable Assimp's broken MSVC "copy DLLs into source tree" utility target before configure.
# (It shows up in CLion target list and fails for static/Ninja builds.)
set(_mw_assimp_cmake "")
if(DEFINED FETCHCONTENT_SOURCE_DIR_ASSIMP AND EXISTS "${FETCHCONTENT_SOURCE_DIR_ASSIMP}/CMakeLists.txt")
  set(_mw_assimp_cmake "${FETCHCONTENT_SOURCE_DIR_ASSIMP}/CMakeLists.txt")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/build/_deps/assimp-src/CMakeLists.txt")
  set(_mw_assimp_cmake "${CMAKE_SOURCE_DIR}/build/_deps/assimp-src/CMakeLists.txt")
endif()
if(_mw_assimp_cmake)
  file(READ "${_mw_assimp_cmake}" _mw_assimp_cmake_txt)
  string(REPLACE
    "IF(MSVC12 OR MSVC14 OR MSVC15 )"
    "IF(FALSE) # ModelWash: disabled UpdateAssimpLibsDebugSymbolsAndDLLs"
    _mw_assimp_cmake_txt "${_mw_assimp_cmake_txt}")
  file(WRITE "${_mw_assimp_cmake}" "${_mw_assimp_cmake_txt}")
endif()

FetchContent_MakeAvailable(assimp)

# meshoptimizer
FetchContent_Declare(
  meshoptimizer
  GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
  GIT_TAG v0.22
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(meshoptimizer)

add_subdirectory(src/core)
add_subdirectory(src/io)
add_subdirectory(src/presentation)
add_subdirectory(src/app)

enable_testing()
add_subdirectory(tests)
