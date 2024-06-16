FROM ubuntu:focal-20220113

ARG DEBIAN_FRONTEND=noninteractive

# Common
# hadolint ignore=DL3008
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    build-essential \
    curl \
    cmake \
    tar \
    ca-certificates \
    openssh-client \
    bash-completion \
    git \
    g++ \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# add a custom user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

# ARM compiler
# hadolint ignore=DL3008,DL3015
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    gcc-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    && rm -rf /var/lib/apt/lists/*

# Debugger, ST-Link flasher
# Current VSCode cortex-debug versions don't work with old openocd, so build the required version from sources.
ARG OPENOCD_VERSION=v0.12.0
# hadolint ignore=DL3008, DL3003
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    autoconf \
    automake \
    libtool \
    pkg-config \
    libusb-1.0-0-dev \
    libftdi1-dev \
    telnet \
    gdb-multiarch \
    stlink-tools \
    && rm -rf /var/lib/apt/lists/* \
    && git clone --depth 1 -b ${OPENOCD_VERSION} https://github.com/ntfreak/openocd.git \
    && cd openocd \
    && git submodule update --init --recursive \
    && ./bootstrap \
    && ./configure \
    && make \
    && make install \
    && rm -rf openocd

# hadolint ignore=DL3008
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    unzip \
    && rm -rf /var/lib/apt/lists/*

# Download and unzip clangd.
ARG CLANGD_VERSION=18.1.3
ENV PATH="${PATH}:/usr/local/clangd_${CLANGD_VERSION}/bin"
RUN curl -sLO "https://github.com/clangd/clangd/releases/download/${CLANGD_VERSION}/clangd-linux-${CLANGD_VERSION}.zip" \
    && unzip clangd-linux-${CLANGD_VERSION}.zip -d /usr/local/ \
    && rm clangd-linux-${CLANGD_VERSION}.zip

# Linters.
ARG PRECOMMIT_VERSION=3.5.0
ARG CLANG_FORMAT_VERSION=17.0.6
ARG CLANG_TIDY_VERSION=17.0.1
ARG SHFMT_VERSION=3.7.0.1
ARG CPPLINT_VERSION=1.6.1
ARG SHELLCHECK_VERSION=0.10.0.1
RUN pip install --no-cache-dir \
    pre-commit==${PRECOMMIT_VERSION} \
    clang-format==${CLANG_FORMAT_VERSION} \
    clang-tidy==${CLANG_TIDY_VERSION} \
    shfmt-py==${SHFMT_VERSION} \
    shellcheck-py==${SHELLCHECK_VERSION} \
    cpplint==${CPPLINT_VERSION}

# Linter: Hadolint (Docker)
ARG HADOLINT_VERSION=v2.8.0
RUN curl -sL "https://github.com/hadolint/hadolint/releases/download/${HADOLINT_VERSION}/hadolint-Linux-$(uname -m)" -o /usr/local/bin/hadolint  \
    && chmod +x /usr/local/bin/hadolint

# hadolint ignore=DL3008
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    unzip \
    gpg \
    && rm -rf /var/lib/apt/lists/*

USER $USERNAME

WORKDIR /data
