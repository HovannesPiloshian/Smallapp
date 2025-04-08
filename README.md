# Printer Attribute Fetcher

## Overview

This C++ program retrieves and displays attributes from an IPP (Internet Printing Protocol) compatible printer using CURL and the IPP library.

## Features

Sends an IPP request to the printer to fetch attributes.

Uses CURL for HTTP communication.

Parses and displays printer attributes.


## Prerequisites
Ensure you have the following dependencies installed:
- A C++ compiler supporting C++ v11 or later.
- CURL development library.
- IPP library for handling IPP requests.
- Visual Studio Code with the C++ extension.
- https://marketplace.visualstudio.com/items/?itemName=ms-vscode.cpptools-extension-pack


# Installation

1. Install necessary dependencies for Windows using vcpkg:
```bash
vcpkg install curl
vcpkg install ipp
```
2. Clone or download the source code.
3. Ensure the necessary libraries are linked correctly within Visual Studio Code.

## Setting Up in Visual Studio Code

1. Open Visual Studio Code and install the C/C++ Extension from Microsoft.
2. Open the project folder in VSC.
3. Create a tasks.json file inside the .vscode folder:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "g++",
            "args": [
                "-o", "printer_attributes",
                "printer_attributes.cpp",
                "-lcurl"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```

4. Create a launch.json file inside the .vscode folder for debugging:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Run Printer Attributes",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/printer_attributes",
            "args": ["192.168.1.100"],
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb"
        }
    ]
}
```

5. Build the project by running Terminal > Run Task > Build.
6. Run the program via the Run and Debug panel.

## Usage

Run the program with the printer's IP address:

`./printer_attributes <printer_ip>`

### Example:

`./printer_attributes 192.168.1.100`

### Example Output

Printer Attributes:
- Model: HP LaserJet Pro
- Status: Online
- Resolution: 600 DPI

Code Explanation

writeCallback: Handles HTTP response data.

getPrinterAttributes: Constructs an IPP request, sends it via CURL, and parses the response.

main: Accepts printer IP as a command-line argument and calls getPrinterAttributes.
