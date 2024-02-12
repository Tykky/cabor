#!/bin/sh

set -e

apt -y update
apt -y upgrade
apt -y install git cmake gcc

exit 0