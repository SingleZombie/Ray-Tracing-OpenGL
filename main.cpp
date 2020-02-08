#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>

#include "Shader/Shader.h"
#include "RayTracing.h"

const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

void resizeGL(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

glm::vec3 viewPos = glm::vec3(0.0f, 2.0f, 3.0f);
glm::vec3 viewFront = glm::vec3(0, 0, -1);
glm::vec3 viewUp = glm::vec3(0.0f, 1.0f, 0.0f);

RayTracing::Scene scene;

int main()
{
	// 初始化OpenGL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ray Tracing", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resizeGL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 初始化着色器	
	Shader shader("Shader/Vertex", "Shader/Fragment");

	// 将点设为着色对象
	float point[] = { 0, 0, 0 };

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 设置光线、平面、球体的参数，并把它们加入场景
	scene.addLight(new DirLight(
		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.6f, 0.6f, 0.6f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-0.5f, -1.0f, -1.0f)
	));
	Material planeMaterial;
	planeMaterial.kShade = 0.7f;
	planeMaterial.kReflect = 0.3f;
	planeMaterial.kRefract = 0.0f;
	auto isBlack = [](const glm::vec3& pos)
	{
		return fmod(floor(pos.x) + floor(pos.z), 2) == 0;
	};
	planeMaterial.ambient = [=](const glm::vec3& pos)->glm::vec3
	{
		glm::vec3 color(1.0f, 1.0f, 1.0f);
		if (isBlack(pos))
		{
			return color;
		}
		else
		{
			return glm::vec3(1.0f, 1.0f, 1.0f) - color;
		}
	};
	planeMaterial.diffuse = [=](const glm::vec3& pos)->glm::vec3
	{
		glm::vec3 color(1.0f, 1.0f, 1.0f);
		if (isBlack(pos))
		{
			return color;
		}
		else
		{
			return glm::vec3(1.0f, 1.0f, 1.0f) - color;
		}
	};
	planeMaterial.specular = [=](const glm::vec3& pos)->glm::vec3
	{
		glm::vec3 color(1.0f, 1.0f, 1.0f);
		if (isBlack(pos))
		{
			return color;
		}
		else
		{
			return glm::vec3(1.0f, 1.0f, 1.0f) - color;
		}
	};
	planeMaterial.shininess = [](const glm::vec3& pos)->float
	{
		return 32.0f;
	};
	RayTracing::Plane* plane = new RayTracing::Plane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	plane->setMaterial(planeMaterial);
	scene.addEntity(plane);

	Material ballMaterial;
	ballMaterial.kShade = 0.6f;
	ballMaterial.kReflect = 0.2f;
	ballMaterial.kRefract = 0.2f;
	ballMaterial.refractiveIndex = 1.5f;
	ballMaterial.ambient = [](const glm::vec3& pos)->glm::vec3
	{
		return { 1.0f, 1.0f, 1.0f };
	};
	ballMaterial.diffuse = [](const glm::vec3& pos)->glm::vec3
	{
		return { 1.0f, 1.0f, 1.0f };
	};
	ballMaterial.specular = [](const glm::vec3& pos)->glm::vec3
	{
		return  { 0.6f, 0.6f, 0.6f };
	};
	ballMaterial.shininess = [](const glm::vec3& pos)->float
	{
		return 32.0f;
	};
	auto ball = new RayTracing::Sphere(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
	ball->setMaterial(ballMaterial);
	scene.addEntity(ball);



	while (!glfwWindowShouldClose(window))
	{
		// 处理输入信息并初始化缓冲、变换矩阵
		processInput(window);

		glClearColor(0.3, 0.3, 0.3, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		model = glm::mat4(1.0f);
		view = glm::lookAt(viewPos, viewPos + viewFront, viewUp);
		projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		// 初始化观察右向量
		glm::vec3 viewRight = glm::normalize(glm::cross(viewFront, viewUp));

		// 将点设置为绘制对象
		shader.use();
		glBindVertexArray(VAO);

		// 枚举屏幕上每一个像素
		for (unsigned int i = 0; i < SCR_WIDTH; i++)
		{
			for (unsigned int j = 0; j < SCR_HEIGHT; j++)
			{
				// 将像素坐标分量映射到[0, 1]
				glm::vec3 pos(float(i) * 2 / SCR_WIDTH - 1.0f, float(j) * 2 / SCR_HEIGHT - 1.0f, 0.0f);
				shader.setVec2("screenPos", pos.x, pos.y);

				// 计算像素在世界坐标中的位置
				glm::vec3 globalPos = viewPos + viewFront + pos.x * viewRight * (float(SCR_WIDTH) / SCR_HEIGHT) + pos.y * viewUp;

				// 计算出光线并进行光线追踪
				RayTracing::Ray ray(viewPos, globalPos);
				glm::vec3 color = scene.traceRay(ray);

				// 绘制该处的像素
				shader.setVec3("vertexColor", color);
				glDrawArrays(GL_POINTS, 0, 1);
			}
		}




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void resizeGL(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}