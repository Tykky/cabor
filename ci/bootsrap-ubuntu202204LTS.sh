#!/bin/sh

apt -y --force-yes update
apt -y --force-yes upgrade
apt -y --force-yes install git cmake gcc clang

git clone https://github.com/Tykky/cabor