#!/bin/bash

set -e

apt-get update
apt-get install --yes --force-yes wget software-properties-common

add-apt-repository 'deb http://apt.cedexis.com/ubuntu/ trusty-thirdparty main'

wget --quiet -O - http://apt.cedexis.com/apt.cedexis.com.key | apt-key add -

# for g++ 4.9
add-apt-repository -y ppa:ubuntu-toolchain-r/test

apt-get update
apt-get install -y \
    libtool \
    libboost-all-dev \
    libevent-dev \
    libnuma-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    g++ \
    make \
    flex \
    bison \
    gperf \
    unzip \
    autoconf \
    autoconf-archive \
    zlib1g-dev \
    libcap-dev \
    libkrb5-dev \
    binutils-dev \
    libjemalloc-dev \
    libssl-dev \
    libsasl2-dev \
    libiberty-dev \
    g++-4.9 \


# For fpm support
apt-get install -y git-core ruby-all-dev
gem install fpm --no-ri --no-rdoc

update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100
update-alternatives --set g++ /usr/bin/g++-4.9

echo 'Build Configuration Complete'
