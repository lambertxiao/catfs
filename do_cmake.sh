#!/bin/sh

if test -e build;then
    echo "build dir already exists; rm -rf build and re-run"
    rm -rf build
fi

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    "$@" ..
