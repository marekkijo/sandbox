#pragma once

#include <cstdint>

namespace gp::misc {
/**
 * @brief Represents system and user input events such as timer clicks, mouse clicks, key presses, etc.
 */
struct Event {
  /**
   * @brief Type alias for subtype used in Type enum.
   */
  using TypeSubtype = std::uint32_t;

  /**
   * @brief Enum representing the type of user input event.
   */
  enum class Type : TypeSubtype {
    None,        /**< No input event */
    MouseButton, /**< Mouse button press/release*/
    MouseMove,   /**< Mouse moved */
    MouseScroll, /**< Mouse scroll event */
    Key          /**< Key press/release */
  };

  /**
   * @brief The type of user input event.
   */
  Type type{Type::None};

  /**
   * @brief Timestamp of the input event.
   */
  std::uint32_t timestamp{};

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

  /**
   * @brief Union representing various input event data.
   */
  union {
    struct {
      Action action;      /**< Pressed/released action */
      MouseButton button; /**< Mouse button */
    } mouse_button;       /**< Mouse button event */

    struct {
      float x{};                                ///< X-coordinate for mouse movement (window-relative).
      float y{};                                ///< Y-coordinate for mouse movement (window-relative).
      float x_rel{};                            ///< Relative X movement.
      float y_rel{};                            ///< Relative Y movement.
      MouseButtonMaskSubType mouse_button_mask; /**< Mouse button mask for mouse move event */

      /**
       * @brief Checks if left mouse button is held.
       */
      bool left_is_down() const { return mouse_button_mask & MouseButtonMask::Left; }

      /**
       * @brief Checks if middle mouse button is held.
       */
      bool middle_is_down() const { return mouse_button_mask & MouseButtonMask::Middle; }

      /**
       * @brief Checks if right mouse button is held.
       */
      bool right_is_down() const { return mouse_button_mask & MouseButtonMask::Right; }
    } mouse_move; /**< Mouse move event */

    struct {
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
    } mouse_scroll; /**< Mouse scroll event */

    struct {
      Action action; /**< Pressed/released action */
      KeyCode code;  /**< Key code */
    } key;           /**< Key event */
  };
};
} // namespace gp::misc
