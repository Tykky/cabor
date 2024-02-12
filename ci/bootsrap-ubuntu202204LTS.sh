#!/bin/sh

set -e

git clone https://github.com/Tykky/cabor
cd cabor

apt -y update
apt -y upgrade
apt -y install git cmake gcc

exit 0