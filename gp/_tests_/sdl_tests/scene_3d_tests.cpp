#include "utils/utils.hpp"

#include <gp/sdl/scene_3d.hpp>

#include <gtest/gtest.h>

namespace {
class Scene3DEventCollector : public gp::sdl::Scene3D {
public:
  explicit Scene3DEventCollector(std::shared_ptr<gp::sdl::internal::SDLContext> ctx = nullptr)
      : gp::sdl::Scene3D{std::move(ctx)} {}

  void loop(const gp::misc::Event &event) override {
    events_history_.push_back(event);

    call_at_event_type(event.type());
    call_at_events_counter();
  }

  void set_callback_at_event_type(const gp::misc::Event::Type type, const std::function<void()> &callback) {
    event_callbacks_[type] = callback;
  }

  void set_callback_at_events_counter(const std::size_t count, const std::function<void()> &callback) {
    events_counter_callback_ = callback;
    expected_events_count_ = count;
    current_events_count_ = 0;
  }

  const std::vector<gp::misc::Event> &events_history() const { return events_history_; }

  void pass_request_close() { request_close(); }

private:
  void call_at_event_type(const gp::misc::Event::Type type) {
    const auto it = event_callbacks_.find(type);
    if (it != event_callbacks_.end()) {
      it->second();
    }
  }

  void call_at_events_counter() {
    if (events_counter_callback_) {
      current_events_count_++;
      if (current_events_count_ == expected_events_count_) {
        events_counter_callback_();
        events_counter_callback_ = nullptr;
      }
    }
  }

  std::vector<gp::misc::Event> events_history_{};
  std::unordered_map<gp::misc::Event::Type, std::function<void()>> event_callbacks_{};
  std::function<void()> events_counter_callback_{};
  std::size_t expected_events_count_{0u};
  std::size_t current_events_count_{0u};
};

} // namespace

TEST(Scene3D, Constructor_WithoutContext_DoesNotThrow) {
  std::unique_ptr<Scene3DEventCollector> scene;
  EXPECT_NO_THROW({ scene = std::make_unique<Scene3DEventCollector>(); });
}

TEST(Scene3D, Constructor_WithContext_DoesNotThrow) {
  auto ctx = std::make_shared<gp::sdl::internal::SDLContext>();
  std::unique_ptr<Scene3DEventCollector> scene;
  EXPECT_NO_THROW({ scene = std::make_unique<Scene3DEventCollector>(ctx); });
}

TEST(Scene3D, Init_DoesNotThrow) {
  Scene3DEventCollector scene;
  EXPECT_NO_THROW({ scene.init(800, 600, "Test Window"); });
}

TEST(Scene3D, Exec_RequestClose_ShouldGetQuitEvent) {
  TimeoutThread timeout_thread;

  Scene3DEventCollector scene;
  scene.init(800, 600, "Test Window");

  // Request close after any 50 events (should already be initialized by then and redraw events are coming)
  scene.set_callback_at_events_counter(50, [&]() { scene.pass_request_close(); });

  scene.set_callback_at_event_type(gp::misc::Event::Type::Quit, [&]() {
    // Ensure that the last event is Quit
    ASSERT_EQ(scene.events_history().back().type(), gp::misc::Event::Type::Quit);
    timeout_thread.finish();
  });

  scene.exec();
}
