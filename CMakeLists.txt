project(catfs)

set(CODE_ROOT "${PROJECT_SOURCE_DIR}/src")
include_directories(${CODE_ROOT})

include_directories("${PROJECT_SOURCE_DIR}/deps/include")
link_directories("${PROJECT_SOURCE_DIR}/deps/lib")

include_directories("/root/workspace/awslib/include")
link_directories("/root/workspace/awslib/lib")

aux_source_directory("${CODE_ROOT}/fs" FS)
aux_source_directory("${CODE_ROOT}/fs/freader" FS_READER)
aux_source_directory("${CODE_ROOT}/fs/fwriter" FS_WRITER)
aux_source_directory("${CODE_ROOT}/meta" META)
aux_source_directory("${CODE_ROOT}/stor" STORAGE)
aux_source_directory("${CODE_ROOT}/types" TYPES)
aux_source_directory("${CODE_ROOT}/util" UTIL)
set(SRC_DIR ${FS} ${FS_READER} ${FS_WRITER} ${META} ${STORAGE} ${TYPES} ${UTIL})

set(WARNING_FLAGS "-Wall")

set(CMAKE_C_COMPILER "/usr/local/bin/g++")
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")

set(CMAKE_CXX_FLAGS "${WARNING_FLAGS} -std=c++17 -Wall -fPIC -DRTE_MACHINE_CPUFLAG_AVX2 -fstack-protector-all -lpthread -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O0 ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -ggdb -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(AWSSDK REQUIRED COMPONENTS s3)

# Message(${AWSSDK})

add_executable(catfs ${CODE_ROOT}/main.cpp ${SRC_DIR})

# 链接静态库
target_link_libraries(
    catfs dl
    libfuse3.a libfmt.a libfmtlog-static.a libyaml-cpp.a libpugixml.a
    # libcurlpp.a libcurl.a libssl.a libcrypto.a libz.a libinih.a
    ${AWSSDK_LINK_LIBRARIES}
)
