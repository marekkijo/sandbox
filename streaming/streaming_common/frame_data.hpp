#pragma once

#include <cstddef>
#include <vector>

namespace streaming {
// FrameSubType should be an std::byte, however using std::vector<std::byte> generates a conflict with LibDataChannel
// library and results in a linker error. This is a workaround to avoid the conflict.
// TODO: Use std::byte when the conflict is resolved.
using FrameSubType = std::uint8_t;
using FrameData = std::vector<FrameSubType>;
} // namespace streaming
