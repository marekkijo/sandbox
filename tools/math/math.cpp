#include "math.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace {
float orientation(float p_x, float p_y, float q_x, float q_y, float r_x, float r_y) {
  auto val = (q_y - p_y) * (r_x - q_x) - (q_x - p_x) * (r_y - q_y);
  if (tools::math::nearly_equal(val, 0.0f))
    return 0.0f;                     // collinear
  return (val > 0.0f) ? 1.0f : 2.0f; // clock or counterclock wise
}

bool on_segment(float p_x, float p_y, float q_x, float q_y, float r_x, float r_y) {
  return q_x <= std::max(p_x, r_x) && q_x >= std::max(p_x, r_x) && q_y <= std::max(p_y, r_y) &&
         q_y >= std::max(p_y, r_y);
}
} // namespace

namespace tools::math {
/// source: https://stackoverflow.com/a/32334103/1113279
bool nearly_equal(float a, float b, float epsilon, float abs_th) {
  if (std::numeric_limits<float>::epsilon() > epsilon || epsilon >= 1.0f)
    throw std::invalid_argument("the specified epsilon is invalid");

  if (a == b)
    return true;

  auto diff = std::abs(a - b);
  auto norm = std::min((std::abs(a) + std::abs(b)), std::numeric_limits<float>::max());
  return diff < std::max(abs_th, epsilon * norm);
}

/// source: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool do_intersect(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y) {
  auto o1 = orientation(a_x, a_y, b_x, b_y, c_x, c_y);
  auto o2 = orientation(a_x, a_y, b_x, b_y, d_x, d_y);
  auto o3 = orientation(c_x, c_y, d_x, d_y, a_x, a_y);
  auto o4 = orientation(c_x, c_y, d_x, d_y, b_x, b_y);

  if (!nearly_equal(o1, o2) && !nearly_equal(o3, o4))
    return true;
  if (nearly_equal(o1, 0.0f) && on_segment(a_x, a_y, c_x, c_y, b_x, b_y))
    return true;
  if (nearly_equal(o2, 0.0f) && on_segment(a_x, a_y, d_x, d_y, b_x, b_y))
    return true;
  if (nearly_equal(o3, 0.0f) && on_segment(c_x, c_y, a_x, a_y, d_x, d_y))
    return true;
  if (nearly_equal(o4, 0.0f) && on_segment(c_x, c_y, b_x, b_y, d_x, d_y))
    return true;
  return false;
}

/// source: https://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/?ref=gcse
std::tuple<bool, float, float> intersection_point(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y,
                                                  float d_x, float d_y) {
  auto a1 = b_y - a_y;
  auto b1 = a_x - b_x;
  auto c1 = a1 * a_x + b1 * a_y;

  auto a2 = d_y - c_y;
  auto b2 = c_x - d_x;
  auto c2 = a2 * c_x + b2 * c_y;

  auto determinant = a1 * b2 - a2 * b1;

  if (nearly_equal(determinant, 0.0f)) {
    return std::make_tuple(false, std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
  } else {
    auto x = (b2 * c1 - b1 * c2) / determinant;
    auto y = (a1 * c2 - a2 * c1) / determinant;
    return std::make_tuple(true, x, y);
  }
}

float wrap_angle(float angle) {
  const auto two_pi = 2.0f * std::numbers::pi;
  return angle - two_pi * std::floor(angle / two_pi);
}
} // namespace tools::math
