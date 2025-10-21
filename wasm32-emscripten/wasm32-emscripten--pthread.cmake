include($ENV{VCPKG_ROOT}/triplets/community/wasm32-emscripten.cmake)

set(ENV{EMCC_CFLAGS} "$ENV{EMCC_CFLAGS} -pthread")
