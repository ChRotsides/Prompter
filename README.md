# README.md for Prompter Utility

## Overview
Prompter is a command-line utility designed to enhance developers' productivity by automating the extraction of function names and return statements from various programming language files. It also features cross-platform clipboard support to facilitate the easy sharing of code snippets and outputs between different environments.

### Key Features
- **Function Name Extraction**: Automatically identifies and lists function names from source code, supporting languages like Python, JavaScript, TypeScript, C++, and C.
- **Return Statement Analysis**: Pinpoints return statements within functions, aiding in code comprehension and debugging.
- **Clipboard Integration**: Allows copying of extracted information to the clipboard on both Windows and Linux platforms, streamlining the process of sharing and using code snippets.

### Supported File Types
- Python: `.py`
- JavaScript: `.js`
- TypeScript: `.ts`
- C/C++: `.cpp`, `.c`
- Header files: `.h`

## Installation

### Prerequisites
- A C++ compiler (g++-12 recommended)
- CMake (version 3.10 or higher)

### Steps
1. **Clone the repository** to your local machine.
   ```bash
   git clone https://github.com/ChRotsides/Prompter.git
   ```
2. **Navigate to the project directory** and run the installation script.
   ```bash
   cd Prompter
   ./install.sh
   ```
   Use `sudo` if necessary to grant installation permissions.

## Usage

### Basic Commands
- **Extract Function Names**: Use the `--function-names-from-file` option followed by the file name.
  ```bash
  prompter --function-names-from-file example.py
  ```
- **Process Multiple Files**: Utilize the `--file-list` option to process a list of files.
  ```bash
  prompter --file-list file1.cpp file2.js *.py
  ```
- **Copy to Clipboard**: By default, if no output file is specified, the output is copied to the clipboard. The output is also printed to the terminal.
  ```bash
  prompter --function-names-from-file example.py *.js
  ```
- **Output to File**: Use the `--output` option to specify an output file.
  ```bash
  prompter --function-names-from-file example.py --output results.txt
  ```

### Advanced Options
- **Help**: Display the help message with `-h` or `--help`.
- **Version**: Output version information with `-V` or `--version`.

## Contributing
Contributions to Prompter are welcome! Whether you're fixing bugs, adding new features, or improving documentation, your help is appreciated. Please feel free to submit pull requests or open issues on GitHub.
