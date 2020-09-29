#!/bin/bash

if [ -z $1 ]
then
	echo which homework?
	exit
fi

if [[ "$1" == "-h" || "$1" == "--help" ]]
then
	echo "usage: mkhw <hw#>"
	echo "ex: mkhw hw5"
	exit
fi

if [[ "$1" =~ [A-Z] ]]
then
	echo hw names should be in lowercase
	exit
fi

prefix="$1_part"

for i in {1..5}
do
	file="$prefix$i.py"
	if [ -f $file ]
	then
		echo "skipping existing file $file"
	else
		echo "creating file $file"
		touch $file
	fi
done
echo done.
