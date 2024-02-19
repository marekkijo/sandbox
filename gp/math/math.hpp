#pragma once

#include <limits>
#include <tuple>

namespace gp::math {
/// source: https://stackoverflow.com/a/32334103/1113279
bool nearly_equal(float a,
                  float b,
                  float epsilon = 128.0f * std::numeric_limits<float>::epsilon(),
                  float abs_th = std::numeric_limits<float>::min());

/// source: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool do_intersect(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y);

/// source: https://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/?ref=gcse
std::tuple<bool, float, float>
intersection_point(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y);

/// source: https://stackoverflow.com/a/11980362/1113279
float wrap_angle(float angle);
} // namespace gp::math
