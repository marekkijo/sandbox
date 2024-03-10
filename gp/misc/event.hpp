#pragma once

#include <gp/misc/event_fwd.hpp>

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
   * @brief Type alias for subtype used in ScanCode enum.
   */
  using ScanCodeSubtype = std::int32_t;

  /**
   * @brief Enum representing key scan codes.
   */
  enum class ScanCode : ScanCodeSubtype {
    A,                                 /**< The 'A' key. Example: 'A' */
    B,                                 /**< The 'B' key. Example: 'B' */
    C,                                 /**< The 'C' key. Example: 'C' */
    D,                                 /**< The 'D' key. Example: 'D' */
    E,                                 /**< The 'E' key. Example: 'E' */
    F,                                 /**< The 'F' key. Example: 'F' */
    G,                                 /**< The 'G' key. Example: 'G' */
    H,                                 /**< The 'H' key. Example: 'H' */
    I,                                 /**< The 'I' key. Example: 'I' */
    J,                                 /**< The 'J' key. Example: 'J' */
    K,                                 /**< The 'K' key. Example: 'K' */
    L,                                 /**< The 'L' key. Example: 'L' */
    M,                                 /**< The 'M' key. Example: 'M' */
    N,                                 /**< The 'N' key. Example: 'N' */
    O,                                 /**< The 'O' key. Example: 'O' */
    P,                                 /**< The 'P' key. Example: 'P' */
    Q,                                 /**< The 'Q' key. Example: 'Q' */
    R,                                 /**< The 'R' key. Example: 'R' */
    S,                                 /**< The 'S' key. Example: 'S' */
    T,                                 /**< The 'T' key. Example: 'T' */
    U,                                 /**< The 'U' key. Example: 'U' */
    V,                                 /**< The 'V' key. Example: 'V' */
    W,                                 /**< The 'W' key. Example: 'W' */
    X,                                 /**< The 'X' key. Example: 'X' */
    Y,                                 /**< The 'Y' key. Example: 'Y' */
    Z,                                 /**< The 'Z' key. Example: 'Z' */
    Number0,                           /**< The '0' key. Example: '0' */
    Number1,                           /**< The '1' key. Example: '1' */
    Number2,                           /**< The '2' key. Example: '2' */
    Number3,                           /**< The '3' key. Example: '3' */
    Number4,                           /**< The '4' key. Example: '4' */
    Number5,                           /**< The '5' key. Example: '5' */
    Number6,                           /**< The '6' key. Example: '6' */
    Number7,                           /**< The '7' key. Example: '7' */
    Number8,                           /**< The '8' key. Example: '8' */
    Number9,                           /**< The '9' key. Example: '9' */
    F1,                                /**< The F1 key. */
    F2,                                /**< The F2 key. */
    F3,                                /**< The F3 key. */
    F4,                                /**< The F4 key. */
    F5,                                /**< The F5 key. */
    F6,                                /**< The F6 key. */
    F7,                                /**< The F7 key. */
    F8,                                /**< The F8 key. */
    F9,                                /**< The F9 key. */
    F10,                               /**< The F10 key. */
    F11,                               /**< The F11 key. */
    F12,                               /**< The F12 key. */
    F13,                               /**< The F13 key. */
    F14,                               /**< The F14 key. */
    F15,                               /**< The F15 key. */
    F16,                               /**< The F16 key. */
    F17,                               /**< The F17 key. */
    F18,                               /**< The F18 key. */
    F19,                               /**< The F19 key. */
    F20,                               /**< The F20 key. */
    F21,                               /**< The F21 key. */
    F22,                               /**< The F22 key. */
    F23,                               /**< The F23 key. */
    F24,                               /**< The F24 key. */
    ApplicationControlKeypadBack,      /**< The application control keypad back key. */
    ApplicationControlKeypadBookmarks, /**< The application control keypad bookmarks key. */
    ApplicationControlKeypadForward,   /**< The application control keypad forward key. */
    ApplicationControlKeypadHome,      /**< The application control keypad home key. */
    ApplicationControlKeypadRefresh,   /**< The application control keypad refresh key. */
    ApplicationControlKeypadSearch,    /**< The application control keypad search key. */
    ApplicationControlKeypadStop,      /**< The application control keypad stop key. */
    Again,                             /**< The 'Again' key. */
    AltErase,                          /**< The 'AltErase' key. */
    Apostrophe,                        /**< The 'Apostrophe' key. Example: ''' */
    Application,                       /**< The 'Application' key. */
    AudioMute,                         /**< The 'AudioMute' key. */
    AudioNext,                         /**< The 'AudioNext' key. */
    AudioPlay,                         /**< The 'AudioPlay' key. */
    AudioPrev,                         /**< The 'AudioPrev' key. */
    AudioStop,                         /**< The 'AudioStop' key. */
    Backslash,                         /**< The 'Backslash' key. Example: '\' */
    Backspace,                         /**< The 'Backspace' key. */
    BrightnessDown,                    /**< The 'BrightnessDown' key. */
    BrightnessUp,                      /**< The 'BrightnessUp' key. */
    Calculator,                        /**< The 'Calculator' key. */
    Cancel,                            /**< The 'Cancel' key. */
    CapsLock,                          /**< The 'CapsLock' key. */
    Clear,                             /**< The 'Clear' key. */
    ClearAgain,                        /**< The 'ClearAgain' key. */
    Comma,                             /**< The 'Comma' key. Example: ',' */
    Computer,                          /**< The 'Computer' key. */
    Copy,                              /**< The 'Copy' key. */
    CrSel,                             /**< The 'CrSel' key. */
    CurrencySubUnit,                   /**< The currency subunit key. */
    CurrencyUnit,                      /**< The currency unit key. */
    Cut,                               /**< The 'Cut' key. */
    DecimalSeparator,                  /**< The decimal separator key. */
    Delete,                            /**< The 'Delete' key. */
    DisplaySwitch,                     /**< The 'DisplaySwitch' key. */
    Down,                              /**< The 'Down' key. */
    Eject,                             /**< The 'Eject' key. */
    End,                               /**< The 'End' key. */
    Equals,                            /**< The 'Equals' key. Example: '=' */
    Escape,                            /**< The 'Escape' key. */
    Execute,                           /**< The 'Execute' key. */
    ExSel,                             /**< The 'ExSel' key. */
    Find,                              /**< The 'Find' key. */
    Grave,                             /**< The 'Grave' key. Example: '`' */
    Help,                              /**< The 'Help' key. */
    Home,                              /**< The 'Home' key. */
    Insert,                            /**< The 'Insert' key. */
    KeyboardIlluminationDown,          /**< The keyboard illumination down key. */
    KeyboardIlluminationToggle,        /**< The keyboard illumination toggle key. */
    KeyboardIlluminationUp,            /**< The keyboard illumination up key. */
    NumericKeypad0,                    /**< The '0' key on the numeric keypad. Example: '0' */
    NumericKeypad1,                    /**< The '1' key on the numeric keypad. Example: '1' */
    NumericKeypad2,                    /**< The '2' key on the numeric keypad. Example: '2' */
    NumericKeypad3,                    /**< The '3' key on the numeric keypad. Example: '3' */
    NumericKeypad4,                    /**< The '4' key on the numeric keypad. Example: '4' */
    NumericKeypad5,                    /**< The '5' key on the numeric keypad. Example: '5' */
    NumericKeypad6,                    /**< The '6' key on the numeric keypad. Example: '6' */
    NumericKeypad7,                    /**< The '7' key on the numeric keypad. Example: '7' */
    NumericKeypad8,                    /**< The '8' key on the numeric keypad. Example: '8' */
    NumericKeypad9,                    /**< The '9' key on the numeric keypad. Example: '9' */
    NumericKeypadA,                    /**< The 'A' key on the numeric keypad. Example: 'A' */
    NumericKeypadB,                    /**< The 'B' key on the numeric keypad. Example: 'B' */
    NumericKeypadC,                    /**< The 'C' key on the numeric keypad. Example: 'C' */
    NumericKeypadD,                    /**< The 'D' key on the numeric keypad. Example: 'D' */
    NumericKeypadE,                    /**< The 'E' key on the numeric keypad. Example: 'E' */
    NumericKeypadF,                    /**< The 'F' key on the numeric keypad. Example: 'F' */
    NumericKeypad00,                   /**< The '00' key on the numeric keypad. Example: '00' */
    NumericKeypad000,                  /**< The '000' key on the numeric keypad. Example: '000' */
    NumericKeypadAmpersand,            /**< The '&' key on the numeric keypad. Example: '&' */
    NumericKeypadAt,                   /**< The '@' key on the numeric keypad. Example: '@' */
    NumericKeypadBackspace,            /**< The 'Backspace' key on the numeric keypad. */
    NumericKeypadBinary,               /**< The binary key on the numeric keypad. */
    NumericKeypadClear,                /**< The 'Clear' key on the numeric keypad. */
    NumericKeypadClearEntry,           /**< The 'ClearEntry' key on the numeric keypad. */
    NumericKeypadColon,                /**< The ':' key on the numeric keypad. Example: ':' */
    NumericKeypadComma,                /**< The ',' key on the numeric keypad. Example: ',' */
    NumericKeypadDoubleAmpersand,      /**< The '&&' key on the numeric keypad. Example: '&&' */
    NumericKeypadDoubleVerticalBar,    /**< The '||' key on the numeric keypad. Example: '||' */
    NumericKeypadDecimal,              /**< The decimal key on the numeric keypad. */
    NumericKeypadDivide,               /**< The divide key on the numeric keypad. */
    NumericKeypadEnter,                /**< The 'Enter' key on the numeric keypad. */
    NumericKeypadEquals,               /**< The '=' key on the numeric keypad. Example: '=' */
    NumericKeypadEqualsAS400,          /**< The equals AS400 key on the numeric keypad. */
    NumericKeypadExclam,               /**< The '!' key on the numeric keypad. Example: '!' */
    NumericKeypadGreater,              /**< The '>' key on the numeric keypad. Example: '>' */
    NumericKeypadHash,                 /**< The '#' key on the numeric keypad. Example: '#' */
    NumericKeypadHexadecimal,          /**< The hexadecimal key on the numeric keypad. */
    NumericKeypadLeftBrace,            /**< The '{' key on the numeric keypad. Example: '{' */
    NumericKeypadLeftParenthesis,      /**< The '(' key on the numeric keypad. Example: '(' */
    NumericKeypadLess,                 /**< The '<' key on the numeric keypad. Example: '<' */
    NumericKeypadMemAdd,               /**< The memory add key on the numeric keypad. */
    NumericKeypadMemClear,             /**< The memory clear key on the numeric keypad. */
    NumericKeypadMemDivide,            /**< The memory divide key on the numeric keypad. */
    NumericKeypadMemMultiply,          /**< The memory multiply key on the numeric keypad. */
    NumericKeypadMemRecall,            /**< The memory recall key on the numeric keypad. */
    NumericKeypadMemStore,             /**< The memory store key on the numeric keypad. */
    NumericKeypadMemSubtract,          /**< The memory subtract key on the numeric keypad. */
    NumericKeypadMinus,                /**< The '-' key on the numeric keypad. Example: '-' */
    NumericKeypadMultiply,             /**< The multiply key on the numeric keypad. */
    NumericKeypadOctal,                /**< The octal key on the numeric keypad. */
    NumericKeypadPercent,              /**< The '%' key on the numeric keypad. Example: '%' */
    NumericKeypadPeriod,               /**< The '.' key on the numeric keypad. Example: '.' */
    NumericKeypadPlus,                 /**< The '+' key on the numeric keypad. Example: '+' */
    NumericKeypadPlusMinus,            /**< The plus-minus key on the numeric keypad. */
    NumericKeypadPower,                /**< The power key on the numeric keypad. */
    NumericKeypadRightBrace,           /**< The '}' key on the numeric keypad. Example: '}' */
    NumericKeypadRightParenthesis,     /**< The ')' key on the numeric keypad. Example: ')' */
    NumericKeypadSpace,                /**< The space key on the numeric keypad. Example: ' ' */
    NumericKeypadTab,                  /**< The tab key on the numeric keypad. Example: '\t' */
    NumericKeypadVerticalBar,          /**< The '|' key on the numeric keypad. Example: '|' */
    NumericKeypadXOR,                  /**< The XOR key on the numeric keypad. */
    LeftAlt,                           /**< The left Alt key. */
    LeftCtrl,                          /**< The left Ctrl key. */
    Left,                              /**< The left arrow key. */
    LeftBracket,                       /**< The '[' key. Example: '[' */
    LeftGui,                           /**< The left GUI key. */
    LeftShift,                         /**< The left Shift key. */
    Mail,                              /**< The 'Mail' key. */
    MediaSelect,                       /**< The 'MediaSelect' key. */
    Menu,                              /**< The 'Menu' key. */
    Minus,                             /**< The '-' key. Example: '-' */
    ModeSwitch,                        /**< The 'ModeSwitch' key. */
    Mute,                              /**< The 'Mute' key. */
    NumLock,                           /**< The 'NumLock' key. */
    Oper,                              /**< The 'Oper' key. */
    Out,                               /**< The 'Out' key. */
    PageDown,                          /**< The 'PageDown' key. */
    PageUp,                            /**< The 'PageUp' key. */
    Paste,                             /**< The 'Paste' key. */
    Pause,                             /**< The 'Pause' key. */
    Period,                            /**< The '.' key. Example: '.' */
    Power,                             /**< The 'Power' key. */
    PrintScreen,                       /**< The 'PrintScreen' key. */
    Prior,                             /**< The 'Prior' key. */
    RightAlt,                          /**< The right Alt key. */
    RightCtrl,                         /**< The right Ctrl key. */
    Enter,                             /**< The 'Enter' key. */
    Return,                            /**< The 'Return' key. */
    RightGui,                          /**< The right GUI key. */
    Right,                             /**< The right arrow key. */
    RightBracket,                      /**< The ']' key. Example: ']' */
    RightShift,                        /**< The right Shift key. */
    ScrollLock,                        /**< The 'ScrollLock' key. */
    Select,                            /**< The 'Select' key. */
    Semicolon,                         /**< The ';' key. Example: ';' */
    Separator,                         /**< The 'Separator' key. */
    Slash,                             /**< The '/' key. Example: '/' */
    Sleep,                             /**< The 'Sleep' key. */
    Space,                             /**< The space key. Example: ' ' */
    Stop,                              /**< The 'Stop' key. */
    SysReq,                            /**< The 'SysReq' key. */
    Tab,                               /**< The 'Tab' key. Example: '\t' */
    ThousandsSeparator,                /**< The thousands separator key. */
    Undo,                              /**< The 'Undo' key. */
    None,                              /**< No key. */
    Up,                                /**< The 'Up' key. */
    VolumeDown,                        /**< The 'VolumeDown' key. */
    VolumeUp,                          /**< The 'VolumeUp' key. */
    WWW,                               /**< The 'WWW' key. */
    EnumCount                          /**< The number of key scan codes. */
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
    float x{};                                /**< X-coordinate for mouse movement (window-relative). */
    float y{};                                /**< Y-coordinate for mouse movement (window-relative). */
    float x_rel{};                            /**< Relative X movement. */
    float y_rel{};                            /**< Relative Y movement. */
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
    Action action;      /**< Pressed/released action */
    ScanCode scan_code; /**< Key scan code */
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
