#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage : $0 <File>"
    exit 1
fi

file=$1

temp="temp.txt"
temp2="temp2.txt"
awk '{print $2 " " $3 " " $6}' "$file" > "$temp"
cat "$temp" > "$temp2"
sort "$temp" | uniq > "$file"

rm "$temp" "$temp2"


echo "Done triming"
