#ifndef PHONG_SHADER_H
#define PHONG_SHADER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <functional>
struct Material
{
	std::function<glm::vec3 (const glm::vec3& pos)> ambient;
	std::function<glm::vec3 (const glm::vec3& pos)> diffuse;
	std::function<glm::vec3 (const glm::vec3& pos)> specular;
	std::function<float(const glm::vec3& pos)> shininess;

	float kShade;
	float kReflect;
	float kRefract;
	float refractiveIndex;
};

class Light
{
public:
	virtual glm::vec3 calLight(
		const Material& material,
		const glm::vec3& fragPos,
		const glm::vec3& norm,
		const glm::vec3& viewDir) const = 0;
private:
};

class DirLight : public Light
{
public:
	DirLight(
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular,
		glm::vec3 direction);
	glm::vec3 calLight(
		const Material& material,
		const glm::vec3& fragPos,
		const glm::vec3& norm,
		const glm::vec3& viewDir) const;
private:
	glm::vec3 _ambient;
	glm::vec3 _diffuse;
	glm::vec3 _specular;
	glm::vec3 _direction;

	
};

#endif