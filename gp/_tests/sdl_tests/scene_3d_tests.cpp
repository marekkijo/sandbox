#include <gp/sdl/scene_3d.hpp>

#include <gtest/gtest.h>

namespace {
class Scene3DImpl : public gp::sdl::Scene3D {
public:
  explicit Scene3DImpl(std::shared_ptr<gp::sdl::internal::SDLContext> ctx = nullptr)
      : gp::sdl::Scene3D{std::move(ctx)} {}

  void loop(const gp::misc::Event &event) override {}
};
} // namespace

TEST(Scene3D, Constructor_WithoutContext_DoesNotThrow) {
  std::unique_ptr<Scene3DImpl> scene;
  EXPECT_NO_THROW({ scene = std::make_unique<Scene3DImpl>(); });
}

TEST(Scene3D, Constructor_WithContext_DoesNotThrow) {
  auto ctx = std::make_shared<gp::sdl::internal::SDLContext>();
  std::unique_ptr<Scene3DImpl> scene;
  EXPECT_NO_THROW({ scene = std::make_unique<Scene3DImpl>(ctx); });
}
