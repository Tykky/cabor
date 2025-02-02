#!/bin/bash

git pull
./build.sh
./cabor.sh --test --server
