#include "misc.hpp"

#include <algorithm>
#include <cctype>
#include <iterator>

namespace gp::ffmpeg {
AVCodecID codec_name_to_id(const std::string &codec_name) {
  auto normalized_codec_name = std::string{};
  normalized_codec_name.reserve(codec_name.size());
  std::transform(codec_name.begin(), codec_name.end(), std::back_inserter(normalized_codec_name), [](auto c) {
    return std::tolower(c);
  });

  const auto *codec_descriptor = avcodec_descriptor_get_by_name(normalized_codec_name.c_str());
  if (codec_descriptor == nullptr) {
    return AV_CODEC_ID_NONE;
  }
  return codec_descriptor->id;
}
} // namespace gp::ffmpeg
