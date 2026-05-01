#include <gp/misc/event.hpp>

#include <gtest/gtest.h>

namespace {
using gp::misc::Event;

TEST(EventCopyAssign, SameType_Init_CopiesData) {
  Event src{Event::Type::Init, 1u};
  src.init().width = 1920;
  src.init().height = 1080;

  Event dst{Event::Type::Init, 0u};
  dst = src;

  EXPECT_EQ(dst.type(), Event::Type::Init);
  EXPECT_EQ(dst.timestamp(), 1u);
  EXPECT_EQ(dst.init().width, 1920);
  EXPECT_EQ(dst.init().height, 1080);
}

TEST(EventCopyAssign, CrossType_InitToDragDrop_CopiesFilepath) {
  Event src{Event::Type::DragDrop, 42u};
  src.drag_drop().filepath = "/tmp/file.txt";

  Event dst{Event::Type::Init, 0u};
  dst.init().width = 800;
  dst = src;

  EXPECT_EQ(dst.type(), Event::Type::DragDrop);
  EXPECT_EQ(dst.timestamp(), 42u);
  EXPECT_EQ(dst.drag_drop().filepath, "/tmp/file.txt");
}

TEST(EventCopyAssign, CrossType_DragDropToKey_DestroysFilepath) {
  Event src{Event::Type::Key, 7u};
  src.key().action = Event::Action::Pressed;
  src.key().scan_code = Event::ScanCode::A;

  Event dst{Event::Type::DragDrop, 0u};
  dst.drag_drop().filepath = "/tmp/old.txt";
  dst = src;

  EXPECT_EQ(dst.type(), Event::Type::Key);
  EXPECT_EQ(dst.timestamp(), 7u);
  EXPECT_EQ(dst.key().action, Event::Action::Pressed);
  EXPECT_EQ(dst.key().scan_code, Event::ScanCode::A);
}

TEST(EventCopyAssign, SameType_DragDrop_CopiesFilepath) {
  Event src{Event::Type::DragDrop, 10u};
  src.drag_drop().filepath = "/home/user/drop.png";

  Event dst{Event::Type::DragDrop, 0u};
  dst.drag_drop().filepath = "/old/path.png";
  dst = src;

  EXPECT_EQ(dst.type(), Event::Type::DragDrop);
  EXPECT_EQ(dst.drag_drop().filepath, "/home/user/drop.png");
}

TEST(EventCopyAssign, SelfAssign_DoesNotCrash) {
  Event ev{Event::Type::DragDrop, 5u};
  ev.drag_drop().filepath = "/self/assign.txt";

  ev = ev; // NOLINT(clang-diagnostic-self-assign-overloaded)

  EXPECT_EQ(ev.type(), Event::Type::DragDrop);
  EXPECT_EQ(ev.drag_drop().filepath, "/self/assign.txt");
}

TEST(EventMoveAssign, CrossType_InitToDragDrop_MovesFilepath) {
  Event src{Event::Type::DragDrop, 99u};
  src.drag_drop().filepath = "/move/me.txt";

  Event dst{Event::Type::Init, 0u};
  dst = std::move(src);

  EXPECT_EQ(dst.type(), Event::Type::DragDrop);
  EXPECT_EQ(dst.timestamp(), 99u);
  EXPECT_EQ(dst.drag_drop().filepath, "/move/me.txt");
}

TEST(EventMoveAssign, CrossType_DragDropToKey_DestroysFilepath) {
  Event src{Event::Type::Key, 3u};
  src.key().action = Event::Action::Released;
  src.key().scan_code = Event::ScanCode::Escape;

  Event dst{Event::Type::DragDrop, 0u};
  dst.drag_drop().filepath = "/move/old.txt";
  dst = std::move(src);

  EXPECT_EQ(dst.type(), Event::Type::Key);
  EXPECT_EQ(dst.key().action, Event::Action::Released);
  EXPECT_EQ(dst.key().scan_code, Event::ScanCode::Escape);
}
} // namespace
