#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <variant>

#include "shapes.h"
#include "linearAlgebra.h"

struct RGB { 
	int r, g, b; 
	auto multiply(const float& factor) const {
		return RGB{
			int(r*factor),
			int(g*factor),
			int(b*factor)
		};
	}
};
struct Metal { float reflect_amount; };
struct Glass { float blur_amount; };

class PPM
{
public:
	const std::string file_name_;
	int width_;
	int height_;
public:
	void change_dimensions(const int& width, const int& height);
	std::vector<RGB> colour_info;
	PPM(const std::string& file_name, const int& width, const int& height);
	auto create_ppm() -> void;
};


using Material = std::variant<RGB, Metal, Glass>;

namespace raytracer
{
	struct Scene {
		std::vector<Material> materials;
		std::vector<shapes::Sphere> spheres;
		std::vector<pt3> point_lights;
		auto get_index(const shapes::Sphere* ptr) const
		{
			auto iter = std::find_if(spheres.begin(), spheres.end(),
				[&](const auto& s) {
					if (ptr == &s) [[unlikely]] {
						return true;
					}
					else [[likely]] {
						return false;
					}
				}
			);
			return std::distance(spheres.begin(), iter);
		}
		auto make_light(const pt3& position, const float& width = 0.1) {
			float offset;
			point_lights.emplace_back(position);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x + offset, position.y + offset, position.z + offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x + offset, position.y + offset, position.z - offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x + offset, position.y + offset, position.z - offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x + offset, position.y - offset, position.z + offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x + offset, position.y - offset, position.z - offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x + offset, position.y - offset, position.z - offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x - offset, position.y + offset, position.z + offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x - offset, position.y + offset, position.z - offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x - offset, position.y + offset, position.z - offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x - offset, position.y - offset, position.z + offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x - offset, position.y - offset, position.z - offset);
			offset = (rand() % 1000) / (1000 * width);
			point_lights.emplace_back(position.x - offset, position.y - offset, position.z - offset);

		}
		auto make_sphere(const pt3& position, const float& radius, const RGB& material = {0,0,200}) {
			materials.emplace_back(material);
			spheres.emplace_back(position, radius);
		}
		auto make_sphere(const shapes::Sphere& s, const RGB& material = { 0,0,200 }) {
			materials.emplace_back(material);
			spheres.emplace_back(s);
		}
	};

	class Camera {
	public:
		const int height;
		const int width;
		const Angle fov;
		const pt3 position;

		Camera(const int& screen_height,const int& screen_width, const float& field_of_view) 
			: height(screen_height), width(screen_width), fov(field_of_view), position(pt3{0,0,0})
		{}

		constexpr auto get_mx()				const { return -2.0f / width; }
		constexpr auto get_my()				const { return -2.0f / height; }
		constexpr auto get_aspect_ratio()	const { return  float(width / height); }
		const	auto get_fov_adjustment()	const { return tan(fov.radians() / 2); }

	};


	struct screen_coords {
		constexpr static vec3 left_low{ -1, -1, -1 };
		constexpr static vec3 vertical{ 0, 2, 0 };
		constexpr static vec3 horizontal{ 2, 0, 0 };
		// left_low + vertical + horizontal = right_high
	};

	auto shoot_rays(const int& height, const int& width, const RGB& background_colour, PPM& image);
	auto render(const Camera& camera) -> std::vector<RGB>;

}





#endif // _RAYTRACER_H_
