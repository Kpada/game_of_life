FROM ubuntu:focal-20220113

ARG DEBIAN_FRONTEND=noninteractive

# Common
# hadolint ignore=DL3008
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    build-essential \
    curl \
    cmake \
    ca-certificates \
    openssh-client \
    bash-completion \
    git \
    gnupg2 \
    g++ \
    python3 \
    python3-pip \
    nano \
    unzip \
    tar \
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
    && make "-j$(nproc)" \
    && make install \
    && rm -rf openocd

# Download and unzip clangd.
ARG CLANGD_VERSION=18.1.3
ENV PATH="${PATH}:/usr/local/clangd_${CLANGD_VERSION}/bin"
RUN curl -sLO "https://github.com/clangd/clangd/releases/download/${CLANGD_VERSION}/clangd-linux-${CLANGD_VERSION}.zip" \
    && unzip clangd-linux-${CLANGD_VERSION}.zip -d /usr/local/ \
    && rm clangd-linux-${CLANGD_VERSION}.zip

# Linter: Hadolint (Docker)
ARG HADOLINT_VERSION=v2.12.0
RUN curl -sL "https://github.com/hadolint/hadolint/releases/download/${HADOLINT_VERSION}/hadolint-Linux-x86_64" -o /usr/local/bin/hadolint  \
    && chmod +x /usr/local/bin/hadolint

# Install Python packages
COPY requirements.txt /tmp/requirements.txt
RUN pip install --no-cache-dir -r /tmp/requirements.txt && rm /tmp/requirements.txt

USER $USERNAME
WORKDIR /data
