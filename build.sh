#!/bin/sh

# Stop the script if any command fails
set -e

# --- Configuration ---
APP_NAME="PathFinder"
SRC_FILE="main.cpp"

# --- Clean up previous build ---
echo "--- Cleaning up old build files ---"
rm -rf "${APP_NAME}.app"
rm -f "${APP_NAME}"
rm -f *.o

# --- Compile for both architectures ---
echo "--- Compiling for Apple Silicon (ARM64) ---"
clang++ -std=c++17 -arch arm64 -c "${SRC_FILE}" -o main_arm64.o -I/opt/homebrew/opt/sfml/include

echo "--- Compiling for Intel (x86_64) ---"
clang++ -std=c++17 -arch x86_64 -c "${SRC_FILE}" -o main_x86_64.o -I/opt/homebrew/opt/sfml/include

# --- Link and create the universal binary ---
echo "--- Linking and creating Universal Binary ---"
clang++ -o "${APP_NAME}" main_arm64.o main_x86_64.o \
  -L/opt/homebrew/opt/sfml/lib \
  -lsfml-graphics -lsfml-window -lsfml-system

# --- Create the .app bundle structure ---
echo "--- Creating .app bundle structure ---"
mkdir -p "${APP_NAME}.app/Contents/MacOS"
mkdir -p "${APP_NAME}.app/Contents/Resources"
mkdir -p "${APP_NAME}.app/Contents/Frameworks"

# --- Move files into the bundle ---
echo "--- Populating bundle ---"
mv "${APP_NAME}" "${APP_NAME}.app/Contents/MacOS/"
cp /opt/homebrew/opt/sfml/lib/libsfml-graphics.dylib "${APP_NAME}.app/Contents/Frameworks/"
cp /opt/homebrew/opt/sfml/lib/libsfml-window.dylib "${APP_NAME}.app/Contents/Frameworks/"
cp /opt/homebrew/opt/sfml/lib/libsfml-system.dylib "${APP_NAME}.app/Contents/Frameworks/"
# Add any other SFML libraries you use (e.g., audio, network) here

# --- CRITICAL STEP: Fix library paths ---
echo "--- Fixing library paths ---"
# Define paths for easier reading
EXEC_PATH="${APP_NAME}.app/Contents/MacOS/${APP_NAME}"
FRAMEWORKS_PATH="${APP_NAME}.app/Contents/Frameworks"
SFML_GRAPHICS="${FRAMEWORKS_PATH}/libsfml-graphics.dylib"
SFML_WINDOW="${FRAMEWORKS_PATH}/libsfml-window.dylib"
SFML_SYSTEM="${FRAMEWORKS_PATH}/libsfml-system.dylib"
SFML_HOMEBREW_PATH="/opt/homebrew/opt/sfml/lib"

# Fix paths in the main executable
install_name_tool -change "${SFML_HOMEBREW_PATH}/libsfml-graphics.dylib" "@executable_path/../Frameworks/libsfml-graphics.dylib" "${EXEC_PATH}"
install_name_tool -change "${SFML_HOMEBREW_PATH}/libsfml-window.dylib" "@executable_path/../Frameworks/libsfml-window.dylib" "${EXEC_PATH}"
install_name_tool -change "${SFML_HOMEBREW_PATH}/libsfml-system.dylib" "@executable_path/../Frameworks/libsfml-system.dylib" "${EXEC_PATH}"

# Fix inter-library dependencies
install_name_tool -change "${SFML_HOMEBREW_PATH}/libsfml-window.dylib" "@executable_path/../Frameworks/libsfml-window.dylib" "${SFML_GRAPHICS}"
install_name_tool -change "${SFML_HOMEBREW_PATH}/libsfml-system.dylib" "@executable_path/../Frameworks/libsfml-system.dylib" "${SFML_GRAPHICS}"
install_name_tool -change "${SFML_HOMEBREW_PATH}/libsfml-system.dylib" "@executable_path/../Frameworks/libsfml-system.dylib" "${SFML_WINDOW}"

# --- Create Info.plist ---
echo "--- Creating Info.plist ---"
PLIST_PATH="${APP_NAME}.app/Contents/Info.plist"
cat <<EOF > "${PLIST_PATH}"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>${APP_NAME}</string>
    <key>CFBundleIconFile</key>
    <string>icon.icns</string>
    <key>CFBundleIdentifier</key>
    <string>com.yourcompany.yourapp</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
</dict>
</plist>
EOF

# --- Ad-Hoc Code Sign ---
echo "--- Signing the application bundle ---"
codesign --force --deep --sign - "${APP_NAME}.app"

echo "--- Build complete! ---"
echo "You can now test '${APP_NAME}.app'"
