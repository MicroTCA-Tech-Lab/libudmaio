cmake_minimum_required(VERSION 3.15)

project(libudmaio VERSION 1.0.0)

find_package(Boost 1.65 COMPONENTS log program_options REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# library
file(GLOB SOURCES "src/*.cpp")

add_library(udmaio SHARED ${SOURCES})
target_include_directories(udmaio PUBLIC inc)
file(GLOB UDMAIO_PUBLIC_HEADERS "inc/udmaio/*.hpp")
set_target_properties(udmaio PROPERTIES PUBLIC_HEADER "${UDMAIO_PUBLIC_HEADERS}")
target_link_libraries(udmaio Boost::log Boost::program_options Boost::dynamic_linking)
target_compile_options(udmaio PRIVATE -Wall -Wextra -Wno-packed-bitfield-compat -Wno-missing-field-initializers -O2)
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
    
install(TARGETS udmaio
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/udmaio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES
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
target_include_directories(axi_dma_demo_cpp PRIVATE example/demo_cpp/inc)
target_link_libraries(axi_dma_demo_cpp Boost::log Boost::program_options Boost::dynamic_linking udmaio)
target_compile_options(axi_dma_demo_cpp PRIVATE -Wall -Wextra -Wno-packed-bitfield-compat -Wno-missing-field-initializers -O2)
target_compile_definitions(axi_dma_demo_cpp PRIVATE -DTARGET_HW=TARGET_HW_${TARGET_HW})
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0") 
    target_precompile_headers(axi_dma_demo_cpp PRIVATE example/demo_cpp/inc/BoostPCH.hpp)
endif()
