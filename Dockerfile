# -----------------------------------------------------------------------------
# Base toolchain & vcpkg setup
# -----------------------------------------------------------------------------
FROM ubuntu:24.04 AS base
LABEL description="NCONPP - Base toolchain container"

# Install core build, debug, and dev dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang-18 \
    clang-tools-18 \
    clangd-18 \
    clang-format-18 \
    clang-tidy-18 \
    lldb-18 \
    gdb \
    cmake \
    ninja-build \
    ca-certificates \
    pkg-config \
    gfortran \
    curl \
    libcurl4-openssl-dev \
    git \
    tar \
    zip \
    unzip \
    sudo \
    stow \
    openssh-client \
    && rm -rf /var/lib/apt/lists/* \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100 \
    && update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-18 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100 \
    && update-alternatives --install /usr/bin/clang-scan-deps clang-scan-deps /usr/bin/clang-scan-deps-18 100

# Remove default Ubuntu 24.04 user and create developer user with UID 1000
RUN (userdel -r ubuntu 2>/dev/null || true) \
    && (groupdel ubuntu 2>/dev/null || true) \
    && groupadd -g 1000 developer \
    && useradd -u 1000 -g developer -m -s /bin/bash developer \
    && echo "developer ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/developer

USER developer
WORKDIR /home/developer

ENV VCPKG_ROOT=/home/developer/vcpkg
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
ENV PATH="${VCPKG_ROOT}:${PATH}"

# Clone & bootstrap vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git "${VCPKG_ROOT}" \
    && "${VCPKG_ROOT}/bootstrap-vcpkg.sh" -useSystemBinaries -disableMetrics

# -----------------------------------------------------------------------------
# Devcontainer Stage (Target for Zed / Podman dev environment)
# -----------------------------------------------------------------------------
FROM base AS devcontainer
LABEL description="NCONPP - Devcontainer"

# Pre-cache vcpkg dependencies in the image
WORKDIR /home/developer/cache
COPY --chown=developer:developer vcpkg.json ./
RUN vcpkg install --x-feature=test --clean-after-build \
    && rm -rf /home/developer/cache

WORKDIR /workspaces/nconpp
ENTRYPOINT ["/bin/bash"]

# -----------------------------------------------------------------------------
# Builder Stage (Build project artifacts)
# -----------------------------------------------------------------------------
FROM base AS builder
LABEL description="NCONPP - Build container for CPP"

WORKDIR /home/developer/project
COPY --chown=developer:developer CMakePresets.json CMakeLists.txt vcpkg.json ./
COPY --chown=developer:developer cpp ./cpp

# -----------------------------------------------------------------------------
# Tester Stage (Run automated tests)
# -----------------------------------------------------------------------------
FROM builder AS tester
LABEL description="Test container - nconpp-tester"

RUN cmake --preset linux-debug \
    && cmake --build --preset linux-debug \
    && ctest --preset linux-gtests-debug --output-on-failure

ENTRYPOINT ["tail", "-f", "/dev/null"]
