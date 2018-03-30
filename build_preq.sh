#!/bin/bash

set -e

apt-get update
apt-get install -y wget software-properties-common

apt-get update
apt-get install -y \
    g++ \
    automake \
    autoconf \
    autoconf-archive \
    libtool \
    libboost-all-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    make \
    zlib1g-dev \
    binutils-dev \
    libjemalloc-dev \
    libssl-dev \
    pkg-config \
    libunwind8-dev \
    libelf-dev \
    libdwarf-dev \
    libiberty-dev

# For fpm support
apt-get install -y git-core ruby-all-dev
gem install fpm --no-ri --no-rdoc

echo 'Build Configuration Complete'
