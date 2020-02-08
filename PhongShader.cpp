#include "PhongShader.h"

DirLight::DirLight(
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	glm::vec3 direction)
	: _ambient(ambient),
	_diffuse(diffuse),
	_specular(specular),
	_direction(direction)
{

}

glm::vec3 DirLight::calLight(
	const Material& material,
	const glm::vec3& fragPos,
	const glm::vec3& norm,
	const glm::vec3& viewDir) const 
{
	glm::vec3 ambient = _ambient * material.ambient(fragPos);

	glm::vec3 lightDir = normalize(-_direction);
	float diff = std::max(dot(norm, lightDir), 0.0f);
	glm::vec3 diffuse = diff * _diffuse * material.diffuse(fragPos);

	glm::vec3 middle = glm::normalize(-viewDir + lightDir);
	float spec = glm::pow(std::max(glm::dot(middle, norm), 0.0f), material.shininess(fragPos));
	glm::vec3 specular = _specular * spec * material.specular(fragPos);

	return (ambient + diffuse + specular);
}