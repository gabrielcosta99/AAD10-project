#!/bin/bash

# Check if a file is provided as an argument
if [ $# -eq 0 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

# Loop through each line of the file
while IFS= read -r line || [ -n "$line" ]; do
    # Extract the line without the first 4 characters
    modified_line="${line:4}"
    # echo -e "$line"
    # Calculate MD5 and store it
    md5=$(printf "%s\n" "$modified_line" | md5sum | awk '{print $1}')
    
    # Print the line followed by its MD5
    echo "$md5 - $modified_line"
done < "$1"

