#!/bin/bash

sudo apt-get update -y
sudo apt-get upgrade -y

sudo apt install build-essential -y
sudo apt-get install software-properties-common -y
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y

sudo apt-get update -y
sudo apt install gcc-13 g++-13 -y
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13 --slave /usr/bin/g++ g++ /usr/bin/g++-13
sudo apt-get install gdb -y
