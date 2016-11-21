#!/bin/bash

echo "==========================================="
echo "press u (up) or d (down) to change price in"
echo "html/price.txt. Start price is 50."
echo "==========================================="

price=50
echo $price > html/price.txt
while true; do
read -rsn1 input
if [ "$input" = "u" ]; then
    ((price+=10))
fi
if [ "$input" = "d" ]; then
    ((price-=10))
fi
echo "price:" $price
echo $price > html/price.txt

