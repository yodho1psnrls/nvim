#pragma once
#include <glm/vec2.hpp>
#include <ostream>


struct Vertex {
private:
  using vec2 = glm::vec2;

public:
  vec2 pos;
  vec2 uv;

  //vec2 tan;
  //vec2 bitan;

  //Vertex() : pos(0.0f, 0.0f), uv(0.0f, 0.0f) {}
  //Vertex(const vec2& Pos) : pos(Pos), uv(0.0f, 0.0f) {}

  Vertex(const vec2& pos = vec2(0.0f, 0.0f), const vec2& uv = vec2(0.0f, 0.0f))
    : pos(pos), uv(uv) {}


  //operator vec2& () { return pos; }
  //operator const vec2& () const { return pos; }
  //operator vi2d () const { return pos; }


  // Component/Attribute wise operations on vertices

  Vertex& operator+=(const Vertex& other) {
    pos += other.pos;
    uv += other.uv;

    return *this;
  }

  Vertex& operator-=(const Vertex& other) {
    pos -= other.pos;
    uv -= other.uv;

    return *this;
  }

  Vertex& operator*=(const float& scalar) {
    pos *= scalar;
    uv *= scalar;

    return *this;
  }

  Vertex& operator/=(const float& scalar) {
    pos /= scalar;
    uv /= scalar;

    return *this;
  }


  Vertex operator+(const Vertex& rhs) const {
    Vertex v(*this);
    return v += rhs;
  }

  Vertex operator-(const Vertex& rhs) const {
    Vertex v(*this);
    return v -= rhs;
  }

  Vertex operator*(const float& scalar) const {
    Vertex v(*this);
    return v *= scalar;
  }

  Vertex operator/(const float& scalar) const {
    Vertex v(*this);
    return v /= scalar;
  }

  /*friend Vertex operator*(const float& scalar, const Vertex& vert) {  // static // friend // constexpr
    Vertex v(vert);
    return v *= scalar;
  }*/

};

//Vertex operator*(const float& scalar, Vertex vert) { return vert *= scalar; }
inline Vertex operator*(const float& scalar, const Vertex& vert) {
  Vertex v(vert);
  return v *= scalar;
}


inline std::ostream& operator<<(std::ostream& stream, const Vertex& v) {
  stream << "(" << v.pos << ", " << v.uv << ")";
  return stream;
}
