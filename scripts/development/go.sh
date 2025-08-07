#!/bin/bash

# Find all .h and .cpp files and process them
find . -type f \( -name "*.h" -o -name "*.cpp" \) | while read -r file; do
  # Print separator with filename and add CR/LF
  echo "============[$(basename "$file") START]=============" >> output.txt
  echo "" >> output.txt
  # Append file contents
  cat "$file" >> output.txt
  # Add newline for separation
  echo "" >> output.txt
  # Append separator line to the end of output
  # echo "------------[$(basename "$file") STOP]-------------" >> output.txt
  # echo "" >> output.txt
done
