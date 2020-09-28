#!/bin/bash

# example format:
# turnin cmsc201 HW3

if [[ $1 == "--help" || $1 == "-h" ]]
then
	echo "usage: turnin [-h] <classname> <assignment>"
	exit
fi

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
