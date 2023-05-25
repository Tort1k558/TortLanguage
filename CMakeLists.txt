cmake_minimum_required(VERSION 3.2)

project(Tort)
set(CMAKE_CXX_STANDARD 17)
set(LLVM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/llvmBuild/lib/cmake/llvm")
set(LLD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/llvmBuild/lib/cmake/lld")

file(GLOB TARGET_SRC "src/*.cpp" "src/*.h")


add_subdirectory(gtests)

find_package(LLVM REQUIRED CONFIG)
find_package(LLD REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(Tort ${TARGET_SRC} main.cpp)

llvm_map_components_to_libnames(LLVM_LIBS windowsmanifest xray libdriver dlltooldriver coverage lineeditor xcoredisassembler xcorecodegen xcoredesc xcoreinfo x86targetmca x86disassembler x86asmparser x86codegen x86desc x86info webassemblydisassembler webassemblyasmparser webassemblycodegen webassemblydesc webassemblyutils webassemblyinfo vedisassembler veasmparser vecodegen vedesc veinfo systemzdisassembler systemzasmparser systemzcodegen systemzdesc systemzinfo sparcdisassembler sparcasmparser sparccodegen sparcdesc sparcinfo riscvtargetmca riscvdisassembler riscvasmparser riscvcodegen riscvdesc riscvinfo powerpcdisassembler powerpcasmparser powerpccodegen powerpcdesc powerpcinfo nvptxcodegen nvptxdesc nvptxinfo msp430disassembler msp430asmparser msp430codegen msp430desc msp430info mipsdisassembler mipsasmparser mipscodegen mipsdesc mipsinfo loongarchdisassembler loongarchasmparser loongarchcodegen loongarchdesc loongarchinfo lanaidisassembler lanaicodegen lanaiasmparser lanaidesc lanaiinfo hexagondisassembler hexagoncodegen hexagonasmparser hexagondesc hexagoninfo bpfdisassembler bpfasmparser bpfcodegen bpfdesc bpfinfo avrdisassembler avrasmparser avrcodegen avrdesc avrinfo armdisassembler armasmparser armcodegen armdesc armutils arminfo amdgputargetmca amdgpudisassembler amdgpuasmparser amdgpucodegen amdgpudesc amdgpuutils amdgpuinfo aarch64disassembler aarch64asmparser aarch64codegen aarch64desc aarch64utils aarch64info orcjit windowsdriver mcjit jitlink interpreter executionengine runtimedyld orctargetprocess orcshared dwp debuginfologicalview debuginfogsym option objectyaml objcopy mca mcdisassembler lto passes cfguard coroutines ipo vectorize linker instrumentation frontendopenmp frontendopenacc frontendhlsl extensions dwarflinkerparallel dwarflinker globalisel mirparser asmprinter selectiondag codegen objcarcopts irprinter interfacestub filecheck fuzzmutate target scalaropts instcombine aggressiveinstcombine transformutils bitwriter analysis profiledata symbolize debuginfopdb debuginfomsf debuginfodwarf object textapi mcparser irreader asmparser mc debuginfocodeview bitreader fuzzercli core remarks bitstreamreader binaryformat targetparser tablegen support demangle)

message(STATUS "libs: ${LLD_EXPORTED_TARGETS} ${LLVM_LIBS}")

target_link_libraries(Tort ${LLD_EXPORTED_TARGETS} ${LLVM_LIBS})