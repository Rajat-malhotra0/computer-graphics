var Module = {
    locateFile: function (path) {
        // Add cache-busting to WASM file
        if (path.endsWith(".wasm")) {
            return path + "?v=" + Date.now();
        }
        return path;
    },
    onRuntimeInitialized: function () {
        // Check if we have the new functions
        if (
            Module._get_dda_points_array &&
            typeof Module._get_dda_points_array === "function"
        ) {
            updateStatus(
                "ready",
                "✅ WebAssembly module loaded successfully!"
            );
        } else {
            updateStatus(
                "error",
                "❌ Old cached module detected. Try hard refresh (Ctrl+F5)"
            );
        }
    },
    onAbort: function (what) {
        updateStatus("error", `WebAssembly error: ${what}`);
    },
    onOutOfMemory: function () {
        updateStatus("error", "WebAssembly ran out of memory");
    },
};

// Add cache-busting parameter to force fresh load
const cacheBuster = Date.now();
const script = document.createElement("script");
script.src = `dda.js?v=${cacheBuster}`;
script.onload = function () {
    console.log("WebAssembly script loaded with cache buster");
};
script.onerror = function () {
    console.error(
        "Failed to load WebAssembly script, falling back to cached version"
    );
    // Fallback to regular script tag
    const fallbackScript = document.createElement("script");
    fallbackScript.src = "dda.js";
    document.head.appendChild(fallbackScript);
};
document.head.appendChild(script);

function updateStatus(type, message) {
    const indicator = document.getElementById("statusIndicator");
    indicator.className = `status-indicator status-${type}`;
    indicator.textContent = message;
}

function showLoading(show) {
    const spinner = document.getElementById("loadingSpinner");
    spinner.style.display = show ? "block" : "none";
}

function validateInput(inputId, value) {
    const input = document.getElementById(inputId);
    const errorElement = document.getElementById(
        inputId + "-error"
    );
    const MAX_VALUE = 5000;
    const MIN_VALUE = -5000;

    if (value < MIN_VALUE || value > MAX_VALUE || isNaN(value)) {
        input.classList.add("error");
        errorElement.classList.add("show");
        return false;
    } else {
        input.classList.remove("error");
        errorElement.classList.remove("show");
        return true;
    }
}

function validateAllInputs() {
    const x1 = parseInt(document.getElementById("x1").value);
    const y1 = parseInt(document.getElementById("y1").value);
    const x2 = parseInt(document.getElementById("x2").value);
    const y2 = parseInt(document.getElementById("y2").value);

    const isX1Valid = validateInput("x1", x1);
    const isY1Valid = validateInput("y1", y1);
    const isX2Valid = validateInput("x2", x2);
    const isY2Valid = validateInput("y2", y2);

    // Calculate expected points
    const dx = Math.abs(x2 - x1);
    const dy = Math.abs(y2 - y1);
    const totalPoints = Math.max(dx, dy) + 1;

    if (totalPoints > 25000) {
        ["x1", "y1", "x2", "y2"].forEach((id) => {
            const input = document.getElementById(id);
            const errorElement = document.getElementById(
                id + "-error"
            );
            input.classList.add("error");
            errorElement.textContent =
                "Coordinate difference too large (max 25000 points)";
            errorElement.classList.add("show");
        });
        return false;
    } else {
        // Reset error messages to default
        ["x1", "y1", "x2", "y2"].forEach((id) => {
            const errorElement = document.getElementById(
                id + "-error"
            );
            errorElement.textContent =
                "Value must be between -5000 and 5000";
        });
    }

    const allValid =
        isX1Valid && isY1Valid && isX2Valid && isY2Valid;
    return allValid;
}

