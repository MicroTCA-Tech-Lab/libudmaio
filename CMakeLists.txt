cmake_minimum_required(VERSION 3.10)

project(libudmaio)

find_package(Boost 1.65 COMPONENTS log program_options REQUIRED)

set(CMAKE_CXX_STANDARD 17)

# library
file(GLOB SOURCES "src/*.cpp")

add_library(udmaio SHARED ${SOURCES})
target_include_directories(udmaio PUBLIC inc)
file(GLOB UDMAIO_PUBLIC_HEADERS "inc/udmaio/*.hpp")
set_target_properties(udmaio PROPERTIES PUBLIC_HEADER "${UDMAIO_PUBLIC_HEADERS}")
target_link_libraries(udmaio Boost::log Boost::program_options Boost::dynamic_linking)
target_compile_options(udmaio PRIVATE -Wall -Wextra -O2)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0") 
    target_precompile_headers(udmaio PRIVATE inc/udmaio/BoostPCH.hpp)
endif()

include(GNUInstallDirs)

install(TARGETS udmaio
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/udmaio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# cpp example
add_executable(axi_dma_demo_cpp)
file(GLOB EXAMPLE_SOURCES "example/demo_cpp/src/*.cpp")
target_sources(axi_dma_demo_cpp PRIVATE ${EXAMPLE_SOURCES})
target_include_directories(axi_dma_demo_cpp PRIVATE example/demo_cpp/inc)
target_link_libraries(axi_dma_demo_cpp Boost::log Boost::program_options Boost::dynamic_linking udmaio)
target_compile_options(axi_dma_demo_cpp PRIVATE -Wall -Wextra -O2)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0") 
    target_precompile_headers(axi_dma_demo_cpp PRIVATE example/demo_cpp/inc/BoostPCH.hpp)
endif()

# Python example
# If we're running a Yocto SDK for cross compilation, force pybind11 to use target's Python headers
if(DEFINED ENV{SDKTARGETSYSROOT})
    # Find Python3 headers for the target
    string(STRIP "$ENV{SDKTARGETSYSROOT}/usr/include" TARGET_HEADERS)
    file(
        GLOB TARGET_PYTHON_HEADERS
        LIST_DIRECTORIES true
        "${TARGET_HEADERS}/python3*"
    )
    if(TARGET_PYTHON_HEADERS STREQUAL "")
        message(FATAL_ERROR "Found no Python3 headers under ${TARGET_HEADERS}")
    endif()
    # Override pybind11's Python search mechanism
    set(PYTHON_INCLUDE_DIRS ${TARGET_PYTHON_HEADERS} CACHE INTERNAL "Cross python include path")
    set(PYTHON_MODULE_EXTENSION ".so" CACHE INTERNAL "Cross python lib extension")
    set(PYTHONLIBS_FOUND TRUE CACHE INTERNAL "")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/example/demo_python/pybind11/CMakeLists.txt")
    message(FATAL_ERROR "pybind11 not found; make sure to 'git submodule init && git submodule update'")
endif()

add_subdirectory(example/demo_python/pybind11)
file(GLOB PY_BINDING_SOURCES "example/demo_python/src/*.cpp")
pybind11_add_module(pyudmaio ${PY_BINDING_SOURCES})
target_include_directories(pyudmaio PRIVATE example/demo_python/inc)
target_link_libraries(pyudmaio PRIVATE Boost::log Boost::program_options Boost::dynamic_linking udmaio)
