LyraIDE
LyraIDE is an Integrated Development Environment (IDE) designed to provide a lightweight and efficient coding experience. It leverages modern libraries to deliver a robust platform for developers.
Disclaimer
LyraIDE is currently in alpha and under active development. Features may be incomplete, and bugs may be present. Use at your own risk and expect frequent updates as we work toward a stable release.
Features

Code editing with syntax highlighting
Terminal integration
Cross-platform support (Linux and Windows)
Customizable interface

Prerequisites
To build LyraIDE, ensure you have the following installed:

Qt6 Libraries (required for core functionality)
CMake (for build configuration)
C++ Compiler (GCC/Clang for Linux, MSVC for Windows)
Git (to clone the repository)

Linux-Specific

Install Qt6 via your package manager (e.g., sudo apt install qt6-base-dev on Ubuntu)
Ensure cmake and g++ are installed

Windows-Specific

Install Qt Creator with Qt6 support
Configure Qt Creator with a compatible MSVC compiler

Build Instructions
Cloning the Repository
git clone https://github.com/yourusername/LyraIDE.git
cd LyraIDE

Building on Linux

Create a build directory:mkdir build && cd build


Run CMake to configure the project:cmake ..


Build the project:make


Run LyraIDE:./LyraIDE



Building on Windows

Open Qt Creator.
Load the project by selecting the CMakeLists.txt file in the LyraIDE repository.
Configure the project with a Qt6 kit and MinGW compiler.
Build the project using the Build menu or Ctrl+B.
Run LyraIDE from Qt Creator or locate the executable in the build directory.

Contributing
Contributions are welcome! Please submit issues or pull requests on the GitHub repository. Ensure you test your changes thoroughly, as LyraIDE is in alpha.
Credits
LyraIDE is built with the help of the following open-source projects:

Monaco Editor: Provides the core code editing experience.
QTermWidget: Enables terminal functionality within the IDE.
