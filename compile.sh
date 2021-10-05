#!/bin/sh

DIR=`dirname "${BASH_SOURCE[0]}"`

FLAGS='-std=c++11 -O2'

if [[ -z "${OUT_DIR}" ]]; then
	OUT_DIR=$DIR
fi

g++ *.cpp $FLAGS -o "$OUT_DIR/board"