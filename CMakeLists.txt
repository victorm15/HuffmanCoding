cmake_minimum_required(VERSION 3.20)
project(Huffman C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(huffman STATIC
  src/huffman_tree.c
  src/code_io.c
  src/csv_writer.c
)
target_include_directories(huffman PUBLIC src)
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
  target_link_libraries(huffman PUBLIC ${MATH_LIBRARY})
endif()
target_compile_options(huffman PRIVATE -Wall -Wextra)

set(TOOLS
  compress
  decode
  write_results
)

foreach(tool IN LISTS TOOLS)
  add_executable(${tool} tools/${tool}.c)
  target_link_libraries(${tool} PRIVATE huffman)
  target_compile_options(${tool} PRIVATE -Wall -Wextra)
endforeach()