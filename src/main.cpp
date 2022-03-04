#include "linearAlgebra.h"
#include "raytracer.h"
#include "shapes.h"
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>

int main(int argc, char* argv[])
{
	static const auto camera = raytracer::Camera{1080, 1080, degrees_to_radians(90) };
	static const auto colour = raytracer::render(camera);

	PPM output("test.ppm", camera.width, camera.height);
	std::copy(colour.begin(), colour.end(), std::back_inserter(output.colour_info));
	output.create_ppm();

	return 1;
}