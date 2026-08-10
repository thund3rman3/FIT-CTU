/**
 * @file    curve.h
 * @author  Josef Jech
 * @date    15.05.2023
 */


#pragma once
#include "pgr.h"

// Checks whether vector is zero-length or not.
bool isVectorNull(const glm::vec3& vect);

/**
 * Align (rotate and move) current coordinate system to given parameters.
 * @param  position - Position of the origin.
 * @param  front - Front direction.
 * @param  up - Up vector.
 */
glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);

extern glm::vec3 curveData[];
extern const size_t  curveSize;

/**
* Cyclic clamping of a value.
* Makes sure that \a value is not outside the interval [\a minBound, \a maxBound].
* If \a value is outside the interval it is treated as a periodic value with the period equal to the size
* of the interval. A necessary number of periods are added/subtracted to fit the value to the interval.
*
* @param  value              Value to be clamped.
* @param  minBound           Minimum bound of value.
* @param  maxBound           Maximum bound of value.
* @return                    Value within range [minBound, maxBound].
*/
template <typename T>
T cyclic_clamp(const T value, const T minBound, const T maxBound) {

    T amp = maxBound - minBound;
    T val = fmod(value - minBound, amp);

    if (val < T(0))
        val += amp;

    return val + minBound;
}

/**
* Evaluates a position on Catmull-Rom curve segment.
* @param P0       First control point of the curve segment.
* @param P1       Second control point of the curve segment.
* @param P2       Third control point of the curve segment.
* @param P3       Fourth control point of the curve segment.
* @param t        Curve segment parameter. Must be within range [0, 1].
* @return         Position on the curve for parameter \a t.
*/
glm::vec3 evaluateCurveSegment(
    const glm::vec3& P0,
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const float t
);

/**
* Evaluates a fist derivative of Catmull-Rom curve segment.
* @param P0       First control point of the curve segment.
* @param P1       Second control point of the curve segment.
* @param P2       Third control point of the curve segment.
* @param P3       Founrth control point of the curve segment.
* @param t        Curve segment parameter. Must be within range [0, 1].
* @return             First derivative of the curve for parameter \a t.
*/
glm::vec3 evaluateCurveSegment_1stDerivative(
    const glm::vec3& P0,
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const float t
);

/**
* Evaluates a position on a closed curve composed of Catmull-Rom segments.
* @param points   Array of curve control points.
* @param count    Number of curve control points.
* @param t        Parameter for which the position shall be evaluated.
* @return             Position on the curve for parameter \a t.
* @note               Although the range of the paramter is from [0, \a count] (outside the range
                      the curve is periodic) one must presume any value (even negative).
*/
glm::vec3 evaluateClosedCurve(
    const glm::vec3 points[],
    const size_t    count,
    const float     t
);

/**
* Evaluates a first derivative of a closed curve composed of Catmull-Rom segments.
* @param points   Array of curve control points.
* @param count    Number of curve control points.
* @param t        Parameter for which the derivative shall be evaluated.
* @return             First derivative of the curve for parameter \a t.
* @note               Although the range of the paramter is from [0, \a count] (outside the range
                      the curve is periodic) one must presume any value (even negative).
*/
glm::vec3 evaluateClosedCurve_1stDerivative(
    const glm::vec3 points[],
    const size_t    count,
    const float     t
);


