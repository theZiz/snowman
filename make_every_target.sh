#!/bin/sh
# Change the folder to YOUR sparrow3d folder!
cd ../sparrow3d
FILES=./target-files/*
for f in $FILES
do
  cd ../snowman
  make clean
  make TARGET=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
  echo "Built for:"
  echo "$f" | cut -d/ -f3 | cut -d. -f1
done
make clean
make
