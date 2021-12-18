FROM alpine:20210804 as builder
LABEL description="Build container - nconpp-core"

# install needed packages
RUN apk update && apk upgrade \
    && apk add --no-cache build-base bash curl zip unzip tar openssl \
    && apk add --no-cache cmake g++ gcc git gfortran make perl pkgconfig linux-headers \
    && apk add --no-cache lapack lapack-dev

# need ninja >= 1.10.2
RUN cd /tmp \
    && git clone git://github.com/ninja-build/ninja.git \
    && cd ninja \
    && git checkout release \
    && cmake -Bbuild-cmake -H. \
    && cmake --build build-cmake \
    && mv build-cmake/ninja /usr/bin/ninja

# build vcpkg
RUN cd /tmp \
    && git clone https://github.com/Microsoft/vcpkg.git \
    && cd vcpkg \
    && ./bootstrap-vcpkg.sh -useSystemBinaries -disableMetrics

# add triplet x64-linux-musl for alpine
RUN echo \
$'set(VCPKG_TARGET_ARCHITECTURE x64) \n\
set(VCPKG_CRT_LINKAGE static) \n\
set(VCPKG_LIBRARY_LINKAGE static) \n\
set(VCPKG_CMAKE_SYSTEM_NAME Linux)'\
> /tmp/vcpkg/triplets/x64-linux-musl.cmake

WORKDIR /tmp/project

COPY CMakeLists.txt vcpkg.json /tmp/project/
COPY nconpp-core /tmp/project/nconpp-core

RUN mkdir build \
    && cmake \
        -DCMAKE_MAKE_PROGRAM=/usr/bin/make \
        -DCMAKE_CXX_COMPILER=/usr/bin/gcc \
        -DCMAKE_TOOLCHAIN_FILE=/tmp/vcpkg/scripts/buildsystems/vcpkg.cmake \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DVCPKG_TARGET_TRIPLET=x64-linux-musl -H/tmp/project -B/tmp/project/build -G Ninja

ENTRYPOINT ["tail", "-f", "/dev/null"]

# FROM alpine:edge as tester
# LABEL description="Build container - nconpp-test"

# COPY --from=builder /tmp/vpckg /tmp/vcpkg
# COPY --from=builder /tmp/project /tmp/project
# COPY test /tmp/project/test

# RUN VCPKG_FORCE_SYSTEM_BINARIES=1 ./tmp/vcpkg/vcpkg --clean-after-build \
#         install gtest

# RUN cmake \
#         -DCMAKE_TOOLCHAIN_FILE=/tmp/vcpkg/scripts/buildsystems/vcpkg.cmake \
#         -DCMAKE_BUILD_TYPE:STRING=Debug \
#         -DVCPKG_TARGET_TRIPLET=x64-linux-musl -H/tmp/project -B/tmp/project/build -G Ninja

# ENTRYPOINT ["tail", "-f", "/dev/null"]
#RUN apk update && apk add --no-cache \
#        python3

#RUN VCPKG_FORCE_SYSTEM_BINARIES=1 ./tmp/vcpkg/vcpkg install pybind11

#ADD ["python", "/project"]
