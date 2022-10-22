project(catfs)

set(CODE_ROOT "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include")

include_directories(${CODE_ROOT})
include_directories(${INCLUDE_ROOT})
include_directories("/usr/local/include")

aux_source_directory("$CODE_ROOT/fs" FS)
aux_source_directory("$CODE_ROOT/meta" META)
aux_source_directory("$CODE_ROOT/storage" STORAGE)
aux_source_directory("$CODE_ROOT/types" TYPES)
set(SRC_DIR ${FS} ${META} ${STORAGE} ${TYPES})

set(WARNING_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${WARNING_FLAGS} -std=c++17 -Wall -fPIC -DRTE_MACHINE_CPUFLAG_AVX2 -fstack-protector-all -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0 ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -ggdb -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(catfs ${CODE_ROOT}/main.cpp ${SRC_DIR})