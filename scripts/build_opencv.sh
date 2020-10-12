#!/bin/bash
cmake -H. -BRelease \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DINSTALL_C_EXAMPLES=OFF \
    -DINSTALL_PYTHON_EXAMPLES=OFF \
    -DOPENCV_ENABLE_NONFREE=ON \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_DOCS=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DWITH_TBB=ON \
    -DWITH_IPP=ON \
    -DWITH_OPENCL=ON \
    -DWITH_QT=ON \
    -DWITH_V4L=ON

cd Release
realnum=`cat /proc/cpuinfo | grep cores | wc -l `
let bestnum=$realnum+$(printf %.0f `echo "$realnum*0.2"|bc`)
make -j${bestnum}
make install
