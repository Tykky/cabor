#!/bin/sh

echo "-- installing dependencies"

apt -y update
apt -y upgrade
apt -y install cmake

echo "-- running build.sh"
bash build.sh