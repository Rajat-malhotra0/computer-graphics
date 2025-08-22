#!/bin/bash

# DDA Algorithm WebAssembly Build Script
# This script compiles the C code to WebAssembly using Emscripten

echo "🚀 Building DDA Algorithm for WebAssembly..."

# Check if emcc is available
if ! command -v emcc &> /dev/null; then
    echo "Error: Emscripten (emcc) not found. Please install Emscripten first."
    exit 1
fi

# Create dist directory for production build
# Ensure fresh dist directory
mkdir -p dist

# Optional: clean old build artifacts in dist (retain index.html until copied)
rm -f dist/dda.js dist/dda.wasm

# Compile C code to WebAssembly with production optimizations
echo "Compiling main.c to WebAssembly (output -> dist/)..."
emcc main.c -o dist/dda.js \
    -s EXPORTED_FUNCTIONS='["_get_dda_points_array","_calculate_slope","_get_point_size","_test_memory_allocation","_malloc","_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP8","HEAP32","HEAPF32"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=0 \
    -s EXPORT_NAME="Module" \
    -s SINGLE_FILE=0 \
    -s INITIAL_MEMORY=2097152 \
    -s MAXIMUM_MEMORY=33554432 \
    -s STACK_SIZE=131072 \
    -s TOTAL_STACK=131072 \
    -s MALLOC=emmalloc \
    -O2 \
    -s ASSERTIONS=1 \
    -s SAFE_HEAP=0

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ WebAssembly compilation successful!"
    
    # Copy HTML entrypoint to dist
    cp index.html dist/
    
    echo "✅ Files copied to dist/ directory"
    echo "📦 Production build ready for deployment!"
    echo ""
    echo "Generated files:"
    echo "  - dist/dda.js (JavaScript glue + WASM loader)"
    echo "  - dist/dda.wasm (WebAssembly binary)"
    echo "  - dist/index.html (Web interface)"
    echo ""
    
    # Show file sizes
    echo "📊 Build Output:"
    ls -lh dist/
    
    # Check if we're in a CI environment
    if [ -z "$CI" ]; then
        read -p "🌐 Start local preview server? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            echo "🌍 Starting server at http://localhost:8080"
            echo "Press Ctrl+C to stop the server"
            cd dist && python3 -m http.server 8080
        fi
    fi
else
    echo "❌ Compilation failed!"
    exit 1
fi
