cmake_minimum_required(VERSION 3.10)

project(axi_dma_demo)

find_package(Boost 1.65 COMPONENTS log program_options REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(axi_dma_demo)
target_include_directories(axi_dma_demo PRIVATE inc)
target_link_libraries(axi_dma_demo ${Boost_LIBRARIES})
file(GLOB SOURCES "src/*.cpp")
target_sources(axi_dma_demo PRIVATE ${SOURCES})
target_compile_options(axi_dma_demo PRIVATE -Wall -Wextra -O2)
#target_link_options(axi_dma_demo PRIVATE -pg -fprofile-arcs)

