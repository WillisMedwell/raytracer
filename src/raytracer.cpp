#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <optional>
#include <chrono>
#include <ranges>
#include <numeric>
#include <thread>
#include <execution>
#include <cstdlib>
#include <thread>
#include <random>
#include <stdlib.h>
#include <random>
#include <string>
#include <array>

#include "raytracer.h"
#include "shapes.h"

constexpr size_t RAY_DEPTH = 10;

struct HitRecord {
	bool has_hit;
	int shape_index;
	pt3 point;
	vec3 normal;

	HitRecord() 
		: has_hit(false), shape_index(-1), point{0,0,0}, normal{0,0,0}
	{}
	HitRecord(int index, pt3 pt, vec3 norm) 
		: has_hit(true), shape_index(index), point(pt), normal(norm)
	{}
};

inline auto find_first_hit(
	const raytracer::Scene& objects,
	const vec3& direction,
	const pt3& origin) -> HitRecord
{
	std::optional<float> t_min;
	auto index = 0;
	auto shape_index = 0;

	for (const auto& sphere : objects.spheres) {
		const auto t = shapes::get_hit_t(sphere, origin, direction);
		if (!t.has_value()) [[likely]] {
		}
		else if(!t_min.has_value() || t_min.value() > t.value()) [[unlikely]] {
			shape_index = index;
			t_min = t;
		}
		++index;
	}

	//no hit
	if (!t_min.has_value()) [[likely]] {
		return HitRecord{};
	}
	//sphere
	else if (shape_index < objects.spheres.size()) {
		const auto hit_pt = pt_from_ray(origin, direction, t_min.value());
		return HitRecord{
			shape_index,
			hit_pt,
			shapes::get_normal_vec(objects.spheres[shape_index], hit_pt)
		};
	}
	//other shape
	else {
		std::cout << "ERROR NEED TO DO OTHER SHAPES\n";
		std::cout << "FUNCTION: find_first_hit\n";
		exit(-1);
		return HitRecord{};
	}
}
 
inline auto get_lit_count(
	const raytracer::Scene& objects,
	const pt3& hit) -> float
{
	static const float increment = 1 / objects.point_lights.size();
	float lit_increment = increment;
	float lit_count = 0;
	auto direction = vec3{ 0,0,0 };

	for (const auto& light : objects.point_lights)
	{
		direction = vec_from_pts(light, hit);
		for (auto& s : objects.spheres)
		{
			auto tt = shapes::get_hit_tt(s, hit, direction);
			if (tt.has_value()) {
				constexpr float min = 0.0001;
				auto t1 = round(0, tt.value().first, min);
				auto t2 = round(0, tt.value().second, min);
				if (t1 < 0 && t2 > 0 ) {
					lit_increment = 0;
				}
				else if (t1 > 0 && t2 == 0) {
					lit_increment = 0;
				}
				else if (t1 > 0 && t2 > 0) {
					lit_increment = 0;
				}
				else {
				}
			}
		}

		lit_count += lit_increment;
		lit_increment = increment;
	}
	return lit_count;
}

inline auto random_vec() {
	vec3 random;
	static std::default_random_engine rand_engine;
	static std::uniform_real_distribution<float> get_rand(-1, 1);
	random = {
		get_rand(rand_engine),
		get_rand(rand_engine),
		get_rand(rand_engine)
	};
	return random;
}

inline auto recursive_get_colour(
	const raytracer::Scene& objects,
	const vec3& direction,
	const pt3& origin,
	std::vector<float>& light_depth,
	const RGB& colour) -> RGB {

	const auto hit = find_first_hit(objects, direction, origin);
	if (hit.has_hit && light_depth.size() < RAY_DEPTH)
	{
		vec3 rand_direction = uvec_to_vec(normalise(random_vec() + hit.normal));
		light_depth.emplace_back(get_lit_count(objects, hit.point));
		return recursive_get_colour(objects, rand_direction, hit.point, light_depth, colour);
	}
	else {
		float factor = 0;
		for (size_t i = 0; i < light_depth.size(); i++)
		{
			factor += light_depth[i] * pow(0.5, i + 1);
		}
		return colour.multiply(factor);
	}
}

inline auto get_colour(
	const raytracer::Scene& objects,
	const vec3& direction, 
	const pt3& origin,
	const RGB& colour) -> RGB {
	const auto hit = find_first_hit(objects, direction, origin);

	if (hit.has_hit)
	{
		std::vector<float> light_depth{};
		light_depth.reserve(RAY_DEPTH);
		light_depth.emplace_back(get_lit_count(objects, hit.point));
		const auto colour2 = std::get<RGB>(objects.materials[hit.shape_index]);

		const vec3 rand_direction = random_vec() + hit.normal;
		return recursive_get_colour(objects, rand_direction, hit.point, light_depth, colour2);
	}
	else {
		return colour;
	}
}


