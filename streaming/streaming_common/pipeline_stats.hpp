#pragma once

#ifdef STREAMING_PIPELINE_STATS

# include <chrono>
# include <cinttypes>
# include <cstdint>
# include <cstdio>
# include <limits>

namespace streaming {

struct StageStats {
  std::chrono::microseconds min{std::numeric_limits<std::chrono::microseconds::rep>::max()};
  std::chrono::microseconds max{std::chrono::microseconds::zero()};
  std::chrono::microseconds sum{std::chrono::microseconds::zero()};
  uint32_t count{0};

  void record(std::chrono::microseconds d) noexcept {
    if (d < min) {
      min = d;
    }
    if (d > max) {
      max = d;
    }
    sum += d;
    ++count;
  }

  std::chrono::microseconds avg() const noexcept { return count > 0 ? sum / count : std::chrono::microseconds::zero(); }

  void reset() noexcept {
    min = std::chrono::microseconds{std::numeric_limits<std::chrono::microseconds::rep>::max()};
    max = std::chrono::microseconds::zero();
    sum = std::chrono::microseconds::zero();
    count = 0;
  }
};

constexpr uint32_t PIPELINE_STATS_REPORT_INTERVAL = 100u;

class EncodeStats {
public:
  struct Frame {
    std::chrono::microseconds render_us{};
    std::chrono::microseconds capture_us{};
    std::chrono::microseconds flip_us{};
    std::chrono::microseconds rgb_to_yuv_us{};
    std::chrono::microseconds encode_us{};
  };

  void set_output(std::FILE *out) noexcept { out_ = out; }

  void record(const Frame &f) noexcept {
    render_.record(f.render_us);
    capture_.record(f.capture_us);
    flip_.record(f.flip_us);
    rgb_to_yuv_.record(f.rgb_to_yuv_us);
    encode_.record(f.encode_us);
    ++frame_count_;

    if (frame_count_ >= PIPELINE_STATS_REPORT_INTERVAL) {
      report();
      reset();
    }
  }

private:
  void report() const {
    fprintf(out_, "--- Encode pipeline stats (over %u frames) ---\n", frame_count_);
    print_stage(out_, "  render      ", render_);
    print_stage(out_, "  capture     ", capture_);
    print_stage(out_, "  flip        ", flip_);
    print_stage(out_, "  rgb->yuv    ", rgb_to_yuv_);
    print_stage(out_, "  encode      ", encode_);
    const auto total = render_.avg() + capture_.avg() + flip_.avg() + rgb_to_yuv_.avg() + encode_.avg();
    fprintf(out_, "  total (avg) : %6" PRId64 " us\n", static_cast<int64_t>(total.count()));
    fprintf(out_, "----------------------------------------------\n\n");
    std::fflush(out_);
  }

  static void print_stage(std::FILE *out, const char *name, const StageStats &s) {
    fprintf(out,
            "%s min=%6" PRId64 "  avg=%6" PRId64 "  max=%6" PRId64 " us\n",
            name,
            static_cast<int64_t>(s.count > 0 ? s.min.count() : 0),
            static_cast<int64_t>(s.avg().count()),
            static_cast<int64_t>(s.max.count()));
  }

  void reset() noexcept {
    render_.reset();
    capture_.reset();
    flip_.reset();
    rgb_to_yuv_.reset();
    encode_.reset();
    frame_count_ = 0;
  }

  StageStats render_{};
  StageStats capture_{};
  StageStats flip_{};
  StageStats rgb_to_yuv_{};
  StageStats encode_{};
  uint32_t frame_count_{0};
  std::FILE *out_{stdout};
};

class DecodeStats {
public:
  struct Frame {
    std::chrono::microseconds upload_us{};
    std::chrono::microseconds receive_us{};
    std::chrono::microseconds yuv_to_rgb_us{};
    std::chrono::microseconds texture_upload_us{};
    std::chrono::microseconds display_us{};
  };

  void set_output(std::FILE *out) noexcept { out_ = out; }

  void set_max_reports(uint32_t n) noexcept { max_reports_ = n; }

  bool record(const Frame &f) noexcept {
    upload_.record(f.upload_us);
    receive_.record(f.receive_us);
    yuv_to_rgb_.record(f.yuv_to_rgb_us);
    texture_upload_.record(f.texture_upload_us);
    display_.record(f.display_us);
    ++frame_count_;

    if (frame_count_ >= PIPELINE_STATS_REPORT_INTERVAL) {
      report();
      reset();
      ++reports_count_;
      return max_reports_ > 0u && reports_count_ >= max_reports_;
    }
    return false;
  }

private:
  void report() const {
    fprintf(out_, "--- Decode pipeline stats (over %u frames) ---\n", frame_count_);
    print_stage(out_, "  upload      ", upload_);
    print_stage(out_, "  receive     ", receive_);
    print_stage(out_, "  yuv->rgb    ", yuv_to_rgb_);
    print_stage(out_, "  tex upload  ", texture_upload_);
    print_stage(out_, "  display     ", display_);
    const auto total = upload_.avg() + receive_.avg() + yuv_to_rgb_.avg() + texture_upload_.avg() + display_.avg();
    fprintf(out_, "  total (avg) : %6" PRId64 " us\n", static_cast<int64_t>(total.count()));
    fprintf(out_, "----------------------------------------------\n\n");
    std::fflush(out_);
  }

  static void print_stage(std::FILE *out, const char *name, const StageStats &s) {
    fprintf(out,
            "%s min=%6" PRId64 "  avg=%6" PRId64 "  max=%6" PRId64 " us\n",
            name,
            static_cast<int64_t>(s.count > 0 ? s.min.count() : 0),
            static_cast<int64_t>(s.avg().count()),
            static_cast<int64_t>(s.max.count()));
  }

  void reset() noexcept {
    upload_.reset();
    receive_.reset();
    yuv_to_rgb_.reset();
    texture_upload_.reset();
    display_.reset();
    frame_count_ = 0;
  }

  StageStats upload_{};
  StageStats receive_{};
  StageStats yuv_to_rgb_{};
  StageStats texture_upload_{};
  StageStats display_{};
  uint32_t frame_count_{0};
  uint32_t reports_count_{0};
  uint32_t max_reports_{0};
  std::FILE *out_{stdout};
};

} // namespace streaming

#endif // STREAMING_PIPELINE_STATS
