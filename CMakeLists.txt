cmake_minimum_required(VERSION 3.10)

project(axi_dma_demo)

find_package(Boost 1.65 COMPONENTS log REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(axi_dma_demo)
target_include_directories(axi_dma_demo PRIVATE inc)
target_link_libraries(axi_dma_demo ${Boost_LOG_LIBRARY})
file(GLOB SOURCES "src/*.cpp")
target_sources(axi_dma_demo PRIVATE ${SOURCES})
target_compile_options(axi_dma_demo PRIVATE -Wall -Wextra)
