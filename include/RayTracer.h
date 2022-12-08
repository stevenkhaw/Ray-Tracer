
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"
#include "RTGeometry.h"
#include "Triangle.h"
#include "Ray.h"
#include "Intersection.h"
#include "Image.h"
#include <math.h>

namespace RayTracer {
	Ray RayThruPixel(Camera* cam, int i, int j, int width, int height) {

		float alpha = 2 * (i + 0.5f) / width - 1;
		float beta = 1 - 2 * (j + 0.5f) / height;
		float a = width / height;

		glm::vec3 w = glm::normalize(cam->eye - cam->target);
		glm::vec3 u = glm::normalize(glm::cross(cam->up, w));
		glm::vec3 v = glm::cross(w, u);

		float fovyRad = cam->fovy * M_PI / 180;

		glm::vec3 d = glm::normalize(alpha * a * glm::tan(fovyRad / 2) * u + beta * glm::tan(fovyRad / 2) * v - w);

		
		return Ray(cam->eye, d);
	}

	Intersection Intersect(Ray* ray, Triangle* triangle) {
		glm::vec4 p0 = glm::vec4(ray->p0, 1);
		glm::vec4 p1 = glm::vec4(triangle->P[0], 1);
		glm::vec4 p2 = glm::vec4(triangle->P[1], 1);
		glm::vec4 p3 = glm::vec4(triangle->P[2], 1);
		glm::vec4 newD = glm::vec4(-ray->dir, 0);


		glm::mat4 leftFour = glm::mat4(p1, p2, p3, newD);

		glm::vec4 lambdas = glm::inverse(leftFour) * p0;

		if (lambdas.x < 0 || lambdas.y < 0 || lambdas.z < 0 || lambdas.w < 0) {
			return Intersection();
		}

		Intersection answer = Intersection();
		answer.P = glm::vec3(lambdas.x * triangle->P[0] + lambdas.y * triangle->P[1]
			+ lambdas.z * triangle->P[2]);
		answer.N = glm::vec3(glm::normalize(lambdas.x * triangle->N[0] + lambdas.y * triangle->N[1]
			+ lambdas.z * triangle->N[2]));
		answer.V = ray->dir;
		answer.triangle = triangle;
		answer.dist = lambdas.w;

		return answer;
	}

	Intersection Intersect(Ray* ray, RTScene* scene) {
		Intersection hit = Intersection();
		hit.triangle = nullptr;
		hit.dist = INFINITY;

		for (int i = 0; i < scene->triangle_soup.size(); i++) { // Find closest intersection; test all objects

			Triangle* currTriangle = scene->triangle_soup[i];

			Intersection hit_temp = Intersect(ray, currTriangle);

			if (hit_temp.triangle == nullptr) {
				continue;
			} else if (hit_temp.dist < hit.dist) { // closer than previous hit
				hit = hit_temp;
			}
		}

		return hit;
	}

	glm::vec3 FindColor(Intersection* hit, RTScene* scene, int recursion_depth) {

		
		if (hit->triangle == nullptr) {
			//return glm::vec3(0.3f); // gray
			return glm::vec3(0.18, 0.25, 0.325); // grayish blue
		}

		// color 
		glm::vec3 color = hit->triangle->material->emision;

		// specular
		glm::vec3 spec = hit->triangle->material->specular;

		// ambient
		glm::vec3 amb = hit->triangle->material->ambient;

		// diffuse
		glm::vec3 diff = hit->triangle->material->diffuse;

		// normal vector
		glm::vec3 n = glm::normalize(hit->N);

		// ref to map of lights
		std::map< std::string, Light* > lights = scene->light;

		// for each light l in scene
		// light = it->second
		for (auto it = lights.begin(); it != lights.end(); it++) {
			
			// reflection
			if (recursion_depth > 0) {
				if (recursion_depth > 5) recursion_depth = 5;

				glm::vec3 v = normalize(hit->V);
				glm::vec3 newDirection = glm::normalize(2.0f * glm::dot(n,v) * n) - v;
				Ray newRay = Ray(hit->P + 0.1f * hit->N, newDirection);
				Intersection newHit = Intersect(&newRay, scene);

				// in case the reflection doesnt hit anything
				if (newHit.triangle == nullptr) {
					return color = FindColor(hit, scene, 0);
				}

				color += spec * FindColor(&newHit, scene, recursion_depth - 1);
			}

			// shadow
			glm::vec4 target = glm::vec4(scene->camera->target, 1.0f);  //origin
			glm::vec3 smallTarget = glm::vec3(scene->camera->target);

			glm::vec4 lgb = scene->camera->view * it->second->position; //light position
			glm::vec3 smallLGB = glm::vec3(lgb[0], lgb[1], lgb[2]); //light position, but as a vec3

			// Generate a ray from the intersect point to the light
			Ray newLightRay = Ray(hit->P + 0.1f * hit->N, it->second->position - glm::vec4(hit->P, 1.0f));
			Intersection hitBlock = Intersect(&newLightRay, scene);

			float manDist = glm::length(smallLGB - newLightRay.p0);
			float hitDist = hitBlock.dist;

			if (hitDist < manDist) {
				continue;
			}
			else {
				color += amb + diff;
			}
		}

		return color;
	}

	void Raytrace(Camera* cam, RTScene* scene, Image& image) {

		int w = image.width; int h = image.height;

		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				Ray ray = RayThruPixel(cam, i, j, w, h);
				Intersection hit = Intersect(&ray, scene);
				image.pixels[h - j - 1][i] = FindColor(&hit, scene, 3);
			}

			std::cout << j << std:: endl;
		}
	}
};
