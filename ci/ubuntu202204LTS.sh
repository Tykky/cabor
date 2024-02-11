#!/bin/sh

set -e

apt -y update
apt -y upgrade
apt -y install cmake

exit 0