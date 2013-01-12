#!/bin/sh
g++ -fPIC -shared  -I../include/ *.cpp -o libecho.so
