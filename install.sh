#!/bin/bash

# Define the installation directory and executable name
INSTALL_DIR="/usr/local/bin"
EXECUTABLE_NAME="prompter"

# Function to uninstall the executable
uninstall() {
    echo "Uninstalling $EXECUTABLE_NAME from $INSTALL_DIR..."
    if sudo rm "$INSTALL_DIR/$EXECUTABLE_NAME"; then
        echo "Uninstallation successful."
    else
        echo "Uninstallation failed."
    fi
    exit
}

# Check for uninstall argument
if [ "$1" == "--uninstall" ]; then
    uninstall
fi

# Ensure the C++ compiler is available
if ! which g++-12 > /dev/null; then
    echo "g++-12 compiler not found. Please install it or check your PATH."
    exit 1
fi

# Check if the user has permission to write to the installation directory
if [ ! -w "$INSTALL_DIR" ] && [ "$EUID" -ne 0 ]; then
    echo "Error: You do not have permission to write to $INSTALL_DIR."
    echo "Try running this script with sudo or as root, or choose a different installation directory."
    exit 1
fi

# Create a build directory and navigate into it
echo "Creating build directory..."
mkdir -p build && cd build || exit 1

# Run CMake to configure the project and generate the build system
echo "Configuring project with CMake..."
if ! cmake .. -DCMAKE_CXX_COMPILER=$(which g++-12); then
    echo "CMake configuration failed."
    exit 1
fi

# Build the project
echo "Building project..."
if ! cmake --build .; then
    echo "Build failed."
    exit 1
fi

# Install the executable to the specified installation directory
echo "Installing executable to $INSTALL_DIR..."
if [ "$EUID" -ne 0 ]; then
    # Use sudo if not running as root
    if ! sudo cp "./$EXECUTABLE_NAME" "$INSTALL_DIR"; then
        echo "Failed to install $EXECUTABLE_NAME to $INSTALL_DIR."
        exit 1
    fi
else
    if ! cp "./$EXECUTABLE_NAME" "$INSTALL_DIR"; then
        echo "Failed to install $EXECUTABLE_NAME to $INSTALL_DIR."
        exit 1
    fi
fi

# Verify the installation and provide feedback
if [ -x "$INSTALL_DIR/$EXECUTABLE_NAME" ]; then
    echo "Installation successful!"
    echo "You can run the program using the command: $EXECUTABLE_NAME"
else
    echo "Installation failed."
    exit 1
fi

# Navigate out of the build directory
cd ..
