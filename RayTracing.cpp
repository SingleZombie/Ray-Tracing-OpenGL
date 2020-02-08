#include "RayTracing.h"

namespace RayTracing
{
	const unsigned int Scene::MAX_RECURSION_TIME = 5;

	Scene::Scene()
	{

	}
	Scene::~Scene()
	{
		for (auto ptr : _entitys)
		{
			delete ptr;
		}
		for (auto ptr : _lights)
		{
			delete ptr;
		}
	}
	void Scene::addEntity(Entity* entity)
	{
		_entitys.push_back(entity);
	}
	void Scene::addLight(Light* light)
	{
		_lights.push_back(light);
	}
	glm::vec3 Scene::traceRay(const Ray& ray, unsigned int recursionTime)
	{
		glm::vec3 lightIntensity(0.0f); // ���ڷ��صĹ���ǿ�ȣ���ʼ��Ϊ0

		// �ݹ�����������������ݹ����
		if (recursionTime >= MAX_RECURSION_TIME)
		{
			return lightIntensity;
		}

		// �������������Ľ����Լ�������
		const auto& pointAndEntity = getIntersection(ray);
		const Entity* collidedEntityPtr = pointAndEntity.second;

		// �ݹ��������������û�����䵽������
		if (collidedEntityPtr == nullptr)
		{
			return lightIntensity;
		}
		
		// �������㼰�䷨����
		glm::vec3 collidedPoint = pointAndEntity.first;
		glm::vec3 normal = glm::normalize(collidedEntityPtr->calNormal(collidedPoint));
		bool enterEntity = collidedEntityPtr->rayInEntity(ray);
		if (enterEntity)
		{
			// �������Ǵ������ڲ�����ģ�������Ӧ��ȡ��
			normal = -normal;
		}

		// ����ǿ�ȵĵ�һ���֣��ֲ�����ǿ��
		if (!enterEntity)
		{
			lightIntensity = collidedEntityPtr->getMaterial().kShade *
				shade(*collidedEntityPtr, collidedPoint, ray);
		}
		

		// ���㷴�䷽��
		glm::vec3 reflectDirection = glm::reflect(ray.getDirection(), normal);
		
		// ����ǿ�ȵĵڶ����֣��������ǿ��
		if (collidedEntityPtr->getMaterial().kReflect > FLOAT_EPS) // > 0
		{
			lightIntensity += collidedEntityPtr->getMaterial().kReflect *
				traceRay(Ray(collidedPoint, collidedPoint + reflectDirection), recursionTime + 1);
		}

		// ����������
		float currentIndex = 1.0f;
		float nextIndex = collidedEntityPtr->getMaterial().refractiveIndex;
		if (enterEntity)
		{
			// �������Ǵ������ڲ�����ģ���������Ҫ���н���
			std::swap(currentIndex, nextIndex);
		}

		// �������䷽��
		glm::vec3 refractDirection = glm::refract(ray.getDirection(), normal,  currentIndex / nextIndex);

		// ����ǿ�ȵĵ������֣��������ǿ��
		if (collidedEntityPtr->getMaterial().kRefract > FLOAT_EPS) // > 0
		{
			lightIntensity += collidedEntityPtr->getMaterial().kRefract * 
				traceRay(Ray(collidedPoint, collidedPoint + refractDirection), recursionTime + 1);
		}

		return lightIntensity;
	}

	std::pair<const glm::vec3&, const Entity*> Scene::getIntersection(const Ray& ray)
	{
		float minT = FLOAT_INF;
		const Entity* collidedEntity = nullptr;
		for (auto pEntity : _entitys)
		{
			float t = pEntity->rayCollision(ray);
			if (t > FLOAT_EPS) // >= 0
			{
				if (t < minT)
				{
					minT = t;
					collidedEntity = pEntity;
				}				
			}
		}

		return std::make_pair(ray.pointAtT(minT), collidedEntity);
	}

	glm::vec3 Scene::shade(const Entity& entity, glm::vec3 fragPos, const Ray& ray)
	{
		glm::vec3 result(0.0f);
		for (auto pLight : _lights)
		{
			result += pLight->calLight(entity.getMaterial(), fragPos, entity.calNormal(fragPos), ray.getDirection());
		}
		return result;
	}
}