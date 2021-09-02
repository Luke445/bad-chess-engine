#!/bin/sh

DIR=`dirname "${BASH_SOURCE[0]}"`

g++ $DIR/board.cxx -O2 -o $DIR/board
