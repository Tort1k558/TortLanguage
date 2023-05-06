cmake_minimum_required(VERSION 3.2)

project(Tort)
set(CMAKE_CXX_STANDARD 17)

set(TARGET_SRC
	main.cpp
	src/AST.h
	src/AST.cpp
	src/externs.h
	src/externs.cpp
	src/Lexer.h
	src/Lexer.cpp
	src/Parser.h
	src/Parser.cpp
	src/Token.h
	src/TokenStream.h
	src/TokenStream.cpp
)

add_executable(Tort ${TARGET_SRC})

set(LLVM_DIR C:/Users/Tort1k/Desktop/llvm/llvm-project)

target_include_directories(Tort PUBLIC ${LLVM_DIR}/llvm/include)

target_link_directories(Tort PUBLIC ${LLVM_DIR}/build/Release/lib)
file(GLOB LIBRARIES ${LLVM_DIR}/build/Release/lib/*.lib)
target_link_libraries(Tort ${LIBRARIES})