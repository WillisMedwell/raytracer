#ifndef _LINEAR_ALGEBRA_H_
#define _LINEAR_ALGEBRA_H_

#include <iostream>
#include <math.h>

constexpr static auto MOE = 0.00001;     
constexpr static auto PI  = 3.1415926535;

/*
    Floating point operations are never perfect so there are rounding issues. 
    This function allows an easy way to have a margin of error.
*/ 
inline auto margin_of_error(const float& cmp, const float& value) -> bool {
    return ((cmp < value + MOE) && (cmp > value - MOE)) ? true : false;;
}
inline auto round(const float& cmp,const float& value, const float& moe = MOE) {
    return (value < (moe + cmp) )&& (value > (cmp-moe)) ? cmp : value;
}


struct vec3 {
    float i, j, k;

    void print() const {
        std::cout << "{ " << i << ", " << j << ", " << k << " }\n";
    }
    friend vec3 operator+(vec3 lhs, const vec3& rhs) {
        return vec3{
            lhs.i + rhs.i,
            lhs.j + rhs.j,
            lhs.k + rhs.k
        };
    }
    friend vec3 operator-(vec3 lhs, const vec3& rhs) {
        return vec3{
            lhs.i - rhs.i,
            lhs.j - rhs.j,
            lhs.k - rhs.k
        };
    }
};

inline auto magnitude(const vec3& v) -> float {
    return sqrt(v.i*v.i + v.j*v.j + v.k*v.k);
}
inline auto length_sqaured(const vec3& v) -> float {
    return v.i*v.i + v.j*v.j + v.k*v.k;
}

struct uvec3
{
    float i, j, k;
    inline uvec3(const float& i_, const float& j_, const float& k_) {
        const auto mag = magnitude(vec3{i_, j_, k_ });
        i = i_ / mag;
        j = j_ / mag;
        k = k_ / mag;
    }
    inline uvec3(const vec3& v) {
        const auto mag = magnitude(v);
        i = v.i / mag;
        j = v.j / mag;
        k = v.k / mag;
    }
    void print() const {
        std::cout << "{ " << i << ", " << j << ", " << k << " }\n";
    }
};

inline constexpr auto magnitude(const uvec3& v) -> float {
    return 1.0f;
}


struct pt3{
    float x, y, z;
    pt3(const float& x_, const float& y_, const float& z_)
        : x(x_), y(y_), z(z_)
    {}
    void print() const {
        std::cout << "( " << x << ", " << y << ", " << z << " )\n";
    }
};

inline auto normalise(const vec3& v) -> uvec3 {
    auto mag = magnitude(v);
    return uvec3{v.i/mag, v.j/mag, v.k/mag};
}

class Ray
{
public:
    const uvec3 direction;
    const pt3  origin;

    inline Ray(const vec3& direction_, const pt3& origin_) 
        : direction(normalise(direction_)), origin(origin_) 
    {}
    inline Ray(const uvec3& direction_, pt3 origin_) 
        : direction(direction_), origin(origin_)
    {}

    inline auto pt_at(float t) const -> pt3 {
        return { 
            origin.x + t * direction.i, 
            origin.y + t * direction.j, 
            origin.z + t * direction.k 
        };
    }
};

class Angle
{
private:
    // angle_ stored in radians, as mathematically preffered unit.
    float angle_;
public:
    Angle(const float& angle) : 
        angle_(angle)
    {}
    inline auto radians() const -> float{ 
        return angle_; 
    }
    inline auto degrees() const -> float {
        return angle_ / PI * 180;
    }
    inline auto is_orthogonal() const {
        return margin_of_error(angle_, PI / 2);
    }
};
inline auto degrees_to_radians(const float& degrees) -> float{
    return degrees * PI / 180;
}

inline auto dot_product(const vec3& v1, const vec3& v2) -> float {
    return (v1.i * v2.i) + (v1.j * v2.j) + (v1.k * v2.k);
}
inline auto dot_product(const uvec3& v1, const uvec3& v2) -> float {
    return (v1.i * v2.i) + (v1.j * v2.j) + (v1.k * v2.k);
}

inline auto angle(const vec3& v1, const vec3& v2) -> Angle {
    return Angle{ acosf(dot_product(v1, v2) / magnitude(v1) / magnitude(v2)) };
}

inline auto cross_product(const vec3& v1, const vec3& v2) -> vec3{
    return vec3{
        v1.j*v2.k - v1.k*v2.j,
        v1.k*v2.i - v1.i*v2.k,
        v1.i*v2.j - v1.j*v2.i
    };
}
inline auto is_orthogonal(const vec3& v1, const vec3& v2) -> bool {
    return margin_of_error(angle(v1, v2).radians(), PI / 2);
}
inline auto is_normalised(const vec3& v) -> bool {
    return margin_of_error(magnitude(v), 1);
}

//Ray is composed of a pt3 and vec3, aka origin and direction respectively
inline auto pt_from_ray(const pt3& origin, const vec3& direction ,const float& t) -> pt3 {
    return pt3{
        origin.x + t * direction.i,
        origin.y + t * direction.j,
        origin.z + t * direction.k
    };
}
inline auto vec_from_pts(const pt3& final, const pt3& intial) -> vec3 {
    return vec3{
        (final.x - intial.x),
        (final.y - intial.y),
        (final.z - intial.z) 
    };
}
inline auto uvec_to_vec(const uvec3& v) -> vec3 {
    return vec3{v.i, v.j, v.k};
}

inline auto min_dist(const pt3& line_point, const vec3& line_direction, const pt3& point) {
    return magnitude(cross_product(vec_from_pts(line_point, point), line_direction)) / magnitude(line_direction);
}
inline auto min_dist(const Ray& ray, const pt3 point) {
    return magnitude(cross_product(vec_from_pts(ray.origin, point), uvec_to_vec(ray.direction))) / magnitude(ray.direction);
}
inline auto get_t(const pt3& origin, const vec3& direction, const pt3& point) {
    
    return (direction.i == 0) ? (point.x - origin.x) / direction.i : (point.y - origin.y) / direction.j;
}

#endif