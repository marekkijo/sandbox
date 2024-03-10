#pragma once

#include <limits>
#include <tuple>

namespace gp::math {
/**
 * @brief Checks if two floating-point numbers are nearly equal within a given epsilon.
 *
 * This function compares two floating-point numbers `a` and `b` and returns true if they are nearly equal,
 * within a given epsilon. The epsilon value determines the maximum allowed difference between `a` and `b`
 * for them to be considered nearly equal.
 *
 * @param a The first floating-point number to compare.
 * @param b The second floating-point number to compare.
 * @param epsilon The maximum allowed difference between `a` and `b` for them to be considered nearly equal.
 *               The default value is 128.0f times the machine epsilon for the `float` type.
 * @param abs_th The minimum absolute threshold for `a` and `b` to be considered nearly equal. If the absolute
 *               value of `a` and `b` is less than `abs_th`, they are considered equal regardless of the epsilon value.
 *               The default value is the minimum positive normalized value for the `float` type.
 * @return True if `a` and `b` are nearly equal within the given epsilon, false otherwise.
 *
 * @see https://stackoverflow.com/a/32334103/1113279
 */
bool nearly_equal(float a,
                  float b,
                  float epsilon = 128.0f * std::numeric_limits<float>::epsilon(),
                  float abs_th = std::numeric_limits<float>::min());

/**
 * Checks if two line segments intersect.
 *
 * @param a_x The x-coordinate of the first endpoint of the first line segment.
 * @param a_y The y-coordinate of the first endpoint of the first line segment.
 * @param b_x The x-coordinate of the second endpoint of the first line segment.
 * @param b_y The y-coordinate of the second endpoint of the first line segment.
 * @param c_x The x-coordinate of the first endpoint of the second line segment.
 * @param c_y The y-coordinate of the first endpoint of the second line segment.
 * @param d_x The x-coordinate of the second endpoint of the second line segment.
 * @param d_y The y-coordinate of the second endpoint of the second line segment.
 * @return True if the line segments intersect, false otherwise.
 *
 * @see https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect
 */
bool do_intersect(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y);

/**
 * Calculates the intersection point between two line segments.
 *
 * @param a_x The x-coordinate of the first endpoint of the first line segment.
 * @param a_y The y-coordinate of the first endpoint of the first line segment.
 * @param b_x The x-coordinate of the second endpoint of the first line segment.
 * @param b_y The y-coordinate of the second endpoint of the first line segment.
 * @param c_x The x-coordinate of the first endpoint of the second line segment.
 * @param c_y The y-coordinate of the first endpoint of the second line segment.
 * @param d_x The x-coordinate of the second endpoint of the second line segment.
 * @param d_y The y-coordinate of the second endpoint of the second line segment.
 * @return A tuple containing three values:
 *         - A boolean indicating whether the line segments intersect.
 *         - The x-coordinate of the intersection point (if it exists).
 *         - The y-coordinate of the intersection point (if it exists).
 *
 * @see https://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines
 */
std::tuple<bool, float, float>
intersection_point(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y);

/**
 * Wraps an angle to the range [-pi, pi].
 *
 * @param angle The angle to be wrapped.
 * @return The wrapped angle.
 *
 * @see https://stackoverflow.com/a/11980362/1113279
 */
float wrap_angle(float angle);
} // namespace gp::math
