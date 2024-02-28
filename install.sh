#!/bin/bash

# Default installation directory and executable name
INSTALL_DIR="/usr/local/bin"
EXECUTABLE_NAME="prompter"
COMPILER="g++"

# Parse command-line arguments for customization
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --uninstall) UNINSTALL=true ;;
        -d|--dir) INSTALL_DIR="$2"; shift ;;
        -e|--exec) EXECUTABLE_NAME="$2"; shift ;;
        -c|--compiler) COMPILER="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

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

# Check for uninstall flag
if [ "$UNINSTALL" == "true" ]; then
    uninstall
fi

# Confirm installation with the user
while true; do
    read -p "Do you wish to install $EXECUTABLE_NAME to $INSTALL_DIR? [y/n] " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

# Initial prompt to choose the compiler strategy
echo "Select your compiler option:"
echo "1) Use the default g++ compiler"
echo "2) Search for an installed g++ version"
echo "3) Input a custom compiler"
read -p "Enter your choice (1/2/3): " choice

case $choice in
    1) COMPILER="g++";;
    2) COMPILER="search";;
    3) read -p "Enter the custom compiler you want to use: " COMPILER;;
    *) echo "Invalid choice. Exiting."; exit 1;;
esac

# Compiler search logic
checkpoint=1
while [ "$COMPILER" == "search" ]; do
    found=false
    for i in $(seq $checkpoint 17); do
        if which "g++-$i" > /dev/null; then
            echo "Found compiler: g++-$i"
            COMPILER="g++-$i"
            found=true
            break
        fi
    done
    if [ "$found" = false ]; then
        echo "No suitable compiler found in the search range."
        read -p "Enter the C++ compiler to use or 'search' to retry: " input
        COMPILER="$input"
    fi
done

# Check if the specified C++ compiler is available
if ! which $COMPILER > /dev/null; then
    echo "$COMPILER compiler not found. Please install it or check your PATH."
    exit 1
fi

echo "Using compiler: $COMPILER"


# Ensure the specified C++ compiler is available
if ! which $COMPILER > /dev/null; then
    echo "$COMPILER compiler not found. Please install it or check your PATH."
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
if ! cmake .. -DCMAKE_CXX_COMPILER=$(which $COMPILER); then
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
