#!/bin/sh

DIR=`dirname "${BASH_SOURCE[0]}"`

g++ $DIR/board.cpp -O2 -o $DIR/board
