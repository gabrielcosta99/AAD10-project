#! /bin/bash

while IFS= read -r line; do
  echo "$line" | cut -b 5- | md5sum
done < deti_coins_vault.txt
