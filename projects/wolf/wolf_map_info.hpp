#pragma once

#include <cstdint>

namespace wolf::Map {
enum class Walls : std::uint16_t {
  nothing = 0,

  grey_brick_1      = 1,
  grey_brick_2      = 2,
  grey_brick_3      = 27,
  grey_brick_flag   = 3,
  grey_brick_hitler = 4,
  grey_brick_eagle  = 6,
  grey_brick_sign   = 28,

  dirty_brick_1 = 24,
  dirty_brick_2 = 26,

  cell          = 5,
  cell_skeleton = 7,

  blue_brick_1    = 8,
  blue_brick_2    = 9,
  blue_brick_sign = 41,

  wood            = 12,
  wood_eagle      = 10,
  wood_hitler     = 11,
  wood_iron_cross = 23,

  entrance_to_level = 13,

  steel      = 15,
  steel_sign = 14,

  landscape = 16,

  red_brick          = 17,
  red_brick_swastika = 18,
  red_brick_flag     = 20,

  multicolor_brick = 38,

  purple       = 19,
  purple_blood = 25,

  brown_weave         = 29,
  brown_weave_blood_1 = 32,
  brown_weave_blood_2 = 30,
  brown_weave_blood_3 = 31,

  stained_glass = 33,

  grey_wall_1      = 35,
  grey_wall_2      = 39,
  grey_wall_hitler = 49,
  grey_wall_map    = 43,
  grey_wall_vent   = 37,

  blue_wall          = 40,
  blue_wall_swastika = 36,
  blue_wall_skull    = 34,

  brown_stone_1     = 44,
  brown_stone_2     = 45,
  brown_marble_1    = 42,
  brown_marble_2    = 46,
  brown_marble_flag = 47,

  wood_panel = 48,

  fake_door        = 50,
  door_excavation  = 51,
  fake_locked_door = 53,

  elevator_wall = 85,
  elevator      = 21,
  fake_elevator = 22,

  door_vertical   = 90,
  door_horizontal = 91,

  door_vertical_gold_key   = 92,
  door_horizontal_gold_key = 93,

  door_vertical_silver_key   = 94,
  door_horizontal_silver_key = 95,

  elevator_door_vertical   = 100,
  elevator_door_horizontal = 101,

  floor_deaf_guard         = 106,
  elevator_to_secret_floor = 107,

  floor_6c = 108,
  floor_6d = 109,
  floor_6e = 110,
  floor_6f = 111,
  floor_70 = 112,
  floor_71 = 113,
  floor_72 = 114,
  floor_73 = 115,
  floor_74 = 116,
  floor_75 = 117,
  floor_76 = 118,
  floor_77 = 119,
  floor_78 = 120,
  floor_79 = 121,
  floor_7a = 122,
  floor_7b = 123,
  floor_7c = 124,
  floor_7d = 125,
  floor_7e = 126,
  floor_7f = 127,
  floor_80 = 128,
  floor_81 = 129,
  floor_82 = 130,
  floor_83 = 131,
  floor_84 = 132,
  floor_85 = 133,
  floor_86 = 134,
  floor_87 = 135,
  floor_88 = 136,
  floor_89 = 137,
  floor_8a = 138,
  floor_8b = 139,
  floor_8c = 140,
  floor_8d = 141,
  floor_8e = 142,
  floor_8f = 143
};

enum class Objects : std::uint16_t {
  nothing = 0,

  start_position_n = 19,
  start_position_e = 20,
  start_position_s = 21,
  start_position_w = 22,

  secret_door = 98,

  gold_key   = 43,
  silver_key = 44,

  cross   = 52,
  chalace = 53,
  jewels  = 54,
  crown   = 55,

  dog_food      = 29,
  food          = 47,
  first_aid_kit = 48,
  extra_life    = 56,

  small_gun   = 50,
  machine_gun = 51,
  ammo        = 49,

  chandelier    = 27,
  ceiling_light = 37,

  floor_lamp        = 26,
  well              = 60,
  well_water        = 59,
  water             = 23,
  pool_of_blood     = 61,
  barrel            = 58,
  oil_drum          = 24,
  basket            = 46,
  table_with_chairs = 25,
  table             = 36,
  white_column      = 30,
  green_plant       = 31,
  brown_plant       = 34,
  vase              = 35,
  armor             = 39,
  flag              = 62,
  sink              = 33,

  utensils_brown = 38,
  utensils_blue  = 67,

  stove = 68,
  rack  = 69,
  bed   = 45,

  cage_empty    = 40,
  cage_skeleton = 41,

  skeleton_hanging = 28,
  skeleton         = 32,

  bones_1     = 42,
  bones_2     = 64,
  bones_3     = 65,
  bones_4     = 66,
  bones_blood = 57,

  aardwolf_sign = 63,
  vines         = 70,

  guard_1_standing_e = 108,
  guard_1_standing_n = 109,
  guard_1_standing_w = 110,
  guard_1_standing_s = 111,

  ss_1_standing_e = 126,
  ss_1_standing_n = 127,
  ss_1_standing_w = 128,
  ss_1_standing_s = 129,

  officer_1_standing_e = 116,
  officer_1_standing_n = 117,
  officer_1_standing_w = 118,
  officer_1_standing_s = 119,

  mutant_1_standing_e = 216,
  mutant_1_standing_n = 217,
  mutant_1_standing_w = 218,
  mutant_1_standing_s = 219,

  guard_3_standing_e = 144,
  guard_3_standing_n = 145,
  guard_3_standing_w = 146,
  guard_3_standing_s = 147,

  ss_3_standing_e = 162,
  ss_3_standing_n = 163,
  ss_3_standing_w = 164,
  ss_3_standing_s = 165,

  officer_3_standing_e = 152,
  officer_3_standing_n = 153,
  officer_3_standing_w = 154,
  officer_3_standing_s = 155,

  mutant_3_standing_e = 234,
  mutant_3_standing_n = 235,
  mutant_3_standing_w = 236,
  mutant_3_standing_s = 237,

  guard_4_standing_e = 180,
  guard_4_standing_n = 181,
  guard_4_standing_w = 182,
  guard_4_standing_s = 183,

  ss_4_standing_e = 198,
  ss_4_standing_n = 199,
  ss_4_standing_w = 200,
  ss_4_standing_s = 201,

  officer_4_standing_e = 188,
  officer_4_standing_n = 189,
  officer_4_standing_w = 190,
  officer_4_standing_s = 191,

  mutant_4_standing_e = 252,
  mutant_4_standing_n = 253,
  mutant_4_standing_w = 254,
  mutant_4_standing_s = 255,

  dog_1_moving_e = 138,
  dog_1_moving_n = 139,
  dog_1_moving_w = 140,
  dog_1_moving_s = 141,

  guard_1_moving_e = 112,
  guard_1_moving_n = 113,
  guard_1_moving_w = 114,
  guard_1_moving_s = 115,

  ss_1_moving_e = 130,
  ss_1_moving_n = 131,
  ss_1_moving_w = 132,
  ss_1_moving_s = 133,

  officer_1_moving_e = 120,
  officer_1_moving_n = 121,
  officer_1_moving_w = 122,
  officer_1_moving_s = 123,

  mutant_1_moving_e = 220,
  mutant_1_moving_n = 221,
  mutant_1_moving_w = 222,
  mutant_1_moving_s = 223,

  dog_3_moving_e = 174,
  dog_3_moving_n = 175,
  dog_3_moving_w = 176,
  dog_3_moving_s = 177,

  guard_3_moving_e = 148,
  guard_3_moving_n = 149,
  guard_3_moving_w = 150,
  guard_3_moving_s = 151,

  ss_3_moving_e = 166,
  ss_3_moving_n = 167,
  ss_3_moving_w = 168,
  ss_3_moving_s = 169,

  officer_3_moving_e = 156,
  officer_3_moving_n = 157,
  officer_3_moving_w = 158,
  officer_3_moving_s = 159,

  mutant_3_moving_e = 238,
  mutant_3_moving_n = 239,
  mutant_3_moving_w = 240,
  mutant_3_moving_s = 241,

  dog_4_moving_e = 210,
  dog_4_moving_n = 211,
  dog_4_moving_w = 212,
  dog_4_moving_s = 213,

  guard_4_moving_e = 184,
  guard_4_moving_n = 185,
  guard_4_moving_w = 186,
  guard_4_moving_s = 187,

  ss_4_moving_e = 202,
  ss_4_moving_n = 203,
  ss_4_moving_w = 204,
  ss_4_moving_s = 205,

  officer_4_moving_e = 192,
  officer_4_moving_n = 193,
  officer_4_moving_w = 194,
  officer_4_moving_s = 195,

  mutant_4_moving_e = 256,
  mutant_4_moving_n = 257,
  mutant_4_moving_w = 258,
  mutant_4_moving_s = 259,

  dead_guard = 124,

  red_pacman    = 224,
  yellow_pacman = 225,
  rose_pacman   = 226,
  blue_pacman   = 227,

  hans_grosse         = 214,
  dr_schabbs          = 196,
  robed_hitler        = 160,
  hitler              = 178,
  otto                = 215,
  gretel_grosse       = 197,
  general_fettgesicht = 179,

  turning_point_e  = 90,
  turning_point_ne = 91,
  turning_point_n  = 92,
  turning_point_nw = 93,
  turning_point_w  = 94,
  turning_point_sw = 95,
  turning_point_s  = 96,
  turning_point_se = 97,

  endgame_trigger = 99,
};

enum class Extra : std::uint16_t { nothing = 0 };
} // namespace wolf::Map
