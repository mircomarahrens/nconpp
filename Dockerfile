# Dockerfile for multi-stage container: https://devblogs.microsoft.com/cppblog/using-multi-stage-containers-for-c-development/
FROM ubuntu:24.04 as builder
LABEL description="NCONPP - Build container for CPP"

# install needed packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang-18 \
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

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
 && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100

RUN cd /tmp \
    && wget https://github.com/Kitware/CMake/releases/download/v4.1.2/cmake-4.1.2-linux-x86_64.sh \
    && chmod +x cmake-4.1.2-linux-x86_64.sh \
    && ./cmake-4.1.2-linux-x86_64.sh --prefix=/usr/local --skip-license \
    && rm cmake-4.1.2-linux-x86_64.sh

# need ninja >= 1.10.2
RUN cd /tmp \
    && wget https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip \
    && unzip ninja-linux.zip \
    && chmod +x ninja \
    && mv ninja /usr/bin/
    
WORKDIR /tmp/project

# build vcpkg
RUN cd /tmp/project \
    && git clone https://github.com/Microsoft/vcpkg.git \
    && cd vcpkg \
    && ./bootstrap-vcpkg.sh -useSystemBinaries -disableMetrics

COPY CMakePresets.json CMakeLists.txt vcpkg.json /tmp/project/
COPY cpp /tmp/project/cpp

RUN cmake --preset linux-debug

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
