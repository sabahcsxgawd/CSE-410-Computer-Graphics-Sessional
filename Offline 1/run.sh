#! /usr/bin/bash

clear
rm trial2
g++ trial2.cpp myVector.cpp -o trial2 -lGL -lGLU -lglut
./trial2