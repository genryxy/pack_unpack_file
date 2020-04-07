cmake_minimum_required(VERSION 3.14)
project(Krasnov_kdz_compression)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(Krasnov_kdz_compression main.cpp Huffman.cpp LZ77.cpp)