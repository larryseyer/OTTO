#!/bin/bash

# Monitor INIConfig.h for truncation
FILE="/Users/larryseyer/AudioDevelopment/OTTO/Source/INIConfig.h"
LOG="/Users/larryseyer/AudioDevelopment/OTTO/truncation_log.txt"

echo "Starting INIConfig.h monitoring at $(date)" > "$LOG"
echo "Initial file size: $(wc -l < "$FILE") lines" >> "$LOG"

while true; do
    LINES=$(wc -l < "$FILE")
    if [ "$LINES" -lt 2500 ]; then
        echo "TRUNCATION DETECTED at $(date): File has $LINES lines" >> "$LOG"
        echo "Last few lines of truncated file:" >> "$LOG"
        tail -5 "$FILE" >> "$LOG"
        echo "---" >> "$LOG"
        
        # Check what processes are accessing the file
        echo "Processes accessing the file:" >> "$LOG"
        lsof "$FILE" 2>/dev/null >> "$LOG" || echo "No processes found" >> "$LOG"
        echo "---" >> "$LOG"
        
        # Restore from backup if available
        if [ -f "/Users/larryseyer/AudioDevelopment/OTTO/Source/INIConfig.h.backup" ]; then
            echo "Attempting to restore from backup..." >> "$LOG"
            cp "/Users/larryseyer/AudioDevelopment/OTTO/Source/INIConfig.h.backup" "$FILE"
            echo "Restored. New line count: $(wc -l < "$FILE")" >> "$LOG"
        fi
        echo "===" >> "$LOG"
    fi
    sleep 1
done