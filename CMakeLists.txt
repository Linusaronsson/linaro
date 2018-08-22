cmake_minimum_required (VERSION 3.11)
include_directories(compiler linaro_utils parsing vm)
project (linaro)
file(GLOB SOURCES "src/compiler/*.cpp" "src/linaro_utils/*.cpp" "src/parsing/*.cpp"
    "src/vm/*.cpp" "src/*.cpp")
add_executable(linaro ${SOURCES})
target_compile_options(linaro PUBLIC -std=c++17 -pedantic -Wall -Wfloat-conversion)
