# Base Image: Ubuntu 22.04 LTS
FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install Toolchain & QA Tools
# 1. build-essential: Installs GCC (Host compiler) and Make
# 2. cmake: The build system we chose
# 3. cppcheck/valgrind: Quality tools
# 4. gcc-arm-none-eabi: The CROSS-COMPILER for the STM32
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    cppcheck \
    valgrind \
    doxygen \
    graphviz \
    clang-format \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    openocd \
    usbutils \
    && rm -rf /var/lib/apt/lists/*

# Set Working Directory
WORKDIR /project

# Default Command
CMD ["bash"]