#!/bin/bash

while true; do
read -rsn1 input
if [ "$input" = "r" ]; then
    echo "red"
    echo "red" > html/LED_Color.txt
fi
if [ "$input" = "g" ]; then
    echo "green"
    echo "green" > html/LED_Color.txt
fi
done

