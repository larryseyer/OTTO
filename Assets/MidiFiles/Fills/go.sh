#!/bin/bash
# Use a more robust approach to handle files with special characters
for file in *.mid *.MID *.midi *.MIDI; do
  # Skip if the glob doesn't match any files
  [[ -f "$file" ]] || continue

  # Extract base name (without extension) and extension
  base="${file%.*}"
  ext="${file##*.}"

  # Create 8 copies with Fill 01 to Fill 16
  for i in {1..16}; do
    # Ensure two-digit number with leading zero
    printf -v num "%02d" $i
    cp "$file" "${base} Fill ${num}.${ext}"
  done

  # Delete the original file after making all copies
  rm "$file"
done
