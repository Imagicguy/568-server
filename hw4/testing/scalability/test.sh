#! /bin/bash

HOST=vcm-7703
NUM=2

./client1 $HOST &
for ((i = 0; i < $NUM; i++))
do
	./client2 $HOST &
	./client3 $HOST &
done

#wait
