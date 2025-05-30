# README

## Prerequisites

### For Windows Users

Make sure you have installed Visual Studio 2019 or later with the C++ development tools.
If not, you can download it from [Visual Studio's official website](https://visualstudio.microsoft.com/downloads/).

### For MacOS Users

Make sure you have installed Xcode and the command line tools.
If not, you can install Xcode from the App Store and then run the following command in your terminal:

```bash
xcode-select --install
```

### For Linux Users

Make sure you have installed the necessary build tools and libraries. You can install them using your package manager. For example, on Ubuntu, you can run:

```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
```

### Installed Git with LFS Support

Make sure you have Git installed with LFS (Large File Storage) support.

### Installed CMake

Make sure you have installed CMake and added it to your `PATH` environment variable.
If not, you can download it from [CMake's official website](https://cmake.org/download/).

### Installed Python

Make sure you have Python 3.x installed and added to your `PATH` environment variable.
If not, you can download it from [Python's official website](https://www.python.org/downloads/).

### Installed vcpkg

Install `vcpkg` to manage your C++ libraries. You can follow the instructions below to install it:

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh # or .\bootstrap-vcpkg.bat on Windows
```

Then, add the `vcpkg` directory to your `PATH` environment variable, and create a VCPKG_ROOT environment variable pointing to the `vcpkg` directory.

## Building

```bash
git clone https://github.com/marekkijo/sandbox.git
cd sandbox
cmake --preset=<your-preset> .
cmake --build . --preset=<your-preset>
```

Replace `<your-preset>` with the appropriate preset defined in `CMakePresets.json` file.

## Contributing

Follow the instructions at `https://pre-commit.com/` to install `pre-commit`.
