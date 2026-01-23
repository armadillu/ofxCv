#!/bin/bash

# Neural Style Transfer Model Downloader
# Downloads fast-neural-style models (Torch format)

set -e

MODEL_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$MODEL_DIR"

echo "========================================="
echo "Neural Style Transfer Model Downloader"
echo "========================================="
echo ""

# Base URL for fast-neural-style models
BASE_URL="https://cs.stanford.edu/people/jcjohns/fast-neural-style/models"

# Available models
declare -a MODELS=(
    "eccv16/the_wave.t7"
    "eccv16/la_muse.t7"
    "eccv16/composition_vii.t7"
    "eccv16/starry_night.t7"
    "instance_norm/candy.t7"
    "instance_norm/mosaic.t7"
    "instance_norm/feathers.t7"
    "instance_norm/the_scream.t7"
    "instance_norm/udnie.t7"
    "instance_norm/la_muse.t7"
)

echo "This will download neural style transfer models."
echo "Total download size: ~70MB"
echo ""
echo "Available models:"
for i in "${!MODELS[@]}"; do
    echo "  [$i] ${MODELS[$i]}"
done
echo ""

# Function to download a model
download_model() {
    local model_path=$1
    local model_name=$(basename "$model_path")
    local url="${BASE_URL}/${model_path}"

    if [ -f "$model_name" ]; then
        echo "✓ $model_name already exists, skipping..."
        return
    fi

    echo "Downloading $model_name..."
    if command -v curl &> /dev/null; then
        curl -L -o "$model_name" "$url" || {
            echo "✗ Failed to download $model_name"
            return 1
        }
    elif command -v wget &> /dev/null; then
        wget -O "$model_name" "$url" || {
            echo "✗ Failed to download $model_name"
            return 1
        }
    else
        echo "✗ Error: Neither curl nor wget found"
        exit 1
    fi

    echo "✓ Downloaded $model_name"
}

# Download default models (smaller set for faster setup)
echo "Downloading default models..."
echo ""

download_model "instance_norm/mosaic.t7"
download_model "instance_norm/candy.t7"
download_model "instance_norm/udnie.t7"
download_model "instance_norm/la_muse.t7"

# Rename to simpler names
[ -f "la_muse.t7" ] && mv la_muse.t7 rain_princess.t7

echo ""
echo "========================================="
echo "Download complete!"
echo "========================================="
echo ""
echo "Downloaded models:"
ls -lh *.t7 2>/dev/null || echo "No models found"
echo ""
echo "To download additional models, run:"
echo "  curl -L -o <model_name>.t7 ${BASE_URL}/<model_path>"
echo ""
echo "Model usage in the app:"
echo "  - Press SPACE to cycle through styles"
echo "  - Press 'r' to reload the current model"
echo ""
