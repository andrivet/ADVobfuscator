#!/bin/bash

# IMPORTANT: You need to install Boost (www.boost.org) to be able to compile this
# with command such as ((Debian & similar): 
#
# sudo apt-get install libboost-all-dev

g++ -std=c++11 -O3 ADVobfuscator/main.cpp ADVobfuscator/DetectDebugger.cpp -o Sample
