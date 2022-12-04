#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"
#include "RTGeometry.h"
#include "Triangle.h"
#include "Ray.h"
#include "Intersection.h"
#include "Image.h"

using namespace glm;
namespace RayTracer {
	void Raytrace(Camera cam, RTScene scene, Image& image); // page 9
	Ray RayThruPixel(Camera cam, int i, int j, int width, int height);// page 10, 18
	Intersection Intersect(Ray ray, Triangle triangle); // page 30, 33
	Intersection Intersect(Ray ray, RTScene scene); // page 11, 28, 31
	glm::vec3 FindColor(Intersection hit, RTScene scene, int recursion_depth); // page 15
};

void RayTracer::Raytrace(Camera cam, RTScene scene, Image& image) {

	int w = image.width; int h = image.height;

	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			Ray ray = RayThruPixel(cam, i, j, w, h);
			Intersection hit = Intersect(ray, scene);
			image.pixel[i][j] = FindColor(hit);
		}
	}
}

Ray RayThruPixel(Camera cam, int i, int j, int width, int height) {

	float alpha = 2 * (i + 1 / 2) / width - 1;
	float beta = 1 - 2 * (j + 1 / 2) / height;
	float a = width / height;
	
	vec3 w = normalize(cam->eye - cam->target);
	vec3 u = normalize(cross(cam->up, w);
	vec3 v = cross(w, u);

	vec3 d = normalize(alpha * a * tan(fovy / 2) * u + beta * tan(fovy / 2) * v - w);

	return (new Ray(cam->eye, d));
}

Intersection Intersect(Ray ray, Triangle triangle) {
	vec4 p0 = vec4(ray->p0, 1);
	vec4 p1 = vec4(triangle->P[0], 1);
	vec4 p2 = vec4(triangle->P[1], 1);
	vec4 p3 = vec4(triangle->P[2], 1);
	vec4 newD = vec4(-ray->dir, 0);

	mat4 leftFour = mat4(p1, p2, p3, newD);

	mat4 lambdas = rayBase * inverse(leftFour);

	if (lambdas.x < 0 || lambdas.y < 0 || lambdas.z < 0 || lambdas.w < 0) {
		return nullptr;
	}

	Intersection answer = new Intersection();
	answer.P = vec3(lambdas.x * triangle->P[0] + lambdas.y * triangle->P[1]
		+ lambdas.z * triangle->P[2]);
	answer.N = vec3(normalize(lambdas.x * triangle->N[0] + lambdas.y * triangle->N[1] 
		+ lambdas.z * triangle->N[2]));
	answer.V = ray->dir;
	answer.triangle = triangle;
	answer.dist = lambdas.w;

	return answer;
}

Intersection Intersect(Ray ray, RTScene scene) {
	float mindist = INFINITY;
	Intersection hit;

	for (int i = 0; i < scene->triangle_soup.length; i++) { // Find closest intersection; test all objects

		Triangle currTriangle = scene->triangle_soup[i];

		Intersection hit_temp = Intersect(ray, currTriangle);

		if (hit_temp.dist < mindist) { // closer than previous hit
			mindist = hit_temp.dist;
			hit = hit_temp;
		}
	}

	return hit;
}

glm::vec3 FindColor(Intersection hit, RTScene scene, int recursion_depth) {
	std::map< std::string, Light* > lights = scene -> light;
	std::vector<Ray> newLightRays;
	std::vector<vec4> newLightColors;
		
	//generate new light rays
	for (auto it = lights.begin(); it != lights.end(); it++) {

		// Generate a ray from the incident point to the light 
		Ray newLightRay = new Ray(hit->P, it->position - hit->P);

		newLightRays.push_back(newLightRay);

		// let's check if the ray gets to the light. 
		if (Intersect(newLightRay, scene) == nullptr) {
			newLightColors.push_back(vec4(0.0f));
		} else {
			newLightColors.push_back(it->color);
		}
	}

	vec3 rayToLight = 

}