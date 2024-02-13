#!/bin/sh

apt -y update
apt -y upgrade
apt -y install git cmake gcc

git clone https://github.com/Tykky/cabor