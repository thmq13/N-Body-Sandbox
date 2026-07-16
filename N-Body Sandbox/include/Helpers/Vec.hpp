#pragma once

#include <cmath>
#include <iostream>
   
struct Vec3;

constexpr Vec3 operator+(Vec3 lhs, const Vec3& rhs) noexcept;
constexpr Vec3 operator-(Vec3 lhs, const Vec3& rhs) noexcept;
constexpr Vec3 operator*(Vec3 lhs, double scalar) noexcept;
constexpr Vec3 operator*(double scalar, Vec3 rhs) noexcept;
constexpr Vec3 operator/(Vec3 lhs, double scalar) noexcept;

struct Vec3 {
    double x{ 0.0 };
    double y{ 0.0 };
    double z{ 0.0 };

    constexpr Vec3() noexcept = default;

    constexpr Vec3(double x_val, double y_val, double z_val) noexcept
        : x(x_val), y(y_val), z(z_val) {
    }

    constexpr explicit Vec3(double scalar) noexcept
        : x(scalar), y(scalar), z(scalar) {
    }

    constexpr Vec3 operator-() const noexcept { return Vec3{ -x, -y, -z }; }

    constexpr Vec3& operator+=(const Vec3& other) noexcept {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    constexpr Vec3& operator-=(const Vec3& other) noexcept {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    constexpr Vec3& operator*=(double scalar) noexcept {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    constexpr Vec3& operator/=(double scalar) noexcept {
        double inv = 1.0 / scalar;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }


    constexpr double length_squared() const noexcept {
        return x * x + y * y + z * z;
    }

    double length() const noexcept {
        return std::sqrt(length_squared());
    }

    constexpr double dot(const Vec3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr Vec3 cross(const Vec3& other) const noexcept {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    Vec3 normalized() const noexcept {
        double len = length();
        if (len == 0.0) return { 0.0, 0.0, 0.0 }; 
        return *this / len;
    }

    Vec3& normalize() noexcept {
        double len = length();
        if (len != 0.0) {
            *this /= len;
        }
        return *this;
    }
};

constexpr Vec3 operator+(Vec3 lhs, const Vec3& rhs) noexcept { return lhs += rhs; }
constexpr Vec3 operator-(Vec3 lhs, const Vec3& rhs) noexcept { return lhs -= rhs; }
constexpr Vec3 operator*(Vec3 lhs, double scalar) noexcept { return lhs *= scalar; }
constexpr Vec3 operator*(double scalar, Vec3 rhs) noexcept { return rhs *= scalar; }
constexpr Vec3 operator/(Vec3 lhs, double scalar) noexcept { return lhs /= scalar; }

constexpr bool operator==(const Vec3& lhs, const Vec3& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
constexpr bool operator!=(const Vec3& lhs, const Vec3& rhs) noexcept { return !(lhs == rhs); }

inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}