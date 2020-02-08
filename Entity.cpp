#include "Entity.h"

namespace RayTracing
{
	// Plane
	Plane::Plane(const glm::vec3& aPoint, const glm::vec3& normal) : _normal(glm::normalize(normal)), _aPoint(aPoint)
	{

	}
	bool Plane::onPlane(const glm::vec3& p) const
	{
		return glm::dot(p - _aPoint, _normal) == 0;
	}

	float Plane::rayCollision(const Ray& ray) const
	{
		float v1 = glm::dot(ray.getVertex() - _aPoint, _normal);
		float v2 = glm::dot(_normal, ray.getDirection());
		if (abs(v2) < FLOAT_EPS) // v2 == 0
		{
			return -1;
		}
		else
		{
			return -v1 / v2;
		}
	}
	glm::vec3 Plane::calNormal(const glm::vec3& p) const
	{
		return _normal;
	}
	// Triangle

	Triangle::Triangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) :
		_vertice{ A, B, C }
	{

	}

	bool Triangle::inTriangle(const glm::vec3& p) const
	{
		auto plane = getPlane();

		if (!plane.onPlane(p))
		{
			return false;
		}

		glm::vec3 vectorToP[3];
		glm::vec3 crosses[3];
		for (int i = 0; i < 3; i++)
		{
			vectorToP[3] = p - _vertice[i];
		}
		for (int i = 0; i < 3; i++)
		{
			crosses[i] = glm::normalize(glm::cross(vectorToP[i], vectorToP[(i + 1) % 3]));
		}

		// Three cross vectors are in same direction
		if (glm::distance(crosses[0], crosses[1]) < FLOAT_EPS &&
			glm::distance(crosses[1], crosses[2]) < FLOAT_EPS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Plane Triangle::getPlane() const
	{
		return Plane(_vertice[0], getNormal());
	}

	glm::vec3 Triangle::getNormal() const
	{
		auto AB = _vertice[1] - _vertice[0];
		auto AC = _vertice[2] - _vertice[0];
		return glm::normalize(glm::cross(AB, AC));
	}

	void Triangle::getVertice(glm::vec3& A, glm::vec3& B, glm::vec3 C) const
	{
		A = _vertice[0];
		B = _vertice[1];
		C = _vertice[2];
	}

	float Triangle::rayCollision(const Ray& ray) const
	{
		auto plane = getPlane();

		float t = plane.rayCollision(ray);
		if (t < -FLOAT_EPS) // no collision
		{
			return -1;
		}
		return inTriangle(ray.pointAtT(t));
	}

	glm::vec3 Triangle::calNormal(const glm::vec3& p) const
	{
		return getNormal();
	}

	// Sphere
	Sphere::Sphere(const glm::vec3& center, float radius) : _center(center), _radius(radius)
	{

	}

	bool Sphere::inSphere(const glm::vec3& p) const
	{
		return glm::distance(p, _center) < _radius + FLOAT_EPS;
	}

	float Sphere::rayCollision(const Ray& ray) const
	{
		glm::vec3 vc = ray.getVertex() - _center;
	
		float A = glm::dot(ray.getDirection(), ray.getDirection());
		float B = 2 * glm::dot(vc, ray.getDirection());
		float C = glm::dot(vc, vc) - _radius * _radius;
		if (abs(C) < FLOAT_EPS)
		{
			C = 0;
		}

		float delta = B * B - 4 * A * C;
		if (delta < FLOAT_EPS)
		{
			return -1;
		}
		delta = sqrt(delta);
		float t1 = (-B + delta) / 2 / A;
		float t2 = (-B - delta) / 2 / A;
		if (t1 < FLOAT_EPS && t2 < FLOAT_EPS)
		{
			return -1;
		}
		if (t2 > FLOAT_EPS)
		{
			return t2;
		}
		else
		{
			return t1;
		}
	}
	glm::vec3 Sphere::calNormal(const glm::vec3& p) const
	{
		return glm::normalize(p - _center);
	}
	bool Sphere::rayInEntity(const Ray& ray) const
	{
		return inSphere(ray.getVertex()) && rayCollision(ray) > FLOAT_EPS;
	}
}