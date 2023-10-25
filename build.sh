#!/bin/sh

rm -rf build/
mkdir build
cd build
cmake ..
if [ $? -ne 0 ];then
    echo "Failed: cmake failed, exit"
    exit 1
fi
make
if [ $? -ne 0 ];then
    echo "Failed: make failed, exit"
    exit 1
fi

ctest
if [ $? -ne 0 ];then
    echo "Failed: ctest failed, exit"
    exit 1
fi


echo "SUCCESS: Build unizip successful."
