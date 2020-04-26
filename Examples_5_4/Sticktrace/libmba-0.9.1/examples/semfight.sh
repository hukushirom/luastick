#!/bin/sh

# run two (or more) of these at the same time

for i in `seq 1000`
do
	SEMID=`./semc /tmp/foo -o 1 1 777 1 1 2> /dev/null`
	if [ $? == 0 ]; then
		./semc $SEMID 0 -r
	fi
done
