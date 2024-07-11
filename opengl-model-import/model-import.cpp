#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include <string>
#include "Model.h"

const int screenWidth = 800;
const int screenHeight = 600;

Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastMousePosX = (float)screenWidth / 2.0f;
float lastMousePosY = (float)screenHeight / 2.0f;
bool firstMoveMouse = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xPosDouble, double yPosDouble);

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

glm::vec3 calculateNormalVector(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 u = p1 - p2;
	glm::vec3 v = p3 - p2;
	return glm::normalize(glm::cross(u, v));
}

unsigned int loadTexture(char const* path);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Lighting OpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader lightShader("lightShader.vs", "lightShader.fs");
	Shader sourceShader("sourceShader.vs", "sourceShader.fs");

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	float textures[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	float normals[sizeof(vertices) / sizeof(float)] = {};

	for (int i = 0; i < sizeof(vertices) / sizeof(float); i += 9)
	{
		std::cout << i << std::endl;
		glm::vec3 p1 = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
		glm::vec3 p2 = glm::vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
		glm::vec3 p3 = glm::vec3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

		glm::vec3 normal = calculateNormalVector(p1, p2, p3);
		normal = glm::mat3(glm::transpose(glm::inverse(glm::mat4(1.0f)))) * normal;

		normals[i] = normal.x;
		normals[i + 1] = normal.y;
		normals[i + 2] = normal.z;

		normals[i + 3] = normal.x;
		normals[i + 4] = normal.y;
		normals[i + 5] = normal.z;

		normals[i + 6] = normal.x;
		normals[i + 7] = normal.y;
		normals[i + 8] = normal.z;

		std::cout <<
			normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << "\n" <<
			normals[i + 3] << " " << normals[i + 4] << " " << normals[i + 5] << "\n" <<
			normals[i + 6] << " " << normals[i + 7] << " " << normals[i + 8] << std::endl;
	}

	// Load and Create Texture
	// -----------------------
	stbi_set_flip_vertically_on_load(true);

	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");
	unsigned int emissionMap = loadTexture("matrix.jpg");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, emissionMap);

	lightShader.use();
	lightShader.setInt("material.diffuse", 0);
	lightShader.setInt("material.specular", 1);
	lightShader.setInt("material.emission", 2);
	// ---------------------

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int normalVBO;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	unsigned int textureVBO;
	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	unsigned int sourceVAO;
	glGenVertexArrays(1, &sourceVAO);
	glBindVertexArray(sourceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	lightShader.use();

	glm::mat4 view = cam.GetViewMatrix();
	lightShader.setMat4("view", view);

	lightShader.setFloat("material.shininess", 32.0f);

	glm::vec3 lightPosition = glm::vec3(-0.2f, -1.0f, -0.3f);

	lightShader.setVec3("dirLight.direction", lightPosition);
	lightShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	lightShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	lightShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec3 pointLightColors[] = {
	glm::vec3(0.2f, 0.2f, 0.6f),
	glm::vec3(0.3f, 0.3f, 0.7f),
	glm::vec3(0.0f, 0.0f, 0.3f),
	glm::vec3(0.4f, 0.4f, 0.4f)
	};

	for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); i++)
	{
		glm::vec3 viewPosition = glm::vec3(view * glm::vec4(pointLightPositions[i], 1.0f));
		lightShader.setVec3("pointLights[" + std::to_string(i) + "].viewPosition", viewPosition);

		lightShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
		lightShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
		lightShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);

		lightShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.1f * pointLightColors[i]);
		lightShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
		lightShader.setVec3("pointLights[" + std::to_string(i) + "].specular", pointLightColors[i]);
	}

	lightShader.setFloat("flashLight.constant", 1.0f);
	lightShader.setFloat("flashLight.linear", 0.09f);
	lightShader.setFloat("flashLight.quadratic", 0.032f);

	lightShader.setVec3("flashLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	lightShader.setVec3("flashLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader.setVec3("flashLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	lightShader.setFloat("flashLight.cutOff", glm::cos(glm::radians(12.5f)));
	lightShader.setFloat("flashLight.outerCutOff", glm::cos(glm::radians(14.0f)));

	glm::vec3 viewPosition = glm::vec3(view * glm::vec4(cam.Position, 1.0f));
	glm::vec3 viewDirection = glm::mat3(glm::transpose(glm::inverse(view))) * cam.Front;
	lightShader.setVec3("flashLight.viewPosition", viewPosition);
	lightShader.setVec3("flashLight.viewDirection", viewDirection);

	const std::string modelPath = "backpack/backpack.obj";
	Model backpackModel(modelPath);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		float time = static_cast<float>(glfwGetTime());
		deltaTime = time - lastFrame;
		lastFrame = time;

		lightShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		lightShader.setMat4("projection", projection);

		glm::mat4 view = cam.GetViewMatrix();
		lightShader.setMat4("view", view);

		glm::vec3 viewPosition = glm::vec3(view * glm::vec4(cam.Position, 1.0f));
		glm::vec3 viewDirection = glm::mat3(glm::transpose(glm::inverse(view))) * cam.Front;
		lightShader.setVec3("flashLight.viewPosition", viewPosition);
		lightShader.setVec3("flashLight.viewDirection", viewDirection);

		glBindVertexArray(lightVAO);
		
		for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++)
		{
			glm::vec3 cubePos = cubePositions[i];
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(22.5f) * (float)time,
				glm::vec3((float)(i % 4) / 3.0f, (float)(i % 6) / 5.0f, (float)(i % 9) / 8.0f));
			
			lightShader.setFloat("time", time * (i + 1) * 0.1f);
			lightShader.setMat4("model", model);

			glm::mat4 normalMatrix = glm::mat3(1.0f);
			normalMatrix = glm::transpose(glm::inverse(view * model));
			lightShader.setMat4("normalMatrix", normalMatrix);

			// ---------
			// Setup Texture boxes

			glActiveTexture(GL_TEXTURE0);
			lightShader.setInt("material.texture_diffuse1", 0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);

			glActiveTexture(GL_TEXTURE1);
			lightShader.setInt("material.texture_specular1", 1);
			glBindTexture(GL_TEXTURE_2D, specularMap);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, emissionMap);
			lightShader.setInt("material.texture_emission", 2);

			// ----------

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		sourceShader.use();

		for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			sourceShader.setMat4("model", model);
			sourceShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

			glBindVertexArray(sourceVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		sourceShader.setMat4("view", view);
		sourceShader.setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2f));
		sourceShader.setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f));
		sourceShader.setMat4("model", model);

		glBindVertexArray(sourceVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, 5.0f, 0.0f));
		model = glm::rotate(model, glm::radians(22.5f), glm::vec3(1.0f));
		lightShader.setMat4("model", model);
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(view * model));
		lightShader.setMat4("normalMatrix", normalMatrix);
		backpackModel.Draw(lightShader);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//camPos += camFront * camSpeed * deltaTime;
		cam.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//camPos += camFront * -camSpeed * deltaTime;
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//camPos += glm::cross(camFront, camUp) * camSpeed * deltaTime;
		cam.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//camPos += glm::cross(camFront, camUp) * -camSpeed * deltaTime;
		cam.ProcessKeyboard(LEFT, deltaTime);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosDouble, double yPosDouble)
{
	float xPos = static_cast<float>(xPosDouble);
	float yPos = static_cast<float>(yPosDouble);

	if (firstMoveMouse)
	{
		lastMousePosX = xPos;
		lastMousePosY = yPos;
		firstMoveMouse = false;
	}

	float offsetX = xPos - lastMousePosX;
	float offsetY = yPos - lastMousePosY;
	lastMousePosX = xPos;
	lastMousePosY = yPos;

	cam.ProcessMouseMovement(offsetX, -offsetY);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	cam.ProcessMouseScroll(static_cast<float>(yOffset));
}