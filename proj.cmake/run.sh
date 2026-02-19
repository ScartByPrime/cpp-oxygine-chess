#!/usr/bin/env sh

mkdir build
cd build

#generate cmake project in the "build" folder
cmake ..

#build it
make

cd ../../

cp -r data/* proj.cmake/build/

cd proj.cmake/build

#run executable
./Chess
