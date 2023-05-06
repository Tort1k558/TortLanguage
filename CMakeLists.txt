cmake_minimum_required(VERSION 3.2)

project(Tort)
set(CMAKE_CXX_STANDARD 17)

file(GLOB TARGET_SRC "src/*.cpp" "src/*.h")

add_executable(Tort ${TARGET_SRC} main.cpp)

set(LLVM_DIR C:/Users/Tort1k/Desktop/llvm/llvm-project)

target_include_directories(Tort PUBLIC ${LLVM_DIR}/llvm/include)
target_include_directories(Tort PUBLIC ${LLVM_DIR}/build/include)

target_link_directories(Tort PUBLIC ${LLVM_DIR}/build/Release/lib)
file(GLOB LIBRARIES ${LLVM_DIR}/build/Release/lib/*.lib)
target_link_libraries(Tort ${LIBRARIES})