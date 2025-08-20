# Deployment Guide

## 🚀 Quick Deployment to Netlify

### Option 1: Drag & Drop (Fastest)

1. Run `./build.sh` locally
2. Go to [netlify.com](https://netlify.com)
3. Drag the `dist/` folder to the deployment area
4. Your site will be live instantly!

### Option 2: Git Integration (Recommended)

1. Push your code to GitHub:

    ```bash
    git add .
    git commit -m "Initial commit"
    git branch -M main
    git remote add origin https://github.com/yourusername/dda-algorithm-wasm.git
    git push -u origin main
    ```

2. Connect to Netlify:
    - Go to [netlify.com](https://netlify.com)
    - Click "New site from Git"
    - Choose GitHub and select your repository
    - Build settings are auto-configured via `netlify.toml`
    - Deploy!

### Option 3: Netlify CLI

```bash
# Install Netlify CLI globally
npm install -g netlify-cli

# Login to your Netlify account
netlify login

# Deploy the dist folder
netlify deploy --prod --dir=dist
```

## 📋 Pre-deployment Checklist

-   [ ] Update repository URL in `package.json`
-   [ ] Update homepage URL in `package.json`
-   [ ] Test build locally with `./build.sh`
-   [ ] Verify all files are in `dist/` folder
-   [ ] Test the built site locally
-   [ ] Commit and push all changes

## 🔧 Build Configuration

The build is configured via:

-   **`build.sh`**: Compiles C to WebAssembly
-   **`netlify.toml`**: Netlify build settings
-   **Headers**: Proper MIME types for WASM files
-   **Optimizations**: Production-ready compilation flags

## 🌍 Custom Domain (Optional)

After deployment:

1. Go to your Netlify site dashboard
2. Click "Domain settings"
3. Add your custom domain
4. Follow DNS configuration steps

## 🔍 Troubleshooting

**Build fails on Netlify:**

-   Check that `build.sh` has execute permissions
-   Verify Emscripten is available in build environment

**WASM not loading:**

-   Check browser console for MIME type errors
-   Ensure proper headers are configured in `netlify.toml`

**Site not working:**

-   Verify all files are in `dist/` directory
-   Check that `index.html` loads properly locally first

## 📊 Expected Build Time

-   Local build: ~5-10 seconds
-   Netlify build: ~30-60 seconds
-   File sizes: ~13KB total (WASM + JS + HTML)
