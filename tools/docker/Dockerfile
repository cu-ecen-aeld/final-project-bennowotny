FROM ubuntu:jammy AS base

RUN apt update
RUN apt install -y software-properties-common
RUN add-apt-repository -y 'ppa:ubuntu-toolchain-r/test'
RUN apt update
RUN apt upgrade -y
RUN useradd -m devcontainer

FROM base AS devel

ARG projName

ENV DEBIAN_FRONTEND=noninteractive
RUN yes | unminimize
RUN apt install -y g++-13 git clangd sudo curl vim libssl-dev wget build-essential bash-completion man gnupg2
RUN apt install -y libc6 libgcc1 libgcc-s1 libgssapi-krb5-2 libicu70 liblttng-ust1 libssl3 libstdc++6 libunwind8 zlib1g
RUN apt install -y ninja-build
RUN apt install dotnet-sdk-6.0 -y
RUN apt autoremove -y
WORKDIR /home/devcontainer
RUN wget https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0.tar.gz
RUN tar -xzvf cmake-3.28.0.tar.gz
WORKDIR /home/devcontainer/cmake-3.28.0
RUN ./bootstrap
RUN make -j$(nproc)
RUN make install
WORKDIR /home/devcontainer
RUN rm -rf cmake-3.28.0

# Add yocto tools
RUN apt install -y gawk wget diffstat unzip texinfo gcc chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1 bear
RUN locale-gen en_US.UTF-8

RUN usermod -aG sudo devcontainer
RUN usermod -aG dialout devcontainer
RUN echo "devcontainer ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers

USER devcontainer
