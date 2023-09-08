LIBS="/Users/josh/gits/opencv/install/lib/*.dylib"
LIBS+=" -lz -lcurl -lcurlpp"

INCLUDES="-I/Users/josh/gits/opencv/install/include/opencv4"
INCLUDES+=" -I/Users/josh/gits/vcpkg/installed/arm64-osx/include"

/usr/bin/clang++ main.cpp src/*.cpp src/*/*.cpp -Iinclude/ $INCLUDES $LIBS -std=c++14 -fuse-ld=lld -o firmware
