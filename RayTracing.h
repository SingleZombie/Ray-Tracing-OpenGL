#ifndef RAYTRACING_H
#define RAYTRACING_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Entity.h"
#include <vector>

namespace RayTracing
{
	class Scene
	{
	public:
		Scene();
		~Scene();
		void addEntity(Entity* entity);
		void addLight(Light* light);
		glm::vec3 traceRay(const Ray& ray, unsigned int recursionTime = 0);
		std::pair<const glm::vec3&, const Entity*> getIntersection(const Ray& ray);
		glm::vec3 shade(const Entity& entity, glm::vec3 fragPos, const Ray& ray);

		static const unsigned int MAX_RECURSION_TIME;
	private:
		std::vector<Entity*> _entitys;
		std::vector<Light*> _lights;
	};
}



#endif