inline auto get_camera_vector(
	const size_t& x, 
	const size_t& y, 
	const raytracer::Camera& camera) {
	static const float mx = camera.get_mx();
	static const float my = camera.get_my();
	static const float aspect_ratio = camera.get_aspect_ratio();
	static const float fov_adjustment = camera.get_fov_adjustment();

	return vec3{
		fov_adjustment * aspect_ratio * (x * mx + 1),
		fov_adjustment * (y * my + 1),
		-1.0f
	};
}

inline auto render_loop(
	const raytracer::Camera& camera, 
	const raytracer::Scene& objects)
{
	std::vector<RGB> pixels;
	pixels.resize(camera.height * camera.width);

	RGB colour = { 0,0,0 };
	auto partial_loop = [&](size_t start, size_t end) {
		HitRecord hit;
		auto colour = RGB{ 0,0,0 };
		int index = start * camera.width;

		for (size_t y = start; y < end; ++y) {
			for (size_t x = camera.width; x > 0; --x) {
				auto direction = get_camera_vector(x, y, camera);

				RGB colourAvg = {0,0,0};
				constexpr auto SIZE = 2;
				for (size_t i = 0; i < SIZE; i++)
				{
					auto newColour = get_colour(objects, direction, pt3{ 0,0,0 }, RGB{ 0,0,0 });
					colourAvg = {
						newColour.r + colourAvg.r,
						newColour.g + colourAvg.g,
						newColour.b + colourAvg.b
					};
				}
				colour = { 
					colourAvg.r / SIZE,
					colourAvg.g / SIZE,
					colourAvg.b / SIZE,
				};

				pixels[index] = colour;
				index++;
			}
		}

	};

	auto threaded_loop = [&](int num_threads) {
		std::vector<std::thread> threads;

		for (size_t i = 0; i < num_threads; i++) {
			threads.emplace_back(partial_loop, i * camera.height / num_threads, (1+i) * camera.height / num_threads);
		}
		for (size_t i = num_threads; i > 0; i--) {
			threads.back().join();
			threads.pop_back();
		}
	};

	threaded_loop(10);


	return pixels;
}

auto raytracer::render(const Camera& camera) -> std::vector<RGB>
{
	raytracer::Scene objects;

	objects.make_sphere(pt3{   0, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{ 0.5, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{-0.5, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{ 1.0, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{-1.0, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{ 1.5, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{-1.5, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{ 2.0, 0,-0.5 }, 0.2, RGB{200,50,50});
	objects.make_sphere(pt3{-2.0, 0,-0.5 }, 0.2, RGB{200,50,50});

	objects.make_sphere(pt3{0, 0, 2 }, 1);
	objects.make_sphere(pt3{0,-100.2,-1 }, 100);

	objects.point_lights.emplace_back(-1, 10, 0);



	auto time_render_start = std::chrono::high_resolution_clock::now();

	std::vector<RGB> pixels = render_loop(camera, objects);

	auto time_render_end = std::chrono::high_resolution_clock::now();
	auto time_render = std::chrono::duration<double, std::milli>{ time_render_end - time_render_start };
	std::cout << "Render Time = " << time_render << "\n";

	return pixels;
}

PPM::PPM(
	const std::string& file_name, 
	const int& width, 
	const int& height)
	: file_name_(file_name), width_(width), height_(height)
{
	colour_info.reserve(width_ * height_);
}

auto PPM::create_ppm() -> void
{
	if (colour_info.size() < width_ * height_) {
		std::cout << ".PPM ERROR:\tColour data is not complete\n";
		std::cout << colour_info.size() << " when should be: " << width_ * height_ << '\n';
	}
	if (colour_info.size() > width_ * height_) {
		std::cout << ".PPM ERROR:\tColour data is too big\n";
		std::cout << colour_info.size() << " when should be: " << width_ * height_ << '\n';
	}
	std::ofstream output_file{ file_name_ };
	output_file << "P3\n";
	output_file << width_ << " " << height_ << "\n255\n";
	for(const auto& rgb : colour_info)
	{
		if (rgb.r > 255 || rgb.g > 255 || rgb.g > 255) {
			std::cout << "invalid- to big\n";
		}
		if (rgb.r < 0 || rgb.g < 0 || rgb.g < 0) {
			std::cout << "invalid - to small\n";
		}
		output_file << std::to_string(rgb.r) << " " << std::to_string(rgb.g) << " " << std::to_string(rgb.b) << " ";
	}

	output_file.close();
}

void PPM::change_dimensions(
	const int& width, 
	const int& height)
{
	width_ = width;
	height_ = height;
	colour_info.resize(width_ * height_);
}