# How to Build
## Prerequisites
Call `conan profile detect`.
Make sure the `~/.conan/profiles/default` looks like this:
```
[settings]
arch=x86_64
build_type=Release
compiler=msvc
compiler.cppstd=23
compiler.runtime=dynamic
compiler.version=193
os=Windows

[conf]
tools.env.virtualenv:powershell=True
```
## Building
`conan install . --output-folder=build --build=missing --settings=build_type=Debug`
`conan install . --output-folder=build --build=missing --settings=build_type=Release`
`cd build`
`.\conanbuild.ps1`
`cmake .. -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"`
`.\sandbox.sln`
