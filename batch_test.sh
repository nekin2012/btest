#!/bin/bash

NCPU=`nproc`
TIME=3

echo "batch test: $NCPU CPU on $TIME seconds for one"

echo "999:1"
echo "use pthread_spinlock"
i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 999 -s 1 -l 0 -q $i
done

echo "use mcs lock"

i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 98 -s 2 -l 1 -q $i
done


echo "99:1"
echo "use pthread_spinlock"
i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 99 -s 1 -l 0 -q $i
done

echo "use mcs lock"
i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 97 -s 3 -l 1 -q $i
done

echo "90:10"
echo "use pthread_spinlock"
i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 90 -s 10 -l 0 -q $i
done

echo "use mcs lock"
i=0
while [ $i -lt $NCPU ]; do
	let i=i+1
	timeout -s 2 $TIME ./amdahl -b -p 90 -s 10 -l 1 -q $i
done
