cmake_minimum_required(VERSION 3.2)

project(TortTests)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
find_package(LLD REQUIRED CONFIG)

include_directories(../src)
include_directories(${LLVM_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(TortTests tests.cpp ${TARGET_SRC})

llvm_map_components_to_libnames(LLVM_LIBS windowsmanifest xray libdriver dlltooldriver coverage lineeditor xcoredisassembler xcorecodegen xcoredesc xcoreinfo x86targetmca x86disassembler x86asmparser x86codegen x86desc x86info webassemblydisassembler webassemblyasmparser webassemblycodegen webassemblydesc webassemblyutils webassemblyinfo vedisassembler veasmparser vecodegen vedesc veinfo systemzdisassembler systemzasmparser systemzcodegen systemzdesc systemzinfo sparcdisassembler sparcasmparser sparccodegen sparcdesc sparcinfo riscvtargetmca riscvdisassembler riscvasmparser riscvcodegen riscvdesc riscvinfo powerpcdisassembler powerpcasmparser powerpccodegen powerpcdesc powerpcinfo nvptxcodegen nvptxdesc nvptxinfo msp430disassembler msp430asmparser msp430codegen msp430desc msp430info mipsdisassembler mipsasmparser mipscodegen mipsdesc mipsinfo loongarchdisassembler loongarchasmparser loongarchcodegen loongarchdesc loongarchinfo lanaidisassembler lanaicodegen lanaiasmparser lanaidesc lanaiinfo hexagondisassembler hexagoncodegen hexagonasmparser hexagondesc hexagoninfo bpfdisassembler bpfasmparser bpfcodegen bpfdesc bpfinfo avrdisassembler avrasmparser avrcodegen avrdesc avrinfo armdisassembler armasmparser armcodegen armdesc armutils arminfo amdgputargetmca amdgpudisassembler amdgpuasmparser amdgpucodegen amdgpudesc amdgpuutils amdgpuinfo aarch64disassembler aarch64asmparser aarch64codegen aarch64desc aarch64utils aarch64info orcjit windowsdriver mcjit jitlink interpreter executionengine runtimedyld orctargetprocess orcshared dwp debuginfologicalview debuginfogsym option objectyaml objcopy mca mcdisassembler lto passes cfguard coroutines ipo vectorize linker instrumentation frontendopenmp frontendopenacc frontendhlsl extensions dwarflinkerparallel dwarflinker globalisel mirparser asmprinter selectiondag codegen objcarcopts irprinter interfacestub filecheck fuzzmutate target scalaropts instcombine aggressiveinstcombine transformutils bitwriter analysis profiledata symbolize debuginfopdb debuginfomsf debuginfodwarf object textapi mcparser irreader asmparser mc debuginfocodeview bitreader fuzzercli core remarks bitstreamreader binaryformat targetparser tablegen support demangle)

target_link_libraries(TortTests GTest::gtest_main ${LLD_LIBS} ${LLVM_LIBS})

include(GoogleTest)
gtest_discover_tests(TortTests)