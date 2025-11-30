#include "misc.hpp"

#include <stdexcept>
#include <unordered_map>

namespace gp::json {
namespace {
misc::Event::Type from_string(const std::string &type_string) {
  static const auto type_map = std::unordered_map<std::string, misc::Event::Type>{
      {       "Init",        misc::Event::Type::Init},
      {       "Quit",        misc::Event::Type::Quit},
      {     "Resize",      misc::Event::Type::Resize},
      {     "Redraw",      misc::Event::Type::Redraw},
      {"MouseButton", misc::Event::Type::MouseButton},
      {  "MouseMove",   misc::Event::Type::MouseMove},
      {"MouseScroll", misc::Event::Type::MouseScroll},
      {        "Key",         misc::Event::Type::Key},
      {   "DragDrop",    misc::Event::Type::DragDrop}
  };
  const auto it = type_map.find(type_string);
  if (it != type_map.end()) {
    return it->second;
  }
  return misc::Event::Type::None;
}

misc::Event::ScanCode to_scan_code(const std::string &scan_code_string) {
  static const auto scan_code_map = std::unordered_map<std::string, misc::Event::ScanCode>{
      {                                "A",                                 misc::Event::ScanCode::A},
      {                                "B",                                 misc::Event::ScanCode::B},
      {                                "C",                                 misc::Event::ScanCode::C},
      {                                "D",                                 misc::Event::ScanCode::D},
      {                                "E",                                 misc::Event::ScanCode::E},
      {                                "F",                                 misc::Event::ScanCode::F},
      {                                "G",                                 misc::Event::ScanCode::G},
      {                                "H",                                 misc::Event::ScanCode::H},
      {                                "I",                                 misc::Event::ScanCode::I},
      {                                "J",                                 misc::Event::ScanCode::J},
      {                                "K",                                 misc::Event::ScanCode::K},
      {                                "L",                                 misc::Event::ScanCode::L},
      {                                "M",                                 misc::Event::ScanCode::M},
      {                                "N",                                 misc::Event::ScanCode::N},
      {                                "O",                                 misc::Event::ScanCode::O},
      {                                "P",                                 misc::Event::ScanCode::P},
      {                                "Q",                                 misc::Event::ScanCode::Q},
      {                                "R",                                 misc::Event::ScanCode::R},
      {                                "S",                                 misc::Event::ScanCode::S},
      {                                "T",                                 misc::Event::ScanCode::T},
      {                                "U",                                 misc::Event::ScanCode::U},
      {                                "V",                                 misc::Event::ScanCode::V},
      {                                "W",                                 misc::Event::ScanCode::W},
      {                                "X",                                 misc::Event::ScanCode::X},
      {                                "Y",                                 misc::Event::ScanCode::Y},
      {                                "Z",                                 misc::Event::ScanCode::Z},
      {                          "Number0",                           misc::Event::ScanCode::Number0},
      {                          "Number1",                           misc::Event::ScanCode::Number1},
      {                          "Number2",                           misc::Event::ScanCode::Number2},
      {                          "Number3",                           misc::Event::ScanCode::Number3},
      {                          "Number4",                           misc::Event::ScanCode::Number4},
      {                          "Number5",                           misc::Event::ScanCode::Number5},
      {                          "Number6",                           misc::Event::ScanCode::Number6},
      {                          "Number7",                           misc::Event::ScanCode::Number7},
      {                          "Number8",                           misc::Event::ScanCode::Number8},
      {                          "Number9",                           misc::Event::ScanCode::Number9},
      {                               "F1",                                misc::Event::ScanCode::F1},
      {                               "F2",                                misc::Event::ScanCode::F2},
      {                               "F3",                                misc::Event::ScanCode::F3},
      {                               "F4",                                misc::Event::ScanCode::F4},
      {                               "F5",                                misc::Event::ScanCode::F5},
      {                               "F6",                                misc::Event::ScanCode::F6},
      {                               "F7",                                misc::Event::ScanCode::F7},
      {                               "F8",                                misc::Event::ScanCode::F8},
      {                               "F9",                                misc::Event::ScanCode::F9},
      {                              "F10",                               misc::Event::ScanCode::F10},
      {                              "F11",                               misc::Event::ScanCode::F11},
      {                              "F12",                               misc::Event::ScanCode::F12},
      {                              "F13",                               misc::Event::ScanCode::F13},
      {                              "F14",                               misc::Event::ScanCode::F14},
      {                              "F15",                               misc::Event::ScanCode::F15},
      {                              "F16",                               misc::Event::ScanCode::F16},
      {                              "F17",                               misc::Event::ScanCode::F17},
      {                              "F18",                               misc::Event::ScanCode::F18},
      {                              "F19",                               misc::Event::ScanCode::F19},
      {                              "F20",                               misc::Event::ScanCode::F20},
      {                              "F21",                               misc::Event::ScanCode::F21},
      {                              "F22",                               misc::Event::ScanCode::F22},
      {                              "F23",                               misc::Event::ScanCode::F23},
      {                              "F24",                               misc::Event::ScanCode::F24},
      {     "ApplicationControlKeypadBack",      misc::Event::ScanCode::ApplicationControlKeypadBack},
      {"ApplicationControlKeypadBookmarks", misc::Event::ScanCode::ApplicationControlKeypadBookmarks},
      {  "ApplicationControlKeypadForward",   misc::Event::ScanCode::ApplicationControlKeypadForward},
      {     "ApplicationControlKeypadHome",      misc::Event::ScanCode::ApplicationControlKeypadHome},
      {  "ApplicationControlKeypadRefresh",   misc::Event::ScanCode::ApplicationControlKeypadRefresh},
      {   "ApplicationControlKeypadSearch",    misc::Event::ScanCode::ApplicationControlKeypadSearch},
      {     "ApplicationControlKeypadStop",      misc::Event::ScanCode::ApplicationControlKeypadStop},
      {                            "Again",                             misc::Event::ScanCode::Again},
      {                         "AltErase",                          misc::Event::ScanCode::AltErase},
      {                       "Apostrophe",                        misc::Event::ScanCode::Apostrophe},
      {                      "Application",                       misc::Event::ScanCode::Application},
      {                   "MediaNextTrack",                    misc::Event::ScanCode::MediaNextTrack},
      {                        "MediaPlay",                         misc::Event::ScanCode::MediaPlay},
      {               "MediaPreviousTrack",                misc::Event::ScanCode::MediaPreviousTrack},
      {                        "MediaStop",                         misc::Event::ScanCode::MediaStop},
      {                        "Backslash",                         misc::Event::ScanCode::Backslash},
      {                        "Backspace",                         misc::Event::ScanCode::Backspace},
      {                           "Cancel",                            misc::Event::ScanCode::Cancel},
      {                         "CapsLock",                          misc::Event::ScanCode::CapsLock},
      {                            "Clear",                             misc::Event::ScanCode::Clear},
      {                       "ClearAgain",                        misc::Event::ScanCode::ClearAgain},
      {                            "Comma",                             misc::Event::ScanCode::Comma},
      {                             "Copy",                              misc::Event::ScanCode::Copy},
      {                            "CrSel",                             misc::Event::ScanCode::CrSel},
      {                  "CurrencySubUnit",                   misc::Event::ScanCode::CurrencySubUnit},
      {                     "CurrencyUnit",                      misc::Event::ScanCode::CurrencyUnit},
      {                              "Cut",                               misc::Event::ScanCode::Cut},
      {                 "DecimalSeparator",                  misc::Event::ScanCode::DecimalSeparator},
      {                           "Delete",                            misc::Event::ScanCode::Delete},
      {                             "Down",                              misc::Event::ScanCode::Down},
      {                            "Eject",                             misc::Event::ScanCode::Eject},
      {                              "End",                               misc::Event::ScanCode::End},
      {                           "Equals",                            misc::Event::ScanCode::Equals},
      {                           "Escape",                            misc::Event::ScanCode::Escape},
      {                          "Execute",                           misc::Event::ScanCode::Execute},
      {                            "ExSel",                             misc::Event::ScanCode::ExSel},
      {                             "Find",                              misc::Event::ScanCode::Find},
      {                            "Grave",                             misc::Event::ScanCode::Grave},
      {                             "Help",                              misc::Event::ScanCode::Help},
      {                             "Home",                              misc::Event::ScanCode::Home},
      {                           "Insert",                            misc::Event::ScanCode::Insert},
      {                   "NumericKeypad0",                    misc::Event::ScanCode::NumericKeypad0},
      {                   "NumericKeypad1",                    misc::Event::ScanCode::NumericKeypad1},
      {                   "NumericKeypad2",                    misc::Event::ScanCode::NumericKeypad2},
      {                   "NumericKeypad3",                    misc::Event::ScanCode::NumericKeypad3},
      {                   "NumericKeypad4",                    misc::Event::ScanCode::NumericKeypad4},
      {                   "NumericKeypad5",                    misc::Event::ScanCode::NumericKeypad5},
      {                   "NumericKeypad6",                    misc::Event::ScanCode::NumericKeypad6},
      {                   "NumericKeypad7",                    misc::Event::ScanCode::NumericKeypad7},
      {                   "NumericKeypad8",                    misc::Event::ScanCode::NumericKeypad8},
      {                   "NumericKeypad9",                    misc::Event::ScanCode::NumericKeypad9},
      {                   "NumericKeypadA",                    misc::Event::ScanCode::NumericKeypadA},
      {                   "NumericKeypadB",                    misc::Event::ScanCode::NumericKeypadB},
      {                   "NumericKeypadC",                    misc::Event::ScanCode::NumericKeypadC},
      {                   "NumericKeypadD",                    misc::Event::ScanCode::NumericKeypadD},
      {                   "NumericKeypadE",                    misc::Event::ScanCode::NumericKeypadE},
      {                   "NumericKeypadF",                    misc::Event::ScanCode::NumericKeypadF},
      {                  "NumericKeypad00",                   misc::Event::ScanCode::NumericKeypad00},
      {                 "NumericKeypad000",                  misc::Event::ScanCode::NumericKeypad000},
      {           "NumericKeypadAmpersand",            misc::Event::ScanCode::NumericKeypadAmpersand},
      {                  "NumericKeypadAt",                   misc::Event::ScanCode::NumericKeypadAt},
      {           "NumericKeypadBackspace",            misc::Event::ScanCode::NumericKeypadBackspace},
      {              "NumericKeypadBinary",               misc::Event::ScanCode::NumericKeypadBinary},
      {               "NumericKeypadClear",                misc::Event::ScanCode::NumericKeypadClear},
      {          "NumericKeypadClearEntry",           misc::Event::ScanCode::NumericKeypadClearEntry},
      {               "NumericKeypadColon",                misc::Event::ScanCode::NumericKeypadColon},
      {               "NumericKeypadComma",                misc::Event::ScanCode::NumericKeypadComma},
      {     "NumericKeypadDoubleAmpersand",      misc::Event::ScanCode::NumericKeypadDoubleAmpersand},
      {   "NumericKeypadDoubleVerticalBar",    misc::Event::ScanCode::NumericKeypadDoubleVerticalBar},
      {             "NumericKeypadDecimal",              misc::Event::ScanCode::NumericKeypadDecimal},
      {              "NumericKeypadDivide",               misc::Event::ScanCode::NumericKeypadDivide},
      {               "NumericKeypadEnter",                misc::Event::ScanCode::NumericKeypadEnter},
      {              "NumericKeypadEquals",               misc::Event::ScanCode::NumericKeypadEquals},
      {         "NumericKeypadEqualsAS400",          misc::Event::ScanCode::NumericKeypadEqualsAS400},
      {              "NumericKeypadExclam",               misc::Event::ScanCode::NumericKeypadExclam},
      {             "NumericKeypadGreater",              misc::Event::ScanCode::NumericKeypadGreater},
      {                "NumericKeypadHash",                 misc::Event::ScanCode::NumericKeypadHash},
      {         "NumericKeypadHexadecimal",          misc::Event::ScanCode::NumericKeypadHexadecimal},
      {           "NumericKeypadLeftBrace",            misc::Event::ScanCode::NumericKeypadLeftBrace},
      {     "NumericKeypadLeftParenthesis",      misc::Event::ScanCode::NumericKeypadLeftParenthesis},
      {                "NumericKeypadLess",                 misc::Event::ScanCode::NumericKeypadLess},
      {              "NumericKeypadMemAdd",               misc::Event::ScanCode::NumericKeypadMemAdd},
      {            "NumericKeypadMemClear",             misc::Event::ScanCode::NumericKeypadMemClear},
      {           "NumericKeypadMemDivide",            misc::Event::ScanCode::NumericKeypadMemDivide},
      {         "NumericKeypadMemMultiply",          misc::Event::ScanCode::NumericKeypadMemMultiply},
      {           "NumericKeypadMemRecall",            misc::Event::ScanCode::NumericKeypadMemRecall},
      {            "NumericKeypadMemStore",             misc::Event::ScanCode::NumericKeypadMemStore},
      {         "NumericKeypadMemSubtract",          misc::Event::ScanCode::NumericKeypadMemSubtract},
      {               "NumericKeypadMinus",                misc::Event::ScanCode::NumericKeypadMinus},
      {            "NumericKeypadMultiply",             misc::Event::ScanCode::NumericKeypadMultiply},
      {               "NumericKeypadOctal",                misc::Event::ScanCode::NumericKeypadOctal},
      {             "NumericKeypadPercent",              misc::Event::ScanCode::NumericKeypadPercent},
      {              "NumericKeypadPeriod",               misc::Event::ScanCode::NumericKeypadPeriod},
      {                "NumericKeypadPlus",                 misc::Event::ScanCode::NumericKeypadPlus},
      {           "NumericKeypadPlusMinus",            misc::Event::ScanCode::NumericKeypadPlusMinus},
      {               "NumericKeypadPower",                misc::Event::ScanCode::NumericKeypadPower},
      {          "NumericKeypadRightBrace",           misc::Event::ScanCode::NumericKeypadRightBrace},
      {    "NumericKeypadRightParenthesis",     misc::Event::ScanCode::NumericKeypadRightParenthesis},
      {               "NumericKeypadSpace",                misc::Event::ScanCode::NumericKeypadSpace},
      {                 "NumericKeypadTab",                  misc::Event::ScanCode::NumericKeypadTab},
      {         "NumericKeypadVerticalBar",          misc::Event::ScanCode::NumericKeypadVerticalBar},
      {                 "NumericKeypadXOR",                  misc::Event::ScanCode::NumericKeypadXOR},
      {                          "LeftAlt",                           misc::Event::ScanCode::LeftAlt},
      {                         "LeftCtrl",                          misc::Event::ScanCode::LeftCtrl},
      {                             "Left",                              misc::Event::ScanCode::Left},
      {                      "LeftBracket",                       misc::Event::ScanCode::LeftBracket},
      {                          "LeftGui",                           misc::Event::ScanCode::LeftGui},
      {                        "LeftShift",                         misc::Event::ScanCode::LeftShift},
      {                      "MediaSelect",                       misc::Event::ScanCode::MediaSelect},
      {                             "Menu",                              misc::Event::ScanCode::Menu},
      {                            "Minus",                             misc::Event::ScanCode::Minus},
      {                       "ModeSwitch",                        misc::Event::ScanCode::ModeSwitch},
      {                             "Mute",                              misc::Event::ScanCode::Mute},
      {                          "NumLock",                           misc::Event::ScanCode::NumLock},
      {                             "Oper",                              misc::Event::ScanCode::Oper},
      {                              "Out",                               misc::Event::ScanCode::Out},
      {                         "PageDown",                          misc::Event::ScanCode::PageDown},
      {                           "PageUp",                            misc::Event::ScanCode::PageUp},
      {                            "Paste",                             misc::Event::ScanCode::Paste},
      {                            "Pause",                             misc::Event::ScanCode::Pause},
      {                           "Period",                            misc::Event::ScanCode::Period},
      {                            "Power",                             misc::Event::ScanCode::Power},
      {                      "PrintScreen",                       misc::Event::ScanCode::PrintScreen},
      {                            "Prior",                             misc::Event::ScanCode::Prior},
      {                         "RightAlt",                          misc::Event::ScanCode::RightAlt},
      {                        "RightCtrl",                         misc::Event::ScanCode::RightCtrl},
      {                            "Enter",                             misc::Event::ScanCode::Enter},
      {                           "Return",                            misc::Event::ScanCode::Return},
      {                         "RightGui",                          misc::Event::ScanCode::RightGui},
      {                            "Right",                             misc::Event::ScanCode::Right},
      {                     "RightBracket",                      misc::Event::ScanCode::RightBracket},
      {                       "RightShift",                        misc::Event::ScanCode::RightShift},
      {                       "ScrollLock",                        misc::Event::ScanCode::ScrollLock},
      {                           "Select",                            misc::Event::ScanCode::Select},
      {                        "Semicolon",                         misc::Event::ScanCode::Semicolon},
      {                        "Separator",                         misc::Event::ScanCode::Separator},
      {                            "Slash",                             misc::Event::ScanCode::Slash},
      {                            "Sleep",                             misc::Event::ScanCode::Sleep},
      {                            "Space",                             misc::Event::ScanCode::Space},
      {                             "Stop",                              misc::Event::ScanCode::Stop},
      {                           "SysReq",                            misc::Event::ScanCode::SysReq},
      {                              "Tab",                               misc::Event::ScanCode::Tab},
      {               "ThousandsSeparator",                misc::Event::ScanCode::ThousandsSeparator},
      {                             "Undo",                              misc::Event::ScanCode::Undo},
      {                               "Up",                                misc::Event::ScanCode::Up},
      {                       "VolumeDown",                        misc::Event::ScanCode::VolumeDown},
      {                         "VolumeUp",                          misc::Event::ScanCode::VolumeUp}
  };
  const auto it = scan_code_map.find(scan_code_string);
  if (it != scan_code_map.end()) {
    return it->second;
  }
  return misc::Event::ScanCode::None;
}

const std::string &from_scan_code(const misc::Event::ScanCode scan_code) {
  static const auto scan_code_map = std::unordered_map<misc::Event::ScanCode, std::string>{
      {                                misc::Event::ScanCode::A,                                 "A"},
      {                                misc::Event::ScanCode::B,                                 "B"},
      {                                misc::Event::ScanCode::C,                                 "C"},
      {                                misc::Event::ScanCode::D,                                 "D"},
      {                                misc::Event::ScanCode::E,                                 "E"},
      {                                misc::Event::ScanCode::F,                                 "F"},
      {                                misc::Event::ScanCode::G,                                 "G"},
      {                                misc::Event::ScanCode::H,                                 "H"},
      {                                misc::Event::ScanCode::I,                                 "I"},
      {                                misc::Event::ScanCode::J,                                 "J"},
      {                                misc::Event::ScanCode::K,                                 "K"},
      {                                misc::Event::ScanCode::L,                                 "L"},
      {                                misc::Event::ScanCode::M,                                 "M"},
      {                                misc::Event::ScanCode::N,                                 "N"},
      {                                misc::Event::ScanCode::O,                                 "O"},
      {                                misc::Event::ScanCode::P,                                 "P"},
      {                                misc::Event::ScanCode::Q,                                 "Q"},
      {                                misc::Event::ScanCode::R,                                 "R"},
      {                                misc::Event::ScanCode::S,                                 "S"},
      {                                misc::Event::ScanCode::T,                                 "T"},
      {                                misc::Event::ScanCode::U,                                 "U"},
      {                                misc::Event::ScanCode::V,                                 "V"},
      {                                misc::Event::ScanCode::W,                                 "W"},
      {                                misc::Event::ScanCode::X,                                 "X"},
      {                                misc::Event::ScanCode::Y,                                 "Y"},
      {                                misc::Event::ScanCode::Z,                                 "Z"},
      {                          misc::Event::ScanCode::Number0,                           "Number0"},
      {                          misc::Event::ScanCode::Number1,                           "Number1"},
      {                          misc::Event::ScanCode::Number2,                           "Number2"},
      {                          misc::Event::ScanCode::Number3,                           "Number3"},
      {                          misc::Event::ScanCode::Number4,                           "Number4"},
      {                          misc::Event::ScanCode::Number5,                           "Number5"},
      {                          misc::Event::ScanCode::Number6,                           "Number6"},
      {                          misc::Event::ScanCode::Number7,                           "Number7"},
      {                          misc::Event::ScanCode::Number8,                           "Number8"},
      {                          misc::Event::ScanCode::Number9,                           "Number9"},
      {                               misc::Event::ScanCode::F1,                                "F1"},
      {                               misc::Event::ScanCode::F2,                                "F2"},
      {                               misc::Event::ScanCode::F3,                                "F3"},
      {                               misc::Event::ScanCode::F4,                                "F4"},
      {                               misc::Event::ScanCode::F5,                                "F5"},
      {                               misc::Event::ScanCode::F6,                                "F6"},
      {                               misc::Event::ScanCode::F7,                                "F7"},
      {                               misc::Event::ScanCode::F8,                                "F8"},
      {                               misc::Event::ScanCode::F9,                                "F9"},
      {                              misc::Event::ScanCode::F10,                               "F10"},
      {                              misc::Event::ScanCode::F11,                               "F11"},
      {                              misc::Event::ScanCode::F12,                               "F12"},
      {                              misc::Event::ScanCode::F13,                               "F13"},
      {                              misc::Event::ScanCode::F14,                               "F14"},
      {                              misc::Event::ScanCode::F15,                               "F15"},
      {                              misc::Event::ScanCode::F16,                               "F16"},
      {                              misc::Event::ScanCode::F17,                               "F17"},
      {                              misc::Event::ScanCode::F18,                               "F18"},
      {                              misc::Event::ScanCode::F19,                               "F19"},
      {                              misc::Event::ScanCode::F20,                               "F20"},
      {                              misc::Event::ScanCode::F21,                               "F21"},
      {                              misc::Event::ScanCode::F22,                               "F22"},
      {                              misc::Event::ScanCode::F23,                               "F23"},
      {                              misc::Event::ScanCode::F24,                               "F24"},
      {     misc::Event::ScanCode::ApplicationControlKeypadBack,      "ApplicationControlKeypadBack"},
      {misc::Event::ScanCode::ApplicationControlKeypadBookmarks, "ApplicationControlKeypadBookmarks"},
      {  misc::Event::ScanCode::ApplicationControlKeypadForward,   "ApplicationControlKeypadForward"},
      {     misc::Event::ScanCode::ApplicationControlKeypadHome,      "ApplicationControlKeypadHome"},
      {  misc::Event::ScanCode::ApplicationControlKeypadRefresh,   "ApplicationControlKeypadRefresh"},
      {   misc::Event::ScanCode::ApplicationControlKeypadSearch,    "ApplicationControlKeypadSearch"},
      {     misc::Event::ScanCode::ApplicationControlKeypadStop,      "ApplicationControlKeypadStop"},
      {                            misc::Event::ScanCode::Again,                             "Again"},
      {                         misc::Event::ScanCode::AltErase,                          "AltErase"},
      {                       misc::Event::ScanCode::Apostrophe,                        "Apostrophe"},
      {                      misc::Event::ScanCode::Application,                       "Application"},
      {                   misc::Event::ScanCode::MediaNextTrack,                    "MediaNextTrack"},
      {                        misc::Event::ScanCode::MediaPlay,                         "MediaPlay"},
      {               misc::Event::ScanCode::MediaPreviousTrack,                "MediaPreviousTrack"},
      {                        misc::Event::ScanCode::MediaStop,                         "MediaStop"},
      {                        misc::Event::ScanCode::Backslash,                         "Backslash"},
      {                        misc::Event::ScanCode::Backspace,                         "Backspace"},
      {                           misc::Event::ScanCode::Cancel,                            "Cancel"},
      {                         misc::Event::ScanCode::CapsLock,                          "CapsLock"},
      {                            misc::Event::ScanCode::Clear,                             "Clear"},
      {                       misc::Event::ScanCode::ClearAgain,                        "ClearAgain"},
      {                            misc::Event::ScanCode::Comma,                             "Comma"},
      {                             misc::Event::ScanCode::Copy,                              "Copy"},
      {                            misc::Event::ScanCode::CrSel,                             "CrSel"},
      {                  misc::Event::ScanCode::CurrencySubUnit,                   "CurrencySubUnit"},
      {                     misc::Event::ScanCode::CurrencyUnit,                      "CurrencyUnit"},
      {                              misc::Event::ScanCode::Cut,                               "Cut"},
      {                 misc::Event::ScanCode::DecimalSeparator,                  "DecimalSeparator"},
      {                           misc::Event::ScanCode::Delete,                            "Delete"},
      {                             misc::Event::ScanCode::Down,                              "Down"},
      {                            misc::Event::ScanCode::Eject,                             "Eject"},
      {                              misc::Event::ScanCode::End,                               "End"},
      {                           misc::Event::ScanCode::Equals,                            "Equals"},
      {                           misc::Event::ScanCode::Escape,                            "Escape"},
      {                          misc::Event::ScanCode::Execute,                           "Execute"},
      {                            misc::Event::ScanCode::ExSel,                             "ExSel"},
      {                             misc::Event::ScanCode::Find,                              "Find"},
      {                            misc::Event::ScanCode::Grave,                             "Grave"},
      {                             misc::Event::ScanCode::Help,                              "Help"},
      {                             misc::Event::ScanCode::Home,                              "Home"},
      {                           misc::Event::ScanCode::Insert,                            "Insert"},
      {                   misc::Event::ScanCode::NumericKeypad0,                    "NumericKeypad0"},
      {                   misc::Event::ScanCode::NumericKeypad1,                    "NumericKeypad1"},
      {                   misc::Event::ScanCode::NumericKeypad2,                    "NumericKeypad2"},
      {                   misc::Event::ScanCode::NumericKeypad3,                    "NumericKeypad3"},
      {                   misc::Event::ScanCode::NumericKeypad4,                    "NumericKeypad4"},
      {                   misc::Event::ScanCode::NumericKeypad5,                    "NumericKeypad5"},
      {                   misc::Event::ScanCode::NumericKeypad6,                    "NumericKeypad6"},
      {                   misc::Event::ScanCode::NumericKeypad7,                    "NumericKeypad7"},
      {                   misc::Event::ScanCode::NumericKeypad8,                    "NumericKeypad8"},
      {                   misc::Event::ScanCode::NumericKeypad9,                    "NumericKeypad9"},
      {                   misc::Event::ScanCode::NumericKeypadA,                    "NumericKeypadA"},
      {                   misc::Event::ScanCode::NumericKeypadB,                    "NumericKeypadB"},
      {                   misc::Event::ScanCode::NumericKeypadC,                    "NumericKeypadC"},
      {                   misc::Event::ScanCode::NumericKeypadD,                    "NumericKeypadD"},
      {                   misc::Event::ScanCode::NumericKeypadE,                    "NumericKeypadE"},
      {                   misc::Event::ScanCode::NumericKeypadF,                    "NumericKeypadF"},
      {                  misc::Event::ScanCode::NumericKeypad00,                   "NumericKeypad00"},
      {                 misc::Event::ScanCode::NumericKeypad000,                  "NumericKeypad000"},
      {           misc::Event::ScanCode::NumericKeypadAmpersand,            "NumericKeypadAmpersand"},
      {                  misc::Event::ScanCode::NumericKeypadAt,                   "NumericKeypadAt"},
      {           misc::Event::ScanCode::NumericKeypadBackspace,            "NumericKeypadBackspace"},
      {              misc::Event::ScanCode::NumericKeypadBinary,               "NumericKeypadBinary"},
      {               misc::Event::ScanCode::NumericKeypadClear,                "NumericKeypadClear"},
      {          misc::Event::ScanCode::NumericKeypadClearEntry,           "NumericKeypadClearEntry"},
      {               misc::Event::ScanCode::NumericKeypadColon,                "NumericKeypadColon"},
      {               misc::Event::ScanCode::NumericKeypadComma,                "NumericKeypadComma"},
      {     misc::Event::ScanCode::NumericKeypadDoubleAmpersand,      "NumericKeypadDoubleAmpersand"},
      {   misc::Event::ScanCode::NumericKeypadDoubleVerticalBar,    "NumericKeypadDoubleVerticalBar"},
      {             misc::Event::ScanCode::NumericKeypadDecimal,              "NumericKeypadDecimal"},
      {              misc::Event::ScanCode::NumericKeypadDivide,               "NumericKeypadDivide"},
      {               misc::Event::ScanCode::NumericKeypadEnter,                "NumericKeypadEnter"},
      {              misc::Event::ScanCode::NumericKeypadEquals,               "NumericKeypadEquals"},
      {         misc::Event::ScanCode::NumericKeypadEqualsAS400,          "NumericKeypadEqualsAS400"},
      {              misc::Event::ScanCode::NumericKeypadExclam,               "NumericKeypadExclam"},
      {             misc::Event::ScanCode::NumericKeypadGreater,              "NumericKeypadGreater"},
      {                misc::Event::ScanCode::NumericKeypadHash,                 "NumericKeypadHash"},
      {         misc::Event::ScanCode::NumericKeypadHexadecimal,          "NumericKeypadHexadecimal"},
      {           misc::Event::ScanCode::NumericKeypadLeftBrace,            "NumericKeypadLeftBrace"},
      {     misc::Event::ScanCode::NumericKeypadLeftParenthesis,      "NumericKeypadLeftParenthesis"},
      {                misc::Event::ScanCode::NumericKeypadLess,                 "NumericKeypadLess"},
      {              misc::Event::ScanCode::NumericKeypadMemAdd,               "NumericKeypadMemAdd"},
      {            misc::Event::ScanCode::NumericKeypadMemClear,             "NumericKeypadMemClear"},
      {           misc::Event::ScanCode::NumericKeypadMemDivide,            "NumericKeypadMemDivide"},
      {         misc::Event::ScanCode::NumericKeypadMemMultiply,          "NumericKeypadMemMultiply"},
      {           misc::Event::ScanCode::NumericKeypadMemRecall,            "NumericKeypadMemRecall"},
      {            misc::Event::ScanCode::NumericKeypadMemStore,             "NumericKeypadMemStore"},
      {         misc::Event::ScanCode::NumericKeypadMemSubtract,          "NumericKeypadMemSubtract"},
      {               misc::Event::ScanCode::NumericKeypadMinus,                "NumericKeypadMinus"},
      {            misc::Event::ScanCode::NumericKeypadMultiply,             "NumericKeypadMultiply"},
      {               misc::Event::ScanCode::NumericKeypadOctal,                "NumericKeypadOctal"},
      {             misc::Event::ScanCode::NumericKeypadPercent,              "NumericKeypadPercent"},
      {              misc::Event::ScanCode::NumericKeypadPeriod,               "NumericKeypadPeriod"},
      {                misc::Event::ScanCode::NumericKeypadPlus,                 "NumericKeypadPlus"},
      {           misc::Event::ScanCode::NumericKeypadPlusMinus,            "NumericKeypadPlusMinus"},
      {               misc::Event::ScanCode::NumericKeypadPower,                "NumericKeypadPower"},
      {          misc::Event::ScanCode::NumericKeypadRightBrace,           "NumericKeypadRightBrace"},
      {    misc::Event::ScanCode::NumericKeypadRightParenthesis,     "NumericKeypadRightParenthesis"},
      {               misc::Event::ScanCode::NumericKeypadSpace,                "NumericKeypadSpace"},
      {                 misc::Event::ScanCode::NumericKeypadTab,                  "NumericKeypadTab"},
      {         misc::Event::ScanCode::NumericKeypadVerticalBar,          "NumericKeypadVerticalBar"},
      {                 misc::Event::ScanCode::NumericKeypadXOR,                  "NumericKeypadXOR"},
      {                          misc::Event::ScanCode::LeftAlt,                           "LeftAlt"},
      {                         misc::Event::ScanCode::LeftCtrl,                          "LeftCtrl"},
      {                             misc::Event::ScanCode::Left,                              "Left"},
      {                      misc::Event::ScanCode::LeftBracket,                       "LeftBracket"},
      {                          misc::Event::ScanCode::LeftGui,                           "LeftGui"},
      {                        misc::Event::ScanCode::LeftShift,                         "LeftShift"},
      {                      misc::Event::ScanCode::MediaSelect,                       "MediaSelect"},
      {                             misc::Event::ScanCode::Menu,                              "Menu"},
      {                            misc::Event::ScanCode::Minus,                             "Minus"},
      {                       misc::Event::ScanCode::ModeSwitch,                        "ModeSwitch"},
      {                             misc::Event::ScanCode::Mute,                              "Mute"},
      {                          misc::Event::ScanCode::NumLock,                           "NumLock"},
      {                             misc::Event::ScanCode::Oper,                              "Oper"},
      {                              misc::Event::ScanCode::Out,                               "Out"},
      {                         misc::Event::ScanCode::PageDown,                          "PageDown"},
      {                           misc::Event::ScanCode::PageUp,                            "PageUp"},
      {                            misc::Event::ScanCode::Paste,                             "Paste"},
      {                            misc::Event::ScanCode::Pause,                             "Pause"},
      {                           misc::Event::ScanCode::Period,                            "Period"},
      {                            misc::Event::ScanCode::Power,                             "Power"},
      {                      misc::Event::ScanCode::PrintScreen,                       "PrintScreen"},
      {                            misc::Event::ScanCode::Prior,                             "Prior"},
      {                         misc::Event::ScanCode::RightAlt,                          "RightAlt"},
      {                        misc::Event::ScanCode::RightCtrl,                         "RightCtrl"},
      {                            misc::Event::ScanCode::Enter,                             "Enter"},
      {                           misc::Event::ScanCode::Return,                            "Return"},
      {                         misc::Event::ScanCode::RightGui,                          "RightGui"},
      {                            misc::Event::ScanCode::Right,                             "Right"},
      {                     misc::Event::ScanCode::RightBracket,                      "RightBracket"},
      {                       misc::Event::ScanCode::RightShift,                        "RightShift"},
      {                       misc::Event::ScanCode::ScrollLock,                        "ScrollLock"},
      {                           misc::Event::ScanCode::Select,                            "Select"},
      {                        misc::Event::ScanCode::Semicolon,                         "Semicolon"},
      {                        misc::Event::ScanCode::Separator,                         "Separator"},
      {                            misc::Event::ScanCode::Slash,                             "Slash"},
      {                            misc::Event::ScanCode::Sleep,                             "Sleep"},
      {                            misc::Event::ScanCode::Space,                             "Space"},
      {                             misc::Event::ScanCode::Stop,                              "Stop"},
      {                           misc::Event::ScanCode::SysReq,                            "SysReq"},
      {                              misc::Event::ScanCode::Tab,                               "Tab"},
      {               misc::Event::ScanCode::ThousandsSeparator,                "ThousandsSeparator"},
      {                             misc::Event::ScanCode::Undo,                              "Undo"},
      {                               misc::Event::ScanCode::Up,                                "Up"},
      {                       misc::Event::ScanCode::VolumeDown,                        "VolumeDown"},
      {                         misc::Event::ScanCode::VolumeUp,                          "VolumeUp"}
  };
  static const auto none_string = std::string{"None"};
  const auto it = scan_code_map.find(scan_code);
  if (it != scan_code_map.end()) {
    return it->second;
  }
  return none_string;
}
} // namespace

misc::Event to_event(const nlohmann::json &json_event) {
  const auto timestamp = json_event.at("timestamp");
  const auto type = from_string(json_event.at("type"));
  auto event = misc::Event(type, timestamp);

  switch (event.type()) {
  case misc::Event::Type::Init: {
    const auto init = json_event.at("init");
    event.init().width = init.at("width");
    event.init().height = init.at("height");
  } break;
  case misc::Event::Type::Quit:
    break;
  case misc::Event::Type::Resize: {
    const auto resize = json_event.at("resize");
    event.resize().width = resize.at("width");
    event.resize().height = resize.at("height");
  } break;
  case misc::Event::Type::Redraw:
    break;
  case misc::Event::Type::MouseButton: {
    const auto mouse_button = json_event.at("mouse_button");
    const auto action = mouse_button.at("action");
    if (action == "Pressed") {
      event.mouse_button().action = misc::Event::Action::Pressed;
    } else if (action == "Released") {
      event.mouse_button().action = misc::Event::Action::Released;
    } else {
      event.mouse_button().action = misc::Event::Action::None;
    }
    const auto button = mouse_button.at("button");
    if (button == "Pressed") {
      event.mouse_button().button = misc::Event::MouseButton::Left;
    } else if (button == "Middle") {
      event.mouse_button().button = misc::Event::MouseButton::Middle;
    } else if (button == "Right") {
      event.mouse_button().button = misc::Event::MouseButton::Right;
    } else {
      event.mouse_button().button = misc::Event::MouseButton::None;
    }
  } break;
  case misc::Event::Type::MouseMove: {
    const auto mouse_move = json_event.at("mouse_move");
    event.mouse_move().x = mouse_move.at("x");
    event.mouse_move().y = mouse_move.at("y");
    event.mouse_move().x_rel = mouse_move.at("x_rel");
    event.mouse_move().y_rel = mouse_move.at("y_rel");
    event.mouse_move().mouse_button_mask = mouse_move.at("mouse_button_mask");
  } break;
  case misc::Event::Type::MouseScroll: {
    const auto mouse_scroll = json_event.at("mouse_scroll");
    event.mouse_scroll().vertical = mouse_scroll.at("vertical");
    event.mouse_scroll().horizontal = mouse_scroll.at("horizontal");
  } break;
  case misc::Event::Type::Key: {
    const auto key = json_event.at("key");
    const auto action = key.at("action");
    if (action == "Pressed") {
      event.key().action = misc::Event::Action::Pressed;
    } else if (action == "Released") {
      event.key().action = misc::Event::Action::Released;
    } else {
      event.key().action = misc::Event::Action::None;
    }
    const auto code = key.at("code");
    event.key().scan_code = to_scan_code(code);
  } break;
  case misc::Event::Type::DragDrop: {
    const auto drag_drop = json_event.at("drag_drop");
    event.drag_drop().filepath = drag_drop.at("filepath");
  } break;
  default:
    throw std::runtime_error("Unhandled event type occurred");
    break;
  }

  return event;
}

nlohmann::json from_event(const misc::Event &event) {
  auto json_event = nlohmann::json{
      {"timestamp", event.timestamp()},
      {     "type",            "None"},
  };

  switch (event.type()) {
  case misc::Event::Type::None:
    json_event["type"] = "None";
    break;
  case misc::Event::Type::Init:
    json_event["type"] = "Init";

    json_event["init"] = nlohmann::json{
        { "width",  event.init().width},
        {"height", event.init().height}
    };
    break;
  case misc::Event::Type::Quit:
    json_event["type"] = "Quit";
    break;
  case misc::Event::Type::Resize:
    json_event["type"] = "Resize";

    json_event["resize"] = nlohmann::json{
        { "width",  event.resize().width},
        {"height", event.resize().height}
    };
    break;
  case misc::Event::Type::Redraw:
    json_event["type"] = "Redraw";
    break;
  case misc::Event::Type::MouseButton: {
    json_event["type"] = "MouseButton";

    auto action = std::string("None");
    switch (event.mouse_button().action) {
    case misc::Event::Action::Pressed:
      action = "Pressed";
      break;
    case misc::Event::Action::Released:
      action = "Released";
      break;
    default:
      break;
    }

    auto button = std::string("None");
    switch (event.mouse_button().button) {
    case misc::Event::MouseButton::Left:
      button = "Left";
      break;
    case misc::Event::MouseButton::Middle:
      button = "Middle";
      break;
    case misc::Event::MouseButton::Right:
      button = "Right";
      break;
    default:
      break;
    }

    json_event["mouse_button"] = nlohmann::json{
        {"action", action},
        {"button", button}
    };
  } break;
  case misc::Event::Type::MouseMove:
    json_event["type"] = "MouseMove";

    json_event["mouse_move"] = nlohmann::json{
        {                "x",                 event.mouse_move().x},
        {                "y",                 event.mouse_move().y},
        {            "x_rel",             event.mouse_move().x_rel},
        {            "y_rel",             event.mouse_move().y_rel},
        {"mouse_button_mask", event.mouse_move().mouse_button_mask}
    };
    break;
  case misc::Event::Type::MouseScroll:
    json_event["type"] = "MouseScroll";

    json_event["mouse_scroll"] = nlohmann::json{
        {  "vertical",   event.mouse_scroll().vertical},
        {"horizontal", event.mouse_scroll().horizontal}
    };
    break;
  case misc::Event::Type::Key: {
    json_event["type"] = "Key";

    auto action = std::string("None");
    switch (event.key().action) {
    case misc::Event::Action::Pressed:
      action = "Pressed";
      break;
    case misc::Event::Action::Released:
      action = "Released";
      break;
    default:
      break;
    }

    auto code = from_scan_code(event.key().scan_code);

    json_event["key"] = nlohmann::json{
        {"action", action},
        {  "code",   code}
    };
  } break;
  case misc::Event::Type::DragDrop:
    json_event["type"] = "DragDrop";

    json_event["drag_drop"] = nlohmann::json{
        {"filepath", event.drag_drop().filepath}
    };
    break;
  default:
    throw std::runtime_error("Unhandled event type occurred");
    break;
  }

  return json_event;
}
} // namespace gp::json
