#!/bin/bash

# Download Haar Cascade models for facial feature detection

echo "Downloading Haar Cascade models for facial feature detection..."

BASE_URL="https://raw.githubusercontent.com/opencv/opencv/4.x/data/haarcascades"

# Face detection
FACE_FILE="haarcascade_frontalface_alt2.xml"
FACE_URL="$BASE_URL/$FACE_FILE"

# Eye detection
EYE_FILE="haarcascade_eye.xml"
EYE_URL="$BASE_URL/$EYE_FILE"

# Nose detection
NOSE_FILE="haarcascade_mcs_nose.xml"
NOSE_URL="$BASE_URL/$NOSE_FILE"

# Mouth detection
MOUTH_FILE="haarcascade_mcs_mouth.xml"
MOUTH_URL="$BASE_URL/$MOUTH_FILE"

# Download Face Cascade
if [ ! -f "$FACE_FILE" ]; then
    echo "Downloading Face Cascade..."
    curl -L -o "$FACE_FILE" "$FACE_URL"
    if [ $? -eq 0 ]; then
        echo "✓ Face Cascade downloaded: $FACE_FILE"
    else
        echo "✗ Failed to download Face Cascade"
        exit 1
    fi
else
    echo "✓ Face Cascade already exists: $FACE_FILE"
fi

# Download Eye Cascade
if [ ! -f "$EYE_FILE" ]; then
    echo "Downloading Eye Cascade..."
    curl -L -o "$EYE_FILE" "$EYE_URL"
    if [ $? -eq 0 ]; then
        echo "✓ Eye Cascade downloaded: $EYE_FILE"
    else
        echo "✗ Failed to download Eye Cascade"
        exit 1
    fi
else
    echo "✓ Eye Cascade already exists: $EYE_FILE"
fi

# Download Nose Cascade
if [ ! -f "$NOSE_FILE" ]; then
    echo "Downloading Nose Cascade..."
    curl -L -o "$NOSE_FILE" "$NOSE_URL"
    if [ $? -eq 0 ]; then
        echo "✓ Nose Cascade downloaded: $NOSE_FILE"
    else
        echo "✗ Failed to download Nose Cascade"
        exit 1
    fi
else
    echo "✓ Nose Cascade already exists: $NOSE_FILE"
fi

# Download Mouth Cascade
if [ ! -f "$MOUTH_FILE" ]; then
    echo "Downloading Mouth Cascade..."
    curl -L -o "$MOUTH_FILE" "$MOUTH_URL"
    if [ $? -eq 0 ]; then
        echo "✓ Mouth Cascade downloaded: $MOUTH_FILE"
    else
        echo "✗ Failed to download Mouth Cascade"
        exit 1
    fi
else
    echo "✓ Mouth Cascade already exists: $MOUTH_FILE"
fi

echo ""
echo "All models downloaded successfully!"
echo "Models:"
echo "  - $FACE_FILE (face detection)"
echo "  - $EYE_FILE (eye detection)"
echo "  - $NOSE_FILE (nose detection)"
echo "  - $MOUTH_FILE (mouth detection)"
