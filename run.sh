#!/bin/bash

rm -f raw/*.png
mkdir raw
gcc -Wall -o extract extract.c
./extract < cursor.bdf
mogrify -format png raw/*.pam

for F in raw/*.png
do
	B=$(basename $F .png)
	echo 32 16 16 $F | xcursorgen - cursors/$B
done

