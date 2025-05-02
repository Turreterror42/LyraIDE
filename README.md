# LyraIDE

LyraIDE is an Integrated Development Environment (IDE) designed to provide a lightweight and efficient coding experience. It leverages modern libraries to deliver a robust platform for developers.

## Disclaimer

**LyraIDE is currently in alpha and under active development.** Features may be incomplete, and bugs may be present. Use at your own risk and expect frequent updates as we work toward a stable release.

## Features

- Code editing with syntax highlighting
- Terminal integration
- Cross-platform support (Linux and Windows)
- Customizable interface

## Prerequisites

To build LyraIDE, ensure you have the following installed:

- **Qt6 Libraries** (required for core functionality)
- **CMake** (for build configuration)
- **C++ Compiler** (GCC/Clang for Linux, MinGW for Windows)
- **Git** (to clone the repository)

### Linux-Specific
- Install Qt6 via your package manager (e.g., `sudo apt install qt6-base-dev cmake gcc` on Ubuntu)
- Ensure `cmake` and `g++` are installed

### Windows-Specific
- Install **Qt Creator** with Qt6 support
- Configure Qt Creator with a compatible MSVC compiler

## Build Instructions

### Cloning the Repository
```bash
git clone https://github.com/yourusername/LyraIDE.git
cd LyraIDE
```

### Building on Linux
1. Create a build directory:
   ```bash
   mkdir build && cd build
   ```
2. Run CMake to configure the project:
   ```bash
   cmake ..
   ```
3. Build the project:
   ```bash
   make
   ```
4. Run LyraIDE:
   ```bash
   ./LyraIDE
   ```

### Building on Windows
1. Open **Qt Creator**.
2. Load the project by selecting the `CMakeLists.txt` file in the LyraIDE repository.
3. Configure the project with a Qt6 kit and MinGW compiler.
4. Build the project using the **Build** menu or `Ctrl+B`.
5. Run LyraIDE from Qt Creator or locate the executable in the build directory.

## Contributing

Contributions are welcome! Please submit issues or pull requests on the GitHub repository. Ensure you test your changes thoroughly, as LyraIDE is in alpha.

## Credits

LyraIDE is built with the help of the following open-source projects:

- **Monaco Editor**: Provides the core code editing experience.
- **QTermWidget**: Enables terminal functionality within the IDE.