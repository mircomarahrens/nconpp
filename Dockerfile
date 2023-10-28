# Dockerfile for multi-stage container: https://devblogs.microsoft.com/cppblog/using-multi-stage-containers-for-c-development/
FROM debian:latest as builder
LABEL description="Build container - nconpp-build"

# install needed packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    pkg-config \
    gfortran \
    curl \
    git \
    tar \
    zip \
    unzip \
    wget \
    --fix-missing \
    && rm -rf /var/lib/apt/lists/*

RUN cd /tmp \
    && wget https://github.com/Kitware/CMake/releases/download/v3.26.3/cmake-3.26.3-linux-x86_64.sh \
    && chmod +x cmake-3.26.3-linux-x86_64.sh \
    && ./cmake-3.26.3-linux-x86_64.sh --prefix=/usr/local --skip-license \
    && rm cmake-3.26.3-linux-x86_64.sh

# need ninja >= 1.10.2
RUN cd /tmp \
    && wget https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip \
    && unzip ninja-linux.zip \
    && chmod +x ninja \
    && mv ninja /usr/bin/

# build vcpkg
RUN cd /tmp \
    && git clone https://github.com/Microsoft/vcpkg.git \
    && cd vcpkg \
    && ./bootstrap-vcpkg.sh -useSystemBinaries -disableMetrics

WORKDIR /tmp/project

COPY CMakeLists.txt vcpkg.json /tmp/project/
COPY include /tmp/project/include

RUN mkdir build \
    && cmake \
        -DCMAKE_GENERATOR=Ninja \
        -DCMAKE_CXX_COMPILER=/usr/bin/gcc \
        -DCMAKE_TOOLCHAIN_FILE=/tmp/vcpkg/scripts/buildsystems/vcpkg.cmake \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DVCPKG_TARGET_TRIPLET=x64-linux \
        -H/tmp/project -B/tmp/project/build

ENTRYPOINT ["tail", "-f", "/dev/null"]

# FROM alpine:edge as tester
# LABEL description="Test container - nconpp-tester"

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
