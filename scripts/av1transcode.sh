#!/bin/bash

INPUT_DIR = "$PWD"
OUTPUT_DIR = "$PWD"

for file in *$INPUT_DIR"/*.{mp4,mkv,avi}; do 
    filename = $(basename "$file")
    echo "Transcoding $filename to AV1..."
    ffmpeg -i "$file" -c:v libaom-av1 -crf 19 -b:v 0 -c:a copy "$OUTPUT_DIR/${filename%.*}.webm"
done 

echo "Transcoding done."