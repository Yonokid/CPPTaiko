#!/bin/bash
# install_deps_windows.sh
# Run this in MSYS2 MinGW64 terminal

echo "================================================"
echo "Installing YataiDON dependencies for Windows"
echo "================================================"
echo ""
echo "Make sure you're running this in MSYS2 MinGW64 terminal!"
echo ""

# Update package database
echo "[1/2] Updating package database..."
pacman -Sy

# Install dependencies
echo "[2/2] Installing dependencies..."
pacman -S --needed --noconfirm \
    mingw-w64-x86_64-SDL2 \
    mingw-w64-x86_64-raylib \
    mingw-w64-x86_64-rapidjson \
    mingw-w64-x86_64-spdlog \
    mingw-w64-x86_64-libsndfile \
    mingw-w64-x86_64-libsamplerate \
    mingw-w64-x86_64-meson \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-pkgconf \
    mingw-w64-x86_64-tomlplusplus

echo ""
echo "================================================"
echo "Checking installations..."
echo "================================================"

# Verify installations
pkg-config --modversion sdl2 && echo "✓ SDL2 installed" || echo "✗ SDL2 NOT found"
pkg-config --modversion raylib && echo "✓ raylib installed" || echo "✗ raylib NOT found"
pkg-config --modversion spdlog && echo "✓ spdlog installed" || echo "✗ spdlog NOT found"
pkg-config --modversion sndfile && echo "✓ libsndfile installed" || echo "✗ libsndfile NOT found"
pkg-config --modversion samplerate && echo "✓ libsamplerate installed" || echo "✗ libsamplerate NOT found"

echo ""
echo "================================================"
echo "Installing header-only libraries manually..."
echo "================================================"

# digestpp (header-only, not in repos)
if [ ! -d "/mingw64/include/digestpp" ]; then
    echo "Installing digestpp..."
    TEMP_DIR=$(mktemp -d)
    cd "$TEMP_DIR"
    git clone --depth 1 https://github.com/kerukuro/digestpp.git
    mkdir -p /mingw64/include/digestpp
    cp -r digestpp/ /mingw64/include/
    cd -
    rm -rf "$TEMP_DIR"
    echo "✓ digestpp installed to /mingw64/include/digestpp"
else
    echo "✓ digestpp already installed"
fi

# rapidjson headers
if [ ! -d "/mingw64/include/rapidjson" ]; then
    echo "Checking rapidjson installation..."
    pacman -S --needed --noconfirm mingw-w64-x86_64-rapidjson
fi

echo ""
echo "================================================"
echo "Installation complete!"
echo "================================================"
echo ""
echo "Verifying header-only libraries..."
echo ""

# Verify header installations
[ -d "/mingw64/include/rapidjson" ] && echo "✓ rapidjson headers" || echo "✗ rapidjson headers MISSING"
[ -d "/mingw64/include/digestpp" ] && echo "✓ digestpp headers" || echo "✗ digestpp headers MISSING"

echo ""
echo "Next steps:"
echo "1. Close and reopen your MSYS2 terminal (or run: source ~/.bashrc)"
echo "2. Optional: Run test_headers_mingw.sh to verify all dependencies"
echo "3. Run: meson setup build"
echo "4. Run: meson compile -C build"
echo ""