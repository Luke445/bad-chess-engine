#!/bin/sh

DIR=`dirname "${BASH_SOURCE[0]}"`

echo $OUT_DIR

if [[ -z "${OUT_DIR}" ]]; then
	g++ $DIR/board.cpp -O2 -o "$DIR/board"
else
	g++ $DIR/board.cpp -O2 -o "$OUT_DIR/board"
fi
