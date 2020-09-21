#!/bin/bash

# example format:
# turning cmsc201 HW3

if [ -z $1 ]
then
	echo error: no class name supplied
	exit
fi

if [ -z $2 ]
then
	echo error: no assignment specified
	exit
fi

~/.autocomment/autocomment $(pwd)

submit $1 $2 *
