LIBS+="-lcurlpp -lcurl"
LIBS+=" /usr/local/lib/libtvm_runtime.so"
LIBS+=" /usr/lib/x86_64-linux-gnu/libopencv*.so"
INCLUDES+=" -Iinclude/"
INCLUDES+=" -I/home/compute/gits.local/tvm/3rdparty/dlpack/include"
INCLUDES+=" -I/home/compute/gits.local/tvm/3rdparty/dmlc-core/include"

clang++ main.cpp src/*.cpp src/*/*.cpp $INCLUDES $LIBS -std=c++17 -fuse-ld=lld -o firmware
