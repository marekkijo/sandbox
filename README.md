# Sandbox

## Prerequisites

### Prepare vcpkg

`vcpkg` tool is included as a git submodule. After cloning the repository, initialize and update the submodules by running:

```bash
git submodule update --init --recursive
```

Bootstrap `vcpkg` by running the following commands:

For Windows:

```bash
cd vcpkg
.\bootstrap-vcpkg.bat
```

For MacOS and Linux:

```bash
cd vcpkg
./bootstrap-vcpkg.sh
```

Then, add the `vcpkg` directory to your `PATH` environment variable, and create a `VCPKG_ROOT` environment variable pointing to the `vcpkg` directory.

```bash
export VCPKG_ROOT="$(pwd)/vcpkg"
export PATH="$PATH:$VCPKG_ROOT"
```

Make sure to make these changes permanent by adding them to your shell profile, e.g., for `.zprofile`:

```bash
echo 'export VCPKG_ROOT="/path/to/your/sandbox/vcpkg"' >> ~/.zprofile
echo 'export PATH="$PATH:$VCPKG_ROOT"' >> ~/.zprofile
```

### Prepare Emscripten

Clone, install and activate the latest Emscripten SDK:

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ..
```

Make sure to add the Emscripten environment setup script to your shell profile, e.g., for `.zprofile`:

```bash
echo 'source "/path/to/your/emsdk/emsdk_env.sh"' >> ~/.zprofile
```

> :bulb: **Note:** If you encounter error like `zsh: ./emsdk: bad interpreter: /bin/sh^M: no such file or directory` install `dos2unix` and convert the line endings of the Emscripten scripts:

```bash
dos2unix emsdk/emsdk
dos2unix emsdk/*.sh
```

### Install Pre-commit Hooks

For Windows (python):

```bash
pip install pre-commit
```

For MacOS:

```bash
brew install pre-commit
```

For Linux:

```bash
sudo apt-get install pre-commit
```

Then, run the following command to install the pre-commit hooks:

```bash
pre-commit install
```

### Enable Clang-Tidy

Install LLVM and Clang-Tidy. Follow the instructions below to install them:

For MacOS:

```bash
brew install llvm
```

For Linux:

```bash
sudo apt-get install llvm
```

For Windows download the LLVM installer from [LLVM's official website](https://releases.llvm.org/download.html).

Make sure to add the LLVM and Clang-Tidy binaries to your `PATH` environment variable.

```bash
export LLVM_ROOT="/opt/homebrew/opt/llvm"
export PATH="$PATH:$LLVM_ROOT/bin"
```

Make sure to make these changes permanent by adding them to your shell profile, e.g., for `.zprofile`:

```bash
echo 'export LLVM_ROOT="/opt/homebrew/opt/llvm"' >> ~/.zprofile
echo 'export PATH="$PATH:$LLVM_ROOT/bin"' >> ~/.zprofile
```

## Building

```bash
cmake --preset=<your-preset> .
cmake --build . --preset=<your-preset>
```

Replace `<your-preset>` with the appropriate preset defined in `CMakePresets.json` file.
