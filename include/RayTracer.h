
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"
#include "RTGeometry.h"
#include "Triangle.h"
#include "Ray.h"
#include "Intersection.h"
#include "Image.h"

namespace RayTracer {
	Ray RayThruPixel(Camera* cam, int i, int j, int width, int height) {

		float alpha = 2 * (i + 1 / 2) / width - 1;
		float beta = 1 - 2 * (j + 1 / 2) / height;
		float a = width / height;

		glm::vec3 w = glm::normalize(cam->eye - cam->target);
		glm::vec3 u = glm::normalize(glm::cross(cam->up, w));
		glm::vec3 v = glm::cross(w, u);

		glm::vec3 d = glm::normalize(alpha * a * glm::tan(cam->fovy / 2) * u + beta * glm::tan(cam->fovy / 2) * v - w);

		return *(new Ray(cam->eye, d));
	}

	Intersection Intersect(Ray* ray, Triangle* triangle) {
		glm::vec4 p0 = glm::vec4(ray->p0, 1);
		glm::vec4 p1 = glm::vec4(triangle->P[0], 1);
		glm::vec4 p2 = glm::vec4(triangle->P[1], 1);
		glm::vec4 p3 = glm::vec4(triangle->P[2], 1);
		glm::vec4 newD = glm::vec4(ray->dir, 0);


		glm::mat4 leftFour = glm::mat4(p1, p2, p3, newD);

		glm::vec4 lambdas = glm::inverse(leftFour) * p0;

		/*if (lambdas.x < 0 || lambdas.y < 0 || lambdas.z < 0 || lambdas.w < 0) {
			return NULL;
		}*/

		Intersection* answer = new Intersection();
		answer->P = glm::vec3(lambdas.x * triangle->P[0] + lambdas.y * triangle->P[1]
			+ lambdas.z * triangle->P[2]);
		answer->N = glm::vec3(glm::normalize(lambdas.x * triangle->N[0] + lambdas.y * triangle->N[1]
			+ lambdas.z * triangle->N[2]));
		answer->V = ray->dir;
		answer->triangle = triangle;
		answer->dist = lambdas.w;

		return *answer;
	}

	Intersection Intersect(Ray* ray, RTScene* scene) {
		float mindist = INFINITY;
		Intersection hit;

		for (int i = 0; i < scene->triangle_soup.size(); i++) { // Find closest intersection; test all objects

			Triangle* currTriangle = scene->triangle_soup[i];

			Intersection hit_temp = Intersect(ray, currTriangle);

			if (hit_temp.dist < mindist) { // closer than previous hit
				mindist = hit_temp.dist;
				hit = hit_temp;
			}
		}

		return hit;
	}

	glm::vec3 FindColor(Intersection* hit, RTScene* scene, int recursion_depth) {
		// A vague attempt at recursive lighting 
		/*std::map< std::string, Light* > lights = scene->light;

		// array of ray2
		std::vector<Ray> newLightRays; // all secondary rays to all lights

		// all colors of light that touch intersection
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
		*/


		//lets convert everything into our camera coordinates
		glm::vec3 lj;
		glm::vec3 camPos = scene->camera->eye;
		//to get vector towards camera, we do xcamPos = ((0,0,0,1) - camPos).xyz  this vector then needs to be normalized
		glm::vec3 v = glm::normalize(scene->camera->target - camPos);
		//normalize... this is probably the source of issues because we really only need to invert and transpose over the inner 3x3 matrix
		glm::vec3 n = glm::normalize(hit->N);
		//vec3 n = normalize(transpose(inverse(mat3(modelview))) * normalize(normal));


		//we are going to do a loop, where each iteration goes over one light
		glm::vec4 total = glm::vec4(0, 0, 0, 0);
		glm::vec4 iterationSum;

		std::map< std::string, Light* > lights = scene->light;

		for (auto it = lights.begin(); it != lights.end(); it++) {
			glm::vec4 target = glm::vec4(scene->camera->target, 1.0f);  //origin
			glm::vec3 smallTarget = glm::vec3(scene->camera->target);
			glm::vec4 lgb = scene->camera->view * it->second->position; //light position
			glm::vec3 smallLGB = glm::vec3(lgb[0], lgb[1], lgb[2]); //light position, but as a vec3

			lj = glm::normalize(target[3] * smallLGB - lgb[3] * smallTarget); //direction to light
			iterationSum = glm::vec4(0, 0, 0, 0);

			//ambient
			iterationSum += hit->triangle->material->ambient;

			//diffuse
			iterationSum += hit->triangle->material->diffuse * glm::max(glm::dot(n, lj), 0.0f);

			//specular
			iterationSum += hit->triangle->material->specular * pow(glm::max(glm::dot(n, glm::normalize(v + lj)), 0.0f), hit->triangle->material->shininess);

			//all together, we multiply this by the light color
			total += iterationSum * it->second->position;
		}

		total += hit->triangle->material->emision;

		glm::vec3 smallTotal = glm::vec3(total[0], total[1], total[2]);

		return smallTotal; // vec4
	}

	void Raytrace(Camera* cam, RTScene* scene, Image& image) {

		int w = image.width; int h = image.height;

		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				Ray ray = RayThruPixel(cam, i, j, w, h);
				Intersection hit = Intersect(&ray, scene);
				image.pixels[i + j] = FindColor(&hit, scene, 0);
			}
		}
	}
};
