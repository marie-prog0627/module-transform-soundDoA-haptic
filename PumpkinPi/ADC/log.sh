#! /bin/bash

date > log.txt

I=0

while [ 1 ]
do
./getCH1 >> log.txt
I=`expr $I + 1`

if [ $I -gt 120 ]
then
	exit
fi
sleep 60
done



