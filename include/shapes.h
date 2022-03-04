#ifndef _SHAPES_H_
#define _SHAPES_H_
#include <math.h>

#include <utility>
#include <optional>
#include "linearAlgebra.h"

namespace shapes
{
	struct Sphere
	{
		const pt3   position;
		const float radius;
		const float radius_squared;
		Sphere(const float& x, const float& y, const float& z, const float& r)
			: position({x,y,z}), radius(r), radius_squared(r*r)
		{}
		Sphere(const pt3& center_point, const float& r)
			: position(center_point), radius(r), radius_squared(r*r)
		{}
	};

	inline auto distance_from_center(const Sphere& s, const pt3& p) -> float {
		return sqrt(pow(p.x - s.position.x, 2) + pow(p.y - s.position.y, 2) + pow(p.z - s.position.z, 2));
	}
	inline auto is_on_surface(const Sphere& s, const pt3& p) -> bool {
		return margin_of_error(distance_from_center(s, p), s.radius);
	}
	inline auto has_hit_sphere(const Sphere& s, const Ray& r) {
		return (min_dist(r, s.position) <= s.radius) ? true : false;
	}

	inline auto get_hit_pt(const Sphere& sphere, const pt3& ray_origin, const vec3& ray_direction) -> std::optional<pt3>{
		//Reference: https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
		//const auto sphere_to_origin = vec_from_pts(sphere.position, ray_origin);
		const vec3 sphere_to_origin = {
			sphere.position.x - ray_origin.x,
			sphere.position.y - ray_origin.y,
			sphere.position.z - ray_origin.z,
		};


		const auto A = dot_product(ray_direction, ray_direction);
		const auto B = 2*dot_product(ray_direction, sphere_to_origin);
		const auto C = dot_product(sphere_to_origin, sphere_to_origin) - sphere.radius_squared;
		const auto D = B*B - 4 * A * C;
		if (D < 0) [[likely]] 
		{
			return {};
		}
		else [[unlikely]]
		{
			//Quadratic Formula
			const auto t1 = float((-B - sqrt(D)) / (A));
			const auto t2 = float((-B + sqrt(D)) / (A));
			//Want closet value to the camera.
			const auto t = (t1 < t2) ? t1 : t2;
		
			return pt3{ 
				ray_origin.x + ray_direction.i * t, 
				ray_origin.y + ray_direction.j * t, 
				ray_origin.z + ray_direction.k * t 
			};
		}
	}
	inline auto get_hit_t(const Sphere& sphere, const pt3& ray_origin, const vec3& ray_direction) -> std::optional<float> {
		//Reference: https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
		const auto sphere_to_origin = vec_from_pts(ray_origin, sphere.position);
		const auto A = dot_product(ray_direction, ray_direction);
		const auto B = 2 * dot_product(ray_direction, sphere_to_origin);
		const auto C = dot_product(sphere_to_origin, sphere_to_origin) - sphere.radius_squared;
		const auto D = B * B - 4 * A * C;

		if (D < 0) [[likely]] {
			return {};
		}
		else [[unlikely]] {
			auto t1 = (-B - sqrt(D)) / (2 * A);
			t1 = round(0, t1);

			//Want smallest +ve value
			if (t1>0) [[unlikely]] {
				return t1;
			}
			else [[likely]] {
				return {};
			}
		}
	}
	inline auto get_hit_tt(const Sphere& sphere, const pt3& ray_origin, const vec3& ray_direction) -> std::optional<std::pair<float, float>> {
		//Reference: https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
		const auto sphere_to_origin = vec_from_pts(ray_origin, sphere.position);
		const auto A = dot_product(ray_direction, ray_direction);
		const auto B = dot_product(ray_direction, sphere_to_origin);
		const auto C = dot_product(sphere_to_origin, sphere_to_origin) - sphere.radius_squared;
		const auto D = B * B - (A * C);

		if (D > 0) [[unlikely]] {
			const auto t1 = (-B + sqrt(D)) / A;
			const auto t2 = (-B - sqrt(D)) / A;
			return std::pair{t1,t2};	
		}
		else [[likely]] {
			return {};
		}
	}

	inline auto get_normal_vec(const Sphere& s, const pt3& point_on_sphere) {
		return vec_from_pts(point_on_sphere, s.position);
	}


}



#endif