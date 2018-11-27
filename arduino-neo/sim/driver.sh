#! /bin/bash

if [ ! -d "./tests" ]; then
    echo "Error: folder ./tests does not exist."
    exit
fi

rm -rf output
mkdir output
files=$(ls ./tests | sed "s:^:tests/:")
./led_sim $files
mv tests/*.png output/
