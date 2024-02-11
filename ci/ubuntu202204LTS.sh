#!/bin/sh

set -e

echo "-- installing build dependencies"

apt -y update
apt -y upgrade
apt -y install cmake

echo "-- running build.sh"
bash build.sh

exit 0