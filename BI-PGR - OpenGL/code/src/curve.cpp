/**
 * @file    curve.cpp
 * @author  Josef Jech
 * @date    15.05.2023
 */

#include "curve.h"
#include <iostream>

bool isVectorNull(const glm::vec3& vect) {

    return !vect.x && !vect.y && !vect.z;
}

glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) {

    glm::vec3 z = -glm::normalize(front);

    if (isVectorNull(z))
        z = glm::vec3(0.0, 0.0, 1.0);

    glm::vec3 x = glm::normalize(glm::cross(up, z));

    if (isVectorNull(x))
        x = glm::vec3(1.0, 0.0, 0.0);

    glm::vec3 y = glm::cross(z, x);
    //mat4 matrix = mat4(1.0f);
    glm::mat4 matrix = glm::mat4(
        x.x, x.y, x.z, 0.0,
        y.x, y.y, y.z, 0.0,
        z.x, z.y, z.z, 0.0,
        position.x, position.y, position.z, 1.0
    );

    return matrix;
}

const size_t  curveSize = 6;

glm::vec3 curveData[] = {
  glm::vec3(255.0, -104.0,  13.0),
  glm::vec3(119.0,  -67.0, 30.0),
  glm::vec3(-45.0,  -51.0, 10.0),
  glm::vec3(-74.0,  -149.0, 19.0),
  glm::vec3(25.6,  -149.5, 10.0),
  glm::vec3(154.0,  -188.0, 19.0)
};

glm::vec3 evaluateCurveSegment(
    const glm::vec3& P0,
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const float t) 
{
    glm::vec3 result(0.0, 0.0, 0.0);
    const float t2 = t * t;
    const float t3 = t * t2;
    result = P0 * (-t3 + 2.0f * t2 - t)
        + P1 * (3.0f * t3 - 5.0f * t2 + 2.0f)
        + P2 * (-3.0f * t3 + 4.0f * t2 + t)
        + P3 * (t3 - t2);
    result *= 0.5f;
    return result;
}

glm::vec3 evaluateCurveSegment_1stDerivative(
    const glm::vec3& P0,
    const glm::vec3& P1,
    const glm::vec3& P2,
    const glm::vec3& P3,
    const float t) 
{
    glm::vec3 result(1.0, 0.0, 0.0);
    const float t2 = t * t;
    result = P0 * (-3.0f * t2 + 4.0f * t - 1.0f)
        + P1 * (9.0f * t2 - 10.0f * t)
        + P2 * (-9.0f * t2 + 8.0f * t + 1.0f)
        + P3 * (3.0f * t2 - 2.0f * t);

    result *= 0.5f;
    return result;
}

glm::vec3 evaluateClosedCurve(
    const glm::vec3 points[],
    const size_t    count,
    const float     t) 
{
    glm::vec3 result(0.0, 0.0, 0.0);
    float param = cyclic_clamp(t, 0.0f, float(count));
    size_t index = size_t(param);

    result = evaluateCurveSegment(
        points[(index - 1 + count) % count],
        points[(index) % count],
        points[(index + 1) % count],
        points[(index + 2) % count],
        param - floor(param));
    return result;
}

glm::vec3 evaluateClosedCurve_1stDerivative(
    const glm::vec3 points[],
    const size_t    count,
    const float     t) 
{
    glm::vec3 result(1.0, 0.0, 0.0);
    float param = cyclic_clamp(t, 0.0f, float(count));
    size_t index = size_t(param);

    result = evaluateCurveSegment_1stDerivative(
        points[(index - 1 + count) % count],
        points[(index) % count],
        points[(index + 1) % count],
        points[(index + 2) % count],
        param - floor(param));
    return result;
}