function calculateLine() {
    const selectedAlgorithm = document.querySelector('input[name="algorithm"]:checked').value;
    const algorithmName = selectedAlgorithm === 'dda' ? 'DDA' : "Bresenham's";
    const wasmFunction = selectedAlgorithm === 'dda' ? Module._get_dda_points_array : Module._get_bresenham_points_array;

    // Check if WebAssembly module is ready
    if (!wasmFunction || typeof wasmFunction !== 'function') {
        updateStatus("error", `WASM function for ${algorithmName} not ready. Please wait or refresh.`);
        return;
    }

    // Validate inputs
    if (!validateAllInputs()) {
        updateStatus("error", "Please fix the input errors before calculating.");
        return;
    }

    showLoading(true);
    updateStatus("processing", `Calculating points with ${algorithmName}...`);

    const x1 = parseInt(document.getElementById("x1").value);
    const y1 = parseInt(document.getElementById("y1").value);
    const x2 = parseInt(document.getElementById("x2").value);
    const y2 = parseInt(document.getElementById("y2").value);

    try {
        const deltaX = Math.abs(x2 - x1);
        const deltaY = Math.abs(y2 - y1);
        const expectedPoints = Math.max(deltaX, deltaY) + 1;

        const arrayBytes = expectedPoints * 2 * 4; // 2 floats per point, 4 bytes per float
        const outputArrayPtr = Module._malloc(arrayBytes);
        const totalPointsPtr = Module._malloc(4);

        if (!outputArrayPtr || !totalPointsPtr) {
            updateStatus("error", "Failed to allocate memory in WebAssembly.");
            if (outputArrayPtr) Module._free(outputArrayPtr);
            if (totalPointsPtr) Module._free(totalPointsPtr);
            return;
        }

        wasmFunction(x1, y1, x2, y2, outputArrayPtr, totalPointsPtr);

        const actualPoints = Module.HEAP32[totalPointsPtr / 4];
        if (actualPoints <= 0 || actualPoints > expectedPoints * 2) {
            updateStatus("error", `Invalid point count returned: ${actualPoints}`);
            Module._free(outputArrayPtr);
            Module._free(totalPointsPtr);
            showLoading(false);
            return;
        }

        const points = [];
        const floatView = new Float32Array(Module.HEAPF32.buffer, outputArrayPtr, actualPoints * 2);
        for (let i = 0; i < actualPoints; i++) {
            points.push({ x: floatView[i * 2], y: floatView[i * 2 + 1] });
        }

        Module._free(outputArrayPtr);
        Module._free(totalPointsPtr);

        const slope = isFinite(Module._calculate_slope) ? Module._calculate_slope(x1, y1, x2, y2).toFixed(3) : "N/A";

        displayResults(points, x1, y1, x2, y2, slope, algorithmName);
        updateStatus("success", `✅ Successfully calculated ${points.length} points using ${algorithmName}`);

    } catch (error) {
        updateStatus("error", `Calculation failed: ${error.message}`);
    } finally {
        showLoading(false);
    }
}

function displayResults(points, x1, y1, x2, y2, slope, algorithmName) {
    const summaryCard = document.getElementById("summaryCard");
    document.getElementById("totalPoints").textContent = points.length;
    const distance = Math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2);
    document.getElementById("lineLength").textContent = distance.toFixed(2);
    document.getElementById("lineSlope").textContent = slope;
    summaryCard.style.display = "block";

    const isBresenham = algorithmName === "Bresenham's";
    const xPrecision = isBresenham ? 0 : 3;
    const yPrecision = isBresenham ? 0 : 3;

    let html = `
        <div class="results-container">
            <h3>📍 ${algorithmName} Points (${points.length} total)</h3>
            <table class="results-table">
                <thead>
                    <tr>
                        <th>Step</th>
                        <th>X</th>
                        <th>Y</th>
                        <th>Rounded-Coordinates</th>
                    </tr>
                </thead>
                <tbody>
    `;

    points.forEach((point, index) => {
        const roundedCoords = `(${Math.round(point.x)}, ${Math.round(point.y)})`;
        html += `
            <tr>
                <td>${index + 1}</td>
                <td>${point.x.toFixed(xPrecision)}</td>
                <td>${point.y.toFixed(yPrecision)}</td>
                <td>${roundedCoords}</td>
            </tr>
        `;
    });

    html += `
                </tbody>
            </table>
        </div>
    `;

    document.getElementById("resultsContainer").innerHTML = html;
}

function clearResults() {
    document.getElementById("resultsContainer").innerHTML =
        '<div class="no-results">Enter coordinates and click "Calculate Points" to see the line drawing algorithm in action.</div>';
    document.getElementById("summaryCard").style.display = "none";
    updateStatus(
        "ready",
        "Results cleared. Ready for new calculation."
    );
}

// Enhanced event listeners
document.addEventListener("DOMContentLoaded", function () {
    const inputs = document.querySelectorAll(
        'input[type="number"]'
    );
    inputs.forEach((input) => {
        input.addEventListener("input", function () {
            validateInput(this.id, parseInt(this.value));
        });

        input.addEventListener("keypress", function (e) {
            if (e.key === "Enter") {
                calculateLine();
            }
        });
    });
});