#!/bin/bash

# Find all .h files in current directory and subdirectories
find . -type f -name "*.h" | while read -r file; do
  # Print separator with filename and add CR/LF
  echo "============[$(basename "$file") START]=============" >> output.txt
  echo "" >> output.txt
  # Append file contents
  cat "$file" >> output.txt
  # Add newline for separation
  echo "" >> output.txt
done
