#ifndef RAY_TRACING_ENTITY_H
#define RAY_TRACING_ENTITY_H

#include "PhongShader.h"
#include "Ray.h"

namespace RayTracing
{
	class Entity
	{
	public:
		virtual float rayCollision(const Ray& ray) const = 0; // return parameter t
		virtual glm::vec3 calNormal(const glm::vec3& p) const = 0;
		virtual bool rayInEntity(const Ray& ray) const = 0;
		void setMaterial(const Material& m) { _material = m; }
		const Material& getMaterial() const { return _material; }
	protected:
		Material _material;
	};

	class Plane : public Entity
	{
	public:
		Plane(const glm::vec3& aPoint, const glm::vec3& normal);
		bool onPlane(const glm::vec3& p) const;
		glm::vec3 getNormal() const { return _normal; }
		glm::vec3 getAPoint() const { return _aPoint; }

		float rayCollision(const Ray& ray) const;
		glm::vec3 calNormal(const glm::vec3& p) const;
		bool rayInEntity(const Ray& ray) const { return false; }
	private:
		glm::vec3 _normal;
		glm::vec3 _aPoint;
	};

	class Triangle : public Entity
	{
	public:
		Triangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
		bool inTriangle(const glm::vec3& p) const;
		Plane getPlane() const;
		glm::vec3 getNormal() const;
		void getVertice(glm::vec3& A, glm::vec3& B, glm::vec3 C) const;

		float rayCollision(const Ray& ray) const;
		glm::vec3 calNormal(const glm::vec3& p) const;
		bool rayInEntity(const Ray& ray) const { return false; }
	private:
		glm::vec3 _vertice[3];
	};

	class Sphere : public Entity
	{
	public:
		Sphere(const glm::vec3& center, float radius);
		bool inSphere(const glm::vec3& p) const;
		glm::vec3 getCenter() const { return _center; }
		float getRadius() const { return _radius; }

		float rayCollision(const Ray& ray) const;
		glm::vec3 calNormal(const glm::vec3& p) const;
		bool rayInEntity(const Ray& ray) const;
	private:
		glm::vec3 _center;
		float _radius;
	};
}

#endif