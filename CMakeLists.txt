cmake_minimum_required (VERSION 3.11)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

include_directories(code_generator linaro_utils ast parsing vm)
project (linaro)
file(GLOB SOURCES "src/code_generator/*.cpp" "src/linaro_utils/*.cpp"
                    "src/ast/*.cpp" "src/parsing/*.cpp"
                    "src/vm/*.cpp" "src/*.cpp")
add_executable(linaro ${SOURCES})
target_compile_options(linaro PUBLIC -DDEBUG -std=c++17 -pedantic -Wall -Wfloat-conversion)
