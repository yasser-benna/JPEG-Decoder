#!/bin/bash
# filepath: /home/bloops/PROJETC/finales/team8/runner.sh

# Create output directory if it doesn't exist
# OUTPUT_DIR="./output"
# mkdir -p "$OUTPUT_DIR"

# Counter for processed files
count=0

# Find all jpg and jpeg files in the images directory
echo "Looking for JPEG files in ./images/"
find ./images/ -type f \( -name "*.jpg" -o -name "*.jpeg" \) | while read -r file; do
    # Extract filename without path and extension
    filename=$(basename "$file")
    base="${filename%.*}"
    
    echo "Processing: $filename"
    
    # Run jpeg2ppm on the file
    ./jpeg2ppm "$file"
    
    # Check if conversion was successful
    if [ $? -eq 0 ]; then
        echo "✓ Successfully converted $filename"
        count=$((count+1))
        eog "output1.ppm" # Open the converted file in eog
        read -p "Press Enter to continue..." # Wait for user input before proceeding
    else
        echo "✗ Failed to convert $filename"
    fi
    
    echo "------------------------------"
done

echo "Conversion complete! Processed $count files."