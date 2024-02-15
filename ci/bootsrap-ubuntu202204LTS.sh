#!/bin/sh

apt -y --force-yes update
apt -y --force-yes upgrade
apt -y --force-yes install git cmake gcc

git clone https://github.com/Tykky/cabor