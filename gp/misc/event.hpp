#pragma once

#include <cstdint>
#include <stdexcept>

namespace gp::misc {
/**
 * @brief Represents system and user input events such as timer clicks, mouse clicks, key presses, etc.
 */
class Event {
public:
  /**
   * @brief Type alias for subtype used in Type enum.
   */
  using TypeSubtype = std::uint32_t;

  /**
   * @brief Enum representing the type of user input event.
   */
  enum class Type : TypeSubtype {
    None,        /**< No input event */
    Init,        /**< Window got initialized */
    Quit,        /**< Window is about to quit */
    Resize,      /**< Window resize */
    Redraw,      /**< Redraw */
    MouseButton, /**< Mouse button press/release */
    MouseMove,   /**< Mouse moved */
    MouseScroll, /**< Mouse scroll event */
    Key,         /**< Key press/release */
    DragDrop     /**< File drag and drop */
  };

  /**
   * @brief Type alias for subtype used in Action enum.
   */
  using ActionSubtype = std::uint8_t;

  /**
   * @brief Enum representing basic actions.
   */
  enum class Action : ActionSubtype {
    None,    /**< No action */
    Pressed, /**< Press action (mouse button or key) */
    Released /**< Release action (mouse button or key) */
  };

  /**
   * @brief Type alias for subtype used in MouseButton enum.
   */
  using MouseButtonSubtype = std::uint8_t;

  /**
   * @brief Enum representing mouse button types.
   */
  enum class MouseButton : MouseButtonSubtype {
    None,   /**< No mouse button */
    Left,   /**< Left mouse button */
    Middle, /**< Middle mouse button */
    Right   /**< Right mouse button */
  };

  /**
   * @brief Type alias for subtype used in MouseButtonMask enum.
   */
  using MouseButtonMaskSubType = std::uint8_t;

  /**
   * @brief Enum representing mouse button masks.
   */
  enum MouseButtonMask : MouseButtonMaskSubType {
    None = 0x0,   /**< No mouse button */
    Left = 0x1,   /**< Left mouse button mask */
    Middle = 0x2, /**< Middle mouse button mask */
    Right = 0x4   /**< Right mouse button mask */
  };

  /**
   * @brief Type alias for subtype used in KeyCode enum.
   */
  using KeyCodeSubtype = std::int32_t;

  /**
   * @brief Enum representing key codes.
   */
  enum class KeyCode : KeyCodeSubtype {
    None,  /**< No key */
    Down,  /**< Down arrow key */
    Left,  /**< Left arrow key */
    Right, /**< Right arrow key */
    Up,    /**< Up arrow key */
    Escape /**< Esc key */
  };

  struct InitData {
    int width;  /**< Initial window width */
    int height; /**< Initial window height */
  };

  struct QuitData {
    int close_flag; /**< Closing flag received from the underlying system */
  };

  struct ResizeData {
    int width;  /**< New window width */
    int height; /**< New window height */
  };

  struct MouseButtonData {
    Action action;      /**< Pressed/released action */
    MouseButton button; /**< Mouse button */
  };

  struct MouseMoveData {
    float x{};                                ///< X-coordinate for mouse movement (window-relative).
    float y{};                                ///< Y-coordinate for mouse movement (window-relative).
    float x_rel{};                            ///< Relative X movement.
    float y_rel{};                            ///< Relative Y movement.
    MouseButtonMaskSubType mouse_button_mask; /**< Mouse button mask for mouse move event */

    /**
     * @brief Checks if left mouse button is held.
     */
    bool left_is_down() const;

    /**
     * @brief Checks if middle mouse button is held.
     */
    bool middle_is_down() const;

    /**
     * @brief Checks if right mouse button is held.
     */
    bool right_is_down() const;
  };

  struct MouseScrollData {
    /**
     * @brief Vertical scroll amount for mouse scrolling.
     * Positive values indicate scrolling away from the user, while negative values indicate scrolling towards the
     * user.
     */
    float vertical{};

    /**
     * @brief Horizontal scroll amount for mouse scrolling.
     * Positive values indicate scrolling to the right, while negative values indicate scrolling to the left.
     */
    float horizontal{};
  };

  struct KeyData {
    Action action; /**< Pressed/released action */
    KeyCode code;  /**< Key code */
  };

  struct DragDropData {
    std::string filepath; /**< Path of a dragged and dropped file */
  };

  Event(const Type type, const std::uint32_t timestamp);
  Event(const Event &other);
  Event(Event &&other) noexcept;
  Event &operator=(const Event &other);
  Event &operator=(Event &&other) noexcept;
  ~Event();
  Type type() const;
  std::uint32_t timestamp() const;
  InitData &init();
  const InitData &init() const;
  QuitData &quit();
  const QuitData &quit() const;
  ResizeData &resize();
  const ResizeData &resize() const;
  MouseButtonData &mouse_button();
  const MouseButtonData &mouse_button() const;
  MouseMoveData &mouse_move();
  const MouseMoveData &mouse_move() const;
  MouseScrollData &mouse_scroll();
  const MouseScrollData &mouse_scroll() const;
  KeyData &key();
  const KeyData &key() const;
  DragDropData &drag_drop();
  const DragDropData &drag_drop() const;

private:
  void copy(const Event &other);
  void move(Event &&other);
  void construct_complex_members();
  void destruct_complex_members();
  void assign_all(const Event &other);

  /**
   * @brief The type of user input event.
   */
  Type type_{Type::None};

  /**
   * @brief Timestamp of the input event in milliseconds.
   */
  std::uint32_t timestamp_{};

  /**
   * @brief Union representing various input event data.
   */
  union {
    InitData init_;                /**< Init event */
    QuitData quit_;                /**< Quit event */
    ResizeData resize_;            /**< Resize event */
    MouseButtonData mouse_button_; /**< Mouse button event */
    MouseMoveData mouse_move_;     /**< Mouse move event */
    MouseScrollData mouse_scroll_; /**< Mouse scroll event */
    KeyData key_;                  /**< Key event */
    DragDropData drag_drop_;       /**< Drag and drop event */
  };
};
} // namespace gp::misc
