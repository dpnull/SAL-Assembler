cmake_minimum_required(VERSION 3.10)

project(SAL-Ass3 VERSION 1.0 LANGUAGES CXX)

add_executable(SAL-Ass3
	src/main.cpp
    src/Assembler.cpp
)

target_include_directories(
	SAL-Ass3
	PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
    SAL-Ass3
    PRIVATE
    cxx_std_17
)