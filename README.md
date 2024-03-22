# How to Build

## Prerequisites

Installed `CMake`, `Visual Studio` and `Python`.\
Call `conan profile detect`.\
Make sure the `~/.conan/profiles/default` looks like this:

```ini
[settings]
arch=x86_64
build_type=Release
compiler=msvc
compiler.cppstd=23
compiler.runtime=dynamic
compiler.version=193
os=Windows
```

## Building

- make sure you are using `cmd` not `PowerShell`
- go to `./deps/libdatachannel`
- and execute `conan create . --build=missing`
- go back to `./`
- and execute:
  - `conan install . --build=missing --settings=build_type=Debug`
  - `conan install . --build=missing --settings=build_type=Release`
  - `.\build\generators\conanbuild.bat`
  - `cmake . -B build -DCMAKE_TOOLCHAIN_FILE="build\generators\conan_toolchain.cmake"`
  - `.\build\sandbox.sln`

## Contributing

Follow the instructions at `https://pre-commit.com/` to install `pre-commit`.
