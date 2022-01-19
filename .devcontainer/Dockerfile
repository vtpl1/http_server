FROM ubuntu:20.04


RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends ca-certificates software-properties-common \
    git curl wget ninja-build build-essential gdb python3-pip python3-setuptools \
    python3-dev python3-venv pkg-config zip unzip tar subversion nasm

RUN apt-get update && apt-get -y install gpg-agent

RUN wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.20.5/cmake-3.20.5-linux-x86_64.sh \
    && sh cmake.sh --prefix=/usr/local/ --exclude-subdir && rm -rf cmake.sh

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add -

RUN add-apt-repository 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal-13 main'

# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install clang-13 --install-suggests

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install clang-13 clang-tools-13 \
    libclang-common-13-dev libclang-13-dev libclang1-13 \
    clang-format-13 clang-tidy-13 \
    lldb-13 lld-13

# RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

ENV SHELL /bin/bash

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

RUN groupmod --gid $USER_GID $USERNAME \
    && usermod --uid $USER_UID --gid $USER_GID $USERNAME \
    && chown -R $USER_UID:$USER_GID /home/$USERNAME