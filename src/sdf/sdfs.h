#pragma once

#include <cmath>  // std::fabs
#include <algorithm>  //std::min std::max
#include <limits>

// #include "../../olc/olcPixelGameEngine.h"
#include <glm/glm.hpp>

// https://iquilezles.org/articles/distfunctions2d/

namespace sdf {


inline glm::vec2 abs(const glm::vec2& v) {
  return glm::vec2(std::fabs(v.x), std::fabs(v.y));
}

inline glm::vec2 max(const glm::vec2& lhs, const glm::vec2& rhs) {
  return glm::vec2(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y));
}

inline glm::vec2 min(const glm::vec2& lhs, const glm::vec2& rhs) {
  return glm::vec2(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y));
}


// Minimum Signed Distance to a Box
// r is half the box size, something like a radius from the center(0, 0) of the box to its sides
inline float box(const glm::vec2& p, const glm::vec2& r) {
    glm::vec2 d = abs(p) - r;
    return glm::length(max(d, glm::vec2(0.0f))) + std::min(std::max(d.x, d.y), 0.0f);
}


inline float box_max(const glm::vec2& p, const glm::vec2& r) {
  const glm::vec2 verts[4] = {
    {-r.x, -r.y},
    { r.x, -r.y},
    {-r.x,  r.y},
    { r.x,  r.y}
  };

  float sd = -std::numeric_limits<float>::max();  
  for(const glm::vec2& v : verts)
    sd = std::max(sd, glm::length(p - v));

  return sd;
}


} // namespace sdf
