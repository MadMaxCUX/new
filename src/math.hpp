#pragma once
#include <cmath>
#include <algorithm>

struct Vec2 { float x = 0.f, y = 0.f; };

struct Vec3 {
    float x = 0.f, y = 0.f, z = 0.f;
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator*(float s)      const { return {x * s,   y * s,   z * s  }; }
    float dot(const Vec3& o)     const { return x*o.x + y*o.y + z*o.z; }
    float length()               const { return std::sqrt(dot(*this)); }
    float length2d()             const { return std::sqrt(x*x + y*y); }
    bool  valid()                const {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
};

struct ViewMatrix { float m[4][4]{}; };

inline bool WorldToScreen(const ViewMatrix& v, const Vec3& w,
                          int sw, int sh, Vec2& out) {
    float cx = w.x * v.m[0][0] + w.y * v.m[0][1] + w.z * v.m[0][2] + v.m[0][3];
    float cy = w.x * v.m[1][0] + w.y * v.m[1][1] + w.z * v.m[1][2] + v.m[1][3];
    float cw = w.x * v.m[3][0] + w.y * v.m[3][1] + w.z * v.m[3][2] + v.m[3][3];
    if (cw < 0.1f) return false;
    float inv = 1.f / cw;
    float ndcX = cx * inv;
    float ndcY = cy * inv;
    out.x = (sw * 0.5f) + (sw * 0.5f) * ndcX;
    out.y = (sh * 0.5f) - (sh * 0.5f) * ndcY;
    return std::isfinite(out.x) && std::isfinite(out.y);
}

inline Vec3 CalcAngle(const Vec3& from, const Vec3& to) {
    Vec3 d = to - from;
    float hyp = std::sqrt(d.x*d.x + d.y*d.y);
    Vec3 a;
    a.x = std::atan2(-d.z, hyp) * 57.2957795f;
    a.y = std::atan2( d.y, d.x) * 57.2957795f;
    a.z = 0.f;
    return a;
}

inline float NormalizeYaw(float a) {
    while (a >  180.f) a -= 360.f;
    while (a < -180.f) a += 360.f;
    return a;
}
