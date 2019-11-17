#!/bin/bash

make clean && make 
for i in `seq 1 20` 
do 
    sleep 1
    echo -e "\nWe are on run $i"
    ./salesman 5000 
done
