#include "Ray.h"

namespace RayTracing
{
	Ray::Ray(glm::vec3 src, glm::vec3 dest) : _vertex(src), _direction(glm::normalize(dest - src))
	{

	}

	glm::vec3 Ray::pointAtT(float t) const
	{
		return _vertex + t * _direction;
	}
}
