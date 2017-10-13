#!/bin/bash

cd `dirname $0`

if [ ! -e ../build ]; then
    ./reload.sh
fi

cd ../build
make
