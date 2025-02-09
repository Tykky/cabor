#!/bin/bash

rm cabor_source.tar.gz
./pack_source.sh
./build.sh

python test-gadget.py submit
