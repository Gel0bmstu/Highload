#!/bin/bash

basePath=`dirname "$pathToScript"`

if ! [[ -d ${basePath}/tmp ]]; then
    mkdir ${basePath}/tmp
fi

if ! [[ -d ${basePath}/bin ]]; then
    mkdir ${basePath}/bin
fi

cd ${basePath}/tmp
cmake .
make
cd ../bin
./highload