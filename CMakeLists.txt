cmake_minimum_required(VERSION 3.15)

project(libudmaio VERSION 1.5.2)

include(CTest)

find_package(Boost 1.65 COMPONENTS log program_options unit_test_framework REQUIRED)
add_definitions(-DBOOST_LOG_DYN_LINK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# library
file(GLOB SOURCES "src/*.cpp")

add_library(udmaio SHARED ${SOURCES})
target_include_directories(udmaio PUBLIC inc)
target_link_libraries(udmaio Boost::log Boost::dynamic_linking)
target_compile_options(udmaio PRIVATE -Wall -Wextra -Wno-packed-bitfield-compat -Wno-missing-field-initializers -Wno-psabi -O2)
set_target_properties(udmaio PROPERTIES VERSION ${PROJECT_VERSION})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    target_precompile_headers(udmaio PRIVATE inc/udmaio/BoostPCH.hpp)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(udmaio-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/udmaio-config.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/udmaio
    PATH_VARS CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/udmaio-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    TARGETS udmaio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/inc/udmaio"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
)
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/udmaio-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/udmaio-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/udmaio"
)

# Invoke cmake with -DTARGET_HW=ZUP or -DTARGET_HW=Z7IO. ZUP is the default
if("${TARGET_HW}" STREQUAL "")
    message(WARNING "Set target hardware, e.g. cmake -DTARGET_HW=ZUP\nSupported targets: ZUP, Z7IO\nUsing ZUP as default target")
    set(TARGET_HW "ZUP")
endif()

# cpp example
add_executable(axi_dma_demo_cpp)
file(GLOB EXAMPLE_SOURCES "example/demo_cpp/src/*.cpp")
target_sources(axi_dma_demo_cpp PRIVATE ${EXAMPLE_SOURCES})
target_include_directories(axi_dma_demo_cpp PRIVATE example/demo_cpp/inc inc)
target_link_libraries(axi_dma_demo_cpp Boost::log Boost::program_options Boost::dynamic_linking udmaio)
target_compile_options(axi_dma_demo_cpp PRIVATE -Wall -Wextra -Wno-packed-bitfield-compat -Wno-missing-field-initializers -O2)
target_compile_definitions(axi_dma_demo_cpp PRIVATE -DTARGET_HW=TARGET_HW_${TARGET_HW})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    target_precompile_headers(axi_dma_demo_cpp PRIVATE example/demo_cpp/inc/BoostPCH.hpp)
endif()

if(BUILD_TESTING)
    # library tests
    file(GLOB LIB_TESTS "tests/*.cpp")

    foreach(TEST_FILE ${LIB_TESTS})
        get_filename_component(test_basename ${TEST_FILE} NAME_WE)

        add_executable(${test_basename} ${TEST_FILE})
        target_include_directories(${test_basename} PRIVATE inc)
        target_link_libraries(${test_basename} LINK_PUBLIC Boost::unit_test_framework Boost::log Boost::program_options Boost::dynamic_linking udmaio)
        add_test(NAME ${test_basename} COMMAND ${test_basename})
    endforeach()

    # cpp example tests
    file(GLOB TEST_SOURCES "example/demo_cpp/src/*.cpp")
    file(GLOB EXAMPLE_MAIN "example/demo_cpp/src/axi_dma_demo.cpp")
    list(REMOVE_ITEM TEST_SOURCES ${EXAMPLE_MAIN})
    add_library(test-objs OBJECT ${TEST_SOURCES})
    target_include_directories(test-objs PRIVATE example/demo_cpp/inc inc)

    file(GLOB TESTS "example/demo_cpp/tests/*.cpp")

    foreach(TEST_FILE ${TESTS})
        get_filename_component(test_basename ${TEST_FILE} NAME_WE)

        add_executable(${test_basename} ${TEST_FILE} $<TARGET_OBJECTS:test-objs>)
        target_include_directories(${test_basename} PRIVATE example/demo_cpp/inc example/demo_cpp/tests/inc inc)
        target_link_libraries(${test_basename} LINK_PUBLIC Boost::unit_test_framework Boost::log Boost::program_options Boost::dynamic_linking udmaio)
        add_test(NAME ${test_basename} COMMAND ${test_basename})
    endforeach()
endif()

# The python binding is usually installed via setuptools;
# we include the C++ part of it here only to populate the compile-command.json for editors/IDEs
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/pyudmaio/pybind11/CMakeLists.txt")
    message("pybind11 submodule found, adding pyudmaio target")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pyudmaio/pybind11")
    pybind11_add_module(pyudmaio EXCLUDE_FROM_ALL
        "${CMAKE_CURRENT_LIST_DIR}/pyudmaio/src/PythonBinding.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/pyudmaio/src/DataHandlerPython.cpp"
    )
else()
    message("skipping python binding, clone pybind11 submodule to include it")
endif()

# Create debian package containing library
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Patrick Huesmann <patrick.huesmann@desy.de>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/MicroTCA-Tech-Lab/libudmaio")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-log-dev") # use -dev b/c it's independent of package version
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Userspace I/O library for Xilinx AXI S2MM DMA ")
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/devel")
set(CPACK_PACKAGE_NAME "libudmaio")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
include(CPack)
