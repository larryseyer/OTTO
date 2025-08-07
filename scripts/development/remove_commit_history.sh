#!/bin/bash

# Clone the repository
git clone "$1"
cd "$(basename "$1" .git)"

# Create a new orphan branch
git checkout --orphan new-main

# Add all files
git add .

# Commit changes
git commit -m "Initial commit"

# Delete the old main branch
git branch -D main

# Rename new branch to main
git branch -m main

# Force push to GitHub
git push -f origin main
