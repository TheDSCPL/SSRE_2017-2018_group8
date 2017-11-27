#!/usr/bin/env bash

folder=Projeto/

letter=$(echo $2 | sed -r 's/([kKMG])?.*/\1/' | sed -r 's/k/K/')

a=$1
a=$(echo $a | sed -r 's/([0-9]+([.][0-9]+)?).*/\1/')

fname="data$a$letter"
fname="$folder$fname"

if [[ -r $fname && -w $fname ]]; then
    exit 0;
fi

case $letter in
    (k)
        #mult=$((1024));;
        letter='K';
        mult=1024;;
    (M)
        #mult=$((1024*1024));;
        mult=1048576;;
    (G)
        #mult=$((1024*1024*1024));;
        mult=1073741824;;
    (*)
        mult=1;
esac

#echo openssl rand -out $fname $(echo $a $mult | awk '{printf "%d",$1*$2}')
openssl rand -out $fname $(echo $a $mult | awk '{printf "%d",$1*$2}')