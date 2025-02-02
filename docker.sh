#!/bin/bash

rm cabor_source.tar.gz
./pack_source.sh
./build.sh

docker build -t cabor_compiler .
