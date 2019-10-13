#!/bin/bash

basePath=`dirname "$pathToScript"`

cd ${basePath}/tmp
cmake .
make
cd ../bin
./highload