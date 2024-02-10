#!/bin/sh

set -e

if [-f build/release/bin/cabor]; then
    ./build/release/bin/cabor $@
else
    echo "Executable not found in build/release/bin/cabor"
fi

exit 0