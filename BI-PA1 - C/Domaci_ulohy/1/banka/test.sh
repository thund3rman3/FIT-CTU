#!/bin/bash

for IN in ./*_in.txt 
do 
	./a.out < $IN > /tmp/output
	
	OUT=$(echo -n $IN | sed -e 's/_in\(.*\)$/_out\1/')
	
	if ! diff -q $OUT /tmp/output
	then
		echo "Chyba je v: $IN"
		diff $OUT /tmp/output
		exit 1
	fi
done
echo "OK YOU HAVE DONE IT"
