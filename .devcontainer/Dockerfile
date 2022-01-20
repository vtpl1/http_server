FROM ubuntu:20.04


RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends ca-certificates software-properties-common \
    git curl wget ninja-build build-essential gdb python3-pip python3-setuptools \
    python3-dev python3-venv pkg-config zip unzip tar subversion nasm

RUN wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.20.5/cmake-3.20.5-linux-x86_64.sh \
    && sh cmake.sh --prefix=/usr/local/ --exclude-subdir && rm -rf cmake.sh

RUN apt-get update && apt-get -y install gpg-agent

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add -

RUN add-apt-repository 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal-13 main'

# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install clang-13 --install-suggests
ARG LLVM_VERSION=13

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install clang-$LLVM_VERSION lldb-$LLVM_VERSION lld-$LLVM_VERSION clangd-$LLVM_VERSION \
    clang-tidy-$LLVM_VERSION clang-format-$LLVM_VERSION clang-tools-$LLVM_VERSION \
    llvm-$LLVM_VERSION-dev lld-$LLVM_VERSION lldb-$LLVM_VERSION llvm-$LLVM_VERSION-tools \
    libomp-$LLVM_VERSION-dev libc++-$LLVM_VERSION-dev libc++abi-$LLVM_VERSION-dev \
    libclang-common-$LLVM_VERSION-dev libclang-$LLVM_VERSION-dev \
    libclang-cpp$LLVM_VERSION-dev libunwind-$LLVM_VERSION-dev

# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install clang-13 clang-tools-13 \
#     libclang-common-13-dev libclang-13-dev libclang1-13 \
#     clang-format-13 clang-tidy-13 \
#     lldb-13 lld-13


# ARG all="all"
# RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh) $(all)"
# COPY ./update-alternatives-clang.sh ./update-alternatives-clang.sh
ARG version=13
ARG priority=100

RUN update-alternatives \
        --install /usr/bin/llvm-config       llvm-config      /usr/bin/llvm-config-${version} ${priority} \
        --slave   /usr/bin/llvm-ar           llvm-ar          /usr/bin/llvm-ar-${version} \
        --slave   /usr/bin/llvm-as           llvm-as          /usr/bin/llvm-as-${version} \
        --slave   /usr/bin/llvm-bcanalyzer   llvm-bcanalyzer  /usr/bin/llvm-bcanalyzer-${version} \
        --slave   /usr/bin/llvm-cov          llvm-cov         /usr/bin/llvm-cov-${version} \
        --slave   /usr/bin/llvm-diff         llvm-diff        /usr/bin/llvm-diff-${version} \
        --slave   /usr/bin/llvm-dis          llvm-dis         /usr/bin/llvm-dis-${version} \
        --slave   /usr/bin/llvm-dwarfdump    llvm-dwarfdump   /usr/bin/llvm-dwarfdump-${version} \
        --slave   /usr/bin/llvm-extract      llvm-extract     /usr/bin/llvm-extract-${version} \
        --slave   /usr/bin/llvm-link         llvm-link        /usr/bin/llvm-link-${version} \
        --slave   /usr/bin/llvm-mc           llvm-mc          /usr/bin/llvm-mc-${version} \
        --slave   /usr/bin/llvm-mcmarkup     llvm-mcmarkup    /usr/bin/llvm-mcmarkup-${version} \
        --slave   /usr/bin/llvm-nm           llvm-nm          /usr/bin/llvm-nm-${version} \
        --slave   /usr/bin/llvm-objdump      llvm-objdump     /usr/bin/llvm-objdump-${version} \
        --slave   /usr/bin/llvm-ranlib       llvm-ranlib      /usr/bin/llvm-ranlib-${version} \
        --slave   /usr/bin/llvm-readobj      llvm-readobj     /usr/bin/llvm-readobj-${version} \
        --slave   /usr/bin/llvm-rtdyld       llvm-rtdyld      /usr/bin/llvm-rtdyld-${version} \
        --slave   /usr/bin/llvm-size         llvm-size        /usr/bin/llvm-size-${version} \
        --slave   /usr/bin/llvm-stress       llvm-stress      /usr/bin/llvm-stress-${version} \
        --slave   /usr/bin/llvm-symbolizer   llvm-symbolizer  /usr/bin/llvm-symbolizer-${version} \
        --slave   /usr/bin/llvm-tblgen       llvm-tblgen      /usr/bin/llvm-tblgen-${version}

RUN update-alternatives \
        --install /usr/bin/clang                 clang                 /usr/bin/clang-${version} ${priority} \
        --slave   /usr/bin/clang++               clang++               /usr/bin/clang++-${version}  \
        --slave   /usr/bin/asan_symbolize        asan_symbolize        /usr/bin/asan_symbolize-${version} \
        --slave   /usr/bin/c-index-test          c-index-test          /usr/bin/c-index-test-${version} \
        --slave   /usr/bin/clang-check           clang-check           /usr/bin/clang-check-${version} \
        --slave   /usr/bin/clang-cl              clang-cl              /usr/bin/clang-cl-${version} \
        --slave   /usr/bin/clang-cpp             clang-cpp             /usr/bin/clang-cpp-${version} \
        --slave   /usr/bin/clang-format          clang-format          /usr/bin/clang-format-${version} \
        --slave   /usr/bin/clang-format-diff     clang-format-diff     /usr/bin/clang-format-diff-${version} \
        --slave   /usr/bin/clang-import-test     clang-import-test     /usr/bin/clang-import-test-${version} \
        --slave   /usr/bin/clang-include-fixer   clang-include-fixer   /usr/bin/clang-include-fixer-${version} \
        --slave   /usr/bin/clang-offload-bundler clang-offload-bundler /usr/bin/clang-offload-bundler-${version} \
        --slave   /usr/bin/clang-query           clang-query           /usr/bin/clang-query-${version} \
        --slave   /usr/bin/clang-rename          clang-rename          /usr/bin/clang-rename-${version} \
        --slave   /usr/bin/clang-reorder-fields  clang-reorder-fields  /usr/bin/clang-reorder-fields-${version} \
        --slave   /usr/bin/clang-tidy            clang-tidy            /usr/bin/clang-tidy-${version} \
        --slave   /usr/bin/lldb                  lldb                  /usr/bin/lldb-${version} \
        --slave   /usr/bin/lldb-server           lldb-server           /usr/bin/lldb-server-${version}

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