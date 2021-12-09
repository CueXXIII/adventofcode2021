#!/bin/bash

set -e

read -p "Day to prepare: " day
read -p "Headline of puzzle: " h1

puzzle="$(printf %02d "$day") - $h1"
if test -d "$puzzle"; then
	echo "Puzzle \"$h1\" already created."
	exit 1
fi

read -p "Program name: " obj
read -p "Input name: " input

mkdir "$puzzle"
cd "$puzzle"

<../Makefile.template perl -pe "s/OBJS=/OBJS=$obj/" >Makefile
<../template.cc perl -pe "s/<example.txt>/<$input.txt>/" >"$obj.cc"

git add Makefile "$obj.cc"
git commit -m "day $day skeleton"

touch example.txt
touch "$input.txt"
