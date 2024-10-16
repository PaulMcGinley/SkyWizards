#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source_folder> <app_name>"
    exit 1
fi

SOURCE_FOLDER=$1
APP_NAME=$2
APP_BUNDLE="${APP_NAME}.app"
CONTENTS_FOLDER="${APP_BUNDLE}/Contents"
MACOS_FOLDER="${CONTENTS_FOLDER}/MacOS"
RESOURCES_FOLDER="${CONTENTS_FOLDER}/Resources"

# Create the necessary directories for the app bundle
mkdir -p "${MACOS_FOLDER}"
mkdir -p "${RESOURCES_FOLDER}"

# Copy the source folder contents to the MacOS folder
cp -R "${SOURCE_FOLDER}/" "${MACOS_FOLDER}/"

# Create the Info.plist file 
cat <<EOL > "${CONTENTS_FOLDER}/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>${APP_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>com.example.${APP_NAME}</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
</dict>
</plist>
EOL

echo "App bundle ${APP_BUNDLE} created successfully."