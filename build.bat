conan install . --build=missing --settings=build_type=Debug
conan install . --build=missing --settings=build_type=Release
call build\generators\conanbuild.bat
cmake . -B build -DCMAKE_TOOLCHAIN_FILE="build\generators\conan_toolchain.cmake"
devenv build\sandbox.sln
