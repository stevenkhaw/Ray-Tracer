#include <vector>

class Ray {
public:
	glm::vec3 p0; // basepoint
	glm::vec3 dir; // direction

	Ray(glm::vec3 p0, glm::vec3 dir) {
		this->p0 = p0;
		this->dir = dir;
	}
};