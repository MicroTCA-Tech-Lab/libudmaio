cmake_minimum_required(VERSION 3.10)

project(libdmamgmt)

find_package(Boost 1.65 COMPONENTS log program_options REQUIRED)

set(CMAKE_CXX_STANDARD 17)

# library
file(GLOB SOURCES "src/*.cpp")

add_library(dmamgmt SHARED ${SOURCES})
target_include_directories(dmamgmt PUBLIC inc)
target_link_libraries(dmamgmt ${Boost_LIBRARIES})
target_compile_options(dmamgmt PRIVATE -Wall -Wextra -O2)

include(GNUInstallDirs)

install(TARGETS dmamgmt
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

file(GLOB LIBDMAMGMT_PUBLIC_HEADERS "inc/*.hpp")
install(FILES ${LIBDMAMGMT_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libdmamgmt)

# example
add_executable(axi_dma_demo example/axi_dma_demo.cpp)
target_include_directories(axi_dma_demo PRIVATE inc)
target_link_libraries(axi_dma_demo ${Boost_LIBRARIES} dmamgmt)
target_compile_options(axi_dma_demo PRIVATE -Wall -Wextra -O2)
