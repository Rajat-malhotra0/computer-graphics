# DDA Line Drawing Algorithm - WebAssembly

A professional implementation of the Digital Differential Analyzer (DDA) line drawing algorithm compiled to WebAssembly for high-performance execution in web browsers.

## 🚀 Live Demo

**[View Live Demo →](https://your-app-name.netlify.app)**

## ✨ Features

-   **High Performance**: WebAssembly implementation for optimal speed
-   **Interactive Interface**: Real-time coordinate input and visualization
-   **Professional Design**: Clean, responsive web interface
-   **Comprehensive Output**: Shows exact coordinates and rounded pixel values
-   **Mobile Friendly**: Responsive design works on all devices
-   **Zero Dependencies**: Pure WebAssembly with no external libraries

## 🛠️ Technology Stack

-   **Core Algorithm**: C language implementation
-   **Web Platform**: WebAssembly (WASM) + JavaScript
-   **Compilation**: Emscripten SDK
-   **Deployment**: Netlify
-   **Styling**: Modern CSS with responsive design

## 📊 Algorithm Details

The DDA (Digital Differential Analyzer) algorithm efficiently draws lines by:

1. **Calculating slope and increments**
2. **Determining major axis** (X or Y direction)
3. **Iteratively plotting points** along the line
4. **Handling all slope cases** (positive, negative, steep, shallow)

## 🏃‍♂️ Quick Start

### Prerequisites

-   Emscripten SDK installed
-   Python 3 (for local server)
-   Git

### Build & Run Locally

```bash
# Clone the repository
git clone https://github.com/yourusername/dda-algorithm-wasm
cd dda-algorithm-wasm

# Make build script executable
chmod +x build.sh

# Build the project
./build.sh

# The build script will offer to start a local server
# Or manually start server:
cd dist && python3 -m http.server 8080

# Open http://localhost:8080 in your browser
```

## 📦 Deployment

### Netlify (Automatic)

1. Fork this repository
2. Connect to Netlify
3. Deploy automatically with provided configuration

### Manual Deployment

```bash
# Build production files
./build.sh

# Upload the dist/ directory to your hosting provider
```

## 🏗️ Project Structure

```
├── main.c              # Core DDA algorithm implementation
├── index.html          # Web interface and visualization
├── build.sh           # Build script with optimizations
├── netlify.toml       # Netlify deployment configuration
├── package.json       # Project metadata and scripts
├── .gitignore         # Git ignore rules
└── dist/              # Production build output (generated)
    ├── index.html
    ├── dda.js         # JavaScript glue code
    └── dda.wasm       # WebAssembly binary
```

## 🔧 Build Configuration

### WebAssembly Optimizations

-   **-O3**: Maximum optimization level
-   **--closure**: JavaScript minification
-   **Memory Growth**: Dynamic memory allocation
-   **Exported Functions**: Only necessary functions exposed

### Security & Performance

-   Proper MIME types for WASM files
-   Security headers (XSS protection, content type validation)
-   Optimized caching strategies
-   Gzipped assets via Netlify CDN

## 💻 Usage

1. **Enter Coordinates**: Input start (x1, y1) and end (x2, y2) points
2. **Calculate**: Click "Calculate DDA" to compute line points
3. **View Results**: See the complete table of coordinates
4. **Analysis**: Review line statistics (length, slope, total points)

### Example Input

-   Start Point: (0, 0)
-   End Point: (10, 5)
-   Result: 11 points plotted along the line

## 🧮 Algorithm Output

The interface displays:

-   **Step number** for each point
-   **Exact coordinates** (floating-point precision)
-   **Rounded coordinates** (pixel positions)
-   **Line statistics** (length, slope, point count)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📋 Development Scripts

```bash
# Build for production
npm run build

# Start development server
npm run dev

# Clean build artifacts
npm run clean
```

## 🐛 Troubleshooting

### Common Issues

**Build fails**: Ensure Emscripten is installed and in PATH

```bash
# Install Emscripten
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

**WASM not loading**: Check that files are served over HTTP (not file://)

**Memory errors**: Ensure proper cleanup of allocated memory

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

-   Emscripten team for the excellent WebAssembly toolchain
-   Netlify for seamless deployment platform
-   Computer graphics community for DDA algorithm development

---

**Made with ❤️ for computer graphics education and WebAssembly demonstration**
