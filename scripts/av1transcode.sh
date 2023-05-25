#!/bin/bash

# INPUT_DIR = "$PWD"
# OUTPUT_DIR = "$PWD"

for file in *.avi; do 
    echo "Transcoding $file to AV1..."
    ffmpeg -i "$file" -c:v libaom-av1 -crf 19 -b:v 0 -c:a copy "${file%.*}_av1.mp4"
done 

echo "Transcoding done."