#!/bin/bash

apt-get update -y
apt-get upgrade -y

export LIBRARY_PATH=/usr/local/cuda/lib64:$LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
export CPATH=/usr/local/cuda/include:$CPATH

# apt install -y git

# apt install build-essential -y
# apt-get install software-properties-common -y
# add-apt-repository ppa:ubuntu-toolchain-r/test -y

# apt-get update -y
# apt install gcc-13 g++-13 -y
# update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13 --slave /usr/bin/g++ g++ /usr/bin/g++-13
# apt-get install gdb -y
