#!/bin/bash

# Find all .h and .cpp files in current directory and subdirectories
files=$(find . -type f \( -name "*.h" -o -name "*.cpp" \))

# Check if any files found
if [ -z "$files" ]; then
    echo "No .h or .cpp files found"
    exit 1
fi

# Process each file
for file in $files; do
    # Create temporary file
    temp_file=$(mktemp)

    # Remove comments while preserving strings and source code
    awk '
    BEGIN { in_multiline = 0; in_string = 0 }
    {
        line = $0
        output = ""
        i = 1
        while (i <= length(line)) {
            # Handle string literals
            if (!in_multiline && !in_string && (substr(line, i, 1) == "\"" || substr(line, i, 1) == "'\''")) {
                in_string = substr(line, i, 1)
                output = output in_string
                i++
                while (i <= length(line)) {
                    if (substr(line, i, 1) == in_string && substr(line, i-1, 1) != "\\") {
                        output = output in_string
                        in_string = 0
                        i++
                        break
                    }
                    output = output substr(line, i, 1)
                    i++
                }
                continue
            }

            # Handle multiline comments
            if (!in_string && substr(line, i, 2) == "/*") {
                in_multiline = 1
                i += 2
                continue
            }
            if (in_multiline && substr(line, i, 2) == "*/") {
                in_multiline = 0
                i += 2
                continue
            }

            # Handle single-line comments
            if (!in_multiline && !in_string && substr(line, i, 2) == "//") {
                break
            }

            # Copy non-comment characters
            if (!in_multiline) {
                output = output substr(line, i, 1)
            }
            i++
        }
        # Print non-empty lines
        if (output ~ /[^ \t\n]/) {
            print output
        }
    }' "$file" > "$temp_file"

    # Replace original file
    mv "$temp_file" "$file"
    echo "Processed $file"
done
