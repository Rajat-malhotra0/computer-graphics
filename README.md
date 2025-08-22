git clone https://github.com/yourusername/dda-algorithm-wasm
git clone https://github.com/emscripten-core/emsdk.git
<div align="center">

# DDA Line Drawing Algorithm (WebAssembly + C)
High‑performance, memory‑safe, browser‑friendly implementation of the Digital Differential Analyzer line rasterization algorithm.

![Status](https://img.shields.io/badge/status-active-success)
![License](https://img.shields.io/badge/license-MIT-blue)
![WASM](https://img.shields.io/badge/target-WebAssembly-6527BE)

</div>

## 🔗 Live Demo

> Replace with your deployed URL:
**https://your-app-name.netlify.app**

---
## 📚 Table of Contents

1. [Overview](#overview)  
2. [Features](#features)  
3. [Architecture](#architecture)  
4. [Algorithm Deep Dive](#algorithm-deep-dive)  
5. [WebAssembly Interface](#webassembly-interface)  
6. [Memory Model & Data Transfer](#memory-model--data-transfer)  
7. [Build System & Flags](#build-system--flags)  
8. [Front‑End UI & UX](#front-end-ui--ux)  
9. [Validation & Limits](#validation--limits)  
10. [Performance Notes](#performance-notes)  
11. [Deployment (Netlify)](#deployment-netlify)  
12. [Local Development](#local-development)  
13. [Troubleshooting](#troubleshooting)  
14. [Security & Hardening](#security--hardening)  
15. [Extensibility Ideas](#extensibility-ideas)  
16. [Contributing](#contributing)  
17. [License](#license)  

---
## Overview

This project demonstrates how to implement the DDA (Digital Differential Analyzer) line rasterization algorithm in **C**, compile it to **WebAssembly** with **Emscripten**, and expose a safe, allocation‑controlled interface to JavaScript for browser visualization. 

The design focuses on:
- Deterministic & safe memory usage
- Transparent data transfer (flat float arrays)
- Simple deployability (static hosting)
- Educational clarity

---
## ✨ Features

| Category | Highlights |
|----------|------------|
| Rendering Logic | Pure C implementation of DDA with bounds & allocation guards |
| WebAssembly Bridge | Explicit exported functions; no dynamic JS glue modification needed |
| Data Transport | Flat `Float32Array` (x,y,x,y,...) for predictable mapping |
| UI | Responsive, no frameworks, accessible form + structured results table |
| Table Output | Step, Current, Next, Rounded coordinate view |
| Performance | O(n) with minimal branching; no heap reallocations during run |
| Safety | Input range validation, point count capping, guarded allocations |
| Deployment | Zero‑build static host (Netlify) with proper MIME & cache headers |
| Cache Busting | Timestamp query params for `dda.js` and `dda.wasm` |

---
## Architecture

```
Browser (UI) ──► JavaScript Loader ──► WebAssembly Module (C runtime)
                                                 │                      │
                                                 │ <── Float32Array ────┘ (x,y pairs)
```

### Layers
- **C Core (`main.c`)**: Implements DDA, slope calculation, memory test, and a marshaling function (`get_dda_points_array`).
- **WebAssembly Glue (`dda.js` generated)**: Produced by Emscripten; exposes exported functions on `Module`.
- **UI (`index.html`)**: Form inputs → validate → allocate WASM memory → invoke exported function → read heap → render table.
- **Build Script (`build.sh`)**: Stable compilation pipeline with explicit flags and reproducible output.
- **Deployment Config (`netlify.toml`)**: MIME types, caching, and security headers.

---
## Algorithm Deep Dive

The DDA algorithm incrementally interpolates floating‑point positions between endpoints:

### Pseudocode
```text
dx = x2 - x1
dy = y2 - y1
steps = max(|dx|, |dy|)
if steps == 0: emit (x1,y1); return
xi = x1; yi = y1
x_inc = dx / steps
y_inc = dy / steps
for i in 0..steps:
        output(xi, yi)
        xi += x_inc
        yi += y_inc
```

### Implementation Nuances
- Handles coincident points (returns exactly one point).
- Caps total points (`> 50,000` rejected in WASM layer; UI further constrains practical input).
- Uses float math, defers rounding to presentation layer.
- Vertical lines: slope returns large sentinel (±999999) to avoid division by zero.

---
## WebAssembly Interface

Exported Functions (see `build.sh` `-s EXPORTED_FUNCTIONS`):

| C Function | JS Symbol | Purpose |
|------------|-----------|---------|
| `get_dda_points_array` | `_get_dda_points_array` | Core marshaling: fills flat float buffer + writes point count |
| `calculate_slope` | `_calculate_slope` | Returns slope (sentinel for vertical) |
| `get_point_size` | `_get_point_size` | Size of `point` struct (for introspection) |
| `test_memory_allocation` | `_test_memory_allocation` | Simple allocation probe (diagnostics) |
| `malloc` / `free` | `_malloc`, `_free` | Manual memory management for buffers |

### Call Pattern (High Level)
1. JS allocates output buffer: `ptr = _malloc(bytes)`
2. JS allocates 4‑byte counter: `countPtr = _malloc(4)`
3. JS invokes `_get_dda_points_array(x1,y1,x2,y2, ptr, countPtr)`
4. JS reads `HEAP32[countPtr >> 2]` → number of points `n`
5. JS creates `Float32Array(HEAPF32.buffer, ptr, n*2)`
6. JS copies or iterates; then frees both allocations.

---
## Memory Model & Data Transfer

| Aspect | Detail |
|--------|--------|
| Layout | `[x0, y0, x1, y1, x2, y2, ...]` contiguous floats |
| Type | `float` (32‑bit IEEE 754) |
| Counter | Separate 32‑bit int (little‑endian) |
| Growth | `ALLOW_MEMORY_GROWTH=1` allows heap expansion if needed |
| Allocation Strategy | Small (<1000) uses `malloc`, large uses `calloc`, fallback attempts alternate path |
| Validation | Rejects oversized input & point counts before allocation |

### Safety Measures
- Guard: expected_points > 50,000 → abort early.
- Bounds: All writes gated by `if (index < total_points)`.
- Memory test path removed from production UI, but function still exported for diagnostics.

---
## Build System & Flags

Compilation via `build.sh`:
```bash
emcc main.c -o dda.js \
    -s EXPORTED_FUNCTIONS='["_get_dda_points_array","_calculate_slope","_get_point_size","_test_memory_allocation","_malloc","_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","HEAP8","HEAP32","HEAPF32"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=0 -s EXPORT_NAME=Module \
    -s SINGLE_FILE=0 -s INITIAL_MEMORY=2097152 -s MAXIMUM_MEMORY=33554432 \
    -s STACK_SIZE=131072 -s TOTAL_STACK=131072 \
    -s MALLOC=emmalloc -O2 -s ASSERTIONS=1 -s SAFE_HEAP=0
```

| Flag | Reason |
|------|--------|
| `ALLOW_MEMORY_GROWTH=1` | Resilience for large coordinate ranges |
| `MALLOC=emmalloc` | Lightweight allocator tuned for WASM |
| `ASSERTIONS=1` | Debug safety (can disable for final minifying) |
| `SAFE_HEAP=0` | Disabled for performance after stabilization |
| `INITIAL_MEMORY` | 2 MB starting heap (adjustable) |
| `MAXIMUM_MEMORY` | 32 MB cap prevents runaway growth |
| `STACK_SIZE` | Explicit stack to avoid overflow for recursion (not used here, precautionary) |

---
## Front-End UI & UX

| Section | Notes |
|---------|-------|
| Input Panel | Coordinate form (x₁,y₁,x₂,y₂) with range validation (±5000 UI; deeper WASM guard ±50000) |
| Output Panel | Line statistics (length, slope, total points) + results table |
| Layout | CSS Grid: 25% input / 75% output (responsive collapse on narrow screens) |
| Table Columns | Step | Current X/Y | Next X/Y | Rounded-Coordinates `(⌊x+0.5⌋, ⌊y+0.5⌋)` |
| Cache Busting | Script & WASM loaded with `?v=timestamp` query param |

### Why "Next" Columns?
These allow visualizing interpolation progression & incremental delta stability.

---
## Validation & Limits

| Layer | Rule |
|-------|------|
| UI | Input numeric; range [-5000, 5000]; derived total points <= 25,000 (user feedback) |
| WASM | Absolute coordinate clamp 50,000; computed point count clamp 50,000 |
| Memory | Early exit if allocation fails; fallback allocation strategies tried |

Customize by editing:
- UI limits: `index.html` validation logic.
- WASM limits: constants & guard logic in `get_dda_points_array` / `dda`.

---
## Performance Notes

| Consideration | Detail |
|---------------|--------|
| Complexity | O(N) points where N = `max(|dx|,|dy|) + 1` |
| Allocation | Single contiguous block per call (no reallocation) |
| Transfer | Zero‑copy view (`Float32Array`) into WASM heap before copying to JS objects |
| Rounding | Deferred to presentation layer to avoid extra passes |
| Memory Growth | Rare for typical teaching examples; budget still capped |

Potential micro‑optimizations (not yet required): inline slope calculate into JS, SIMD (future), batching multiple lines.

---
## Deployment (Netlify)

`netlify.toml` configures:
- Skipped build step (pre‑built artifacts committed)
- Correct WASM MIME (`application/wasm`)
- Long‑term caching for `.js` / `.wasm`, revalidation for `.html`
- Security headers (frame, sniffing, referrer policy)

Steps:
1. Commit `dist/` outputs.
2. Connect repo to Netlify.
3. Ensure **Publish directory** = `dist`.
4. Trigger deploy; verify Network tab shows proper MIME types.

---
## Local Development

```bash
# Clone
git clone <your-fork-url>
cd graphics-algo-solver

# Build (prompts optional local server)
./build.sh

# Or manual preview
cd dist
python3 -m http.server 8080
```

Environment Setup (Emscripten):
```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

---
## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| WASM 404 | Deployed without `dist/` | Rebuild & commit dist artifacts |
| Stale output | Browser cache | Hard refresh / open dev tools disable cache |
| 0 points returned | Input out of bounds / guard triggered | Verify coordinate ranges & limits |
| Memory allocation fail | Extremely large span | Reduce delta or raise MAX / memory flags |
| Vertical slope huge value | Sentinel design | Treat ±999999 as infinite slope |

Debugging Tips:
- Temporarily enable `ASSERTIONS=2` & `SAFE_HEAP=1` for deeper WASM safety.
- Use `Module.HEAPF32.subarray(ptr/4, ptr/4 + n*2)` to inspect raw values.

---
## Security & Hardening

| Area | Current | Future Ideas |
|------|---------|--------------|
| Memory Safety | Manual bounds checks | Compile with UBSan in native builds |
| Content Security | Basic headers | Add CSP & COOP/COEP for advanced isolation |
| C Attack Surface | Minimal API | Add fuzz harness for `get_dda_points_array` |
| Cache Busting | Timestamp query param | Hash‑based filenames in build pipeline |

---
## Extensibility Ideas

- Bresenham variant for integer‑only comparison
- Anti‑aliased (Xiaolin Wu) line renderer
- Multi‑segment polylines & batching API
- Off‑screen canvas visualization + pixel plot
- Benchmark panel (compare JS vs WASM DDA)
- WebWorker offloading for huge datasets
- Add unit tests via `embind` or headless harness

---
## Contributing

1. Fork repository
2. Create branch: `git checkout -b feature/your-idea`
3. Commit: `git commit -m "feat: add your idea"`
4. Push: `git push origin feature/your-idea`
5. Open Pull Request (describe rationale & perf impact)

Code Style Notes:
- Keep C logic side‑effect free except for output buffer writes.
- Avoid adding hidden globals—explicit parameters only.
- Document new exported functions in this README.

---
## License

MIT — see `LICENSE` (add if missing).

---
## Acknowledgments

- Emscripten project
- WASM ecosystem contributors
- Classic computer graphics literature on incremental line algorithms

---
<sub>Educational project demonstrating safe & efficient numeric pipelines from native code to the browser.</sub>

