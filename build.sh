#!/bin/bash
set -e

# Simplified build & deploy script
# 1. Builds WebAssembly artifacts into dist/
# 2. Copies index.html
# 3. Commits & pushes dist/ (triggers Netlify deploy)

echo "🚀 Build & Deploy: DDA WebAssembly"

if ! command -v emcc >/dev/null 2>&1; then
    echo "❌ emcc not found. Load emsdk environment first." >&2
    exit 1
fi

BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo main)
COMMIT_MSG=${COMMIT_MSG:-"build: update dist (auto)"}

echo "🔧 Preparing dist/"
mkdir -p dist
rm -f dist/dda.js dist/dda.wasm

echo "🧪 Compiling (emcc -> dist/dda.js, dist/dda.wasm)"
emcc main.c -o dist/dda.js \
    -s EXPORTED_FUNCTIONS='["_get_dda_points_array","_calculate_slope","_get_point_size","_test_memory_allocation","_malloc","_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP8","HEAP32","HEAPF32"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=0 \
    -s EXPORT_NAME=Module \
    -s SINGLE_FILE=0 \
    -s INITIAL_MEMORY=2097152 \
    -s MAXIMUM_MEMORY=33554432 \
    -s STACK_SIZE=131072 \
    -s TOTAL_STACK=131072 \
    -s MALLOC=emmalloc \
    -O2 \
    -s ASSERTIONS=1 \
    -s SAFE_HEAP=0

echo "📄 Copying index.html"
cp index.html dist/

echo "📦 Build artifacts:"
ls -lh dist/

# Auto add a .nojekyll (optional) to avoid GitHub Pages processing if reused
printf "# Prevent GitHub Pages Jekyll processing (harmless for Netlify)\n" > dist/.nojekyll 2>/dev/null || true

echo "📝 Staging dist/ for commit"
git add dist/

if git diff --cached --quiet; then
    echo "ℹ️  No changes in dist/ to commit. Skipping push."
    exit 0
fi

echo "🧷 Committing: $COMMIT_MSG"
git commit -m "$COMMIT_MSG" --no-verify || true

if git config remote.origin.url >/dev/null 2>&1; then
    echo "🚀 Pushing to origin/$BRANCH"
    git push origin "$BRANCH"
    echo "✅ Push complete. Netlify will deploy if configured."
else
    echo "⚠️  No git remote 'origin' found. Skipping push."
fi

echo "🎉 Done."
