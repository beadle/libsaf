#!/bin/bash

cd `dirname $0`
./clean.sh

cd ..
mkdir -p build
cd build
cmake -Werror=dev ..
