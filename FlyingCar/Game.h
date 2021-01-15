#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include "ShaderProgram.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Mesh.h"
#include "Light.h"
#include "PointLight.h"
#include <string>
#include "Camera.h"
#include "DirectionalLight.h"
//#include <stb_image.h>
//#include <filesystem.h>

using OnTickFun = std::add_pointer_t<void()>;

class Game
{public:
	struct Material {
		glm::vec3 diffuseColor = glm::vec3(0.5, 0.5, 0.5);
		glm::vec3 specularColor = glm::vec3(1, 1, 1);
		GLuint glossiness = 1;
	};
	class GameObject {
	public:
		bool receivesShadows = false;
		bool castShadows = false;
		float shadowSize = 2;
		Material material;
		Mesh* mesh;
		glm::mat4 transformation;
		glm::vec3 COMoffset = glm::vec3(0, 0, 0);
		glm::vec3 velocity = glm::vec3(0,0,0);
		glm::vec3 angularVelocity = glm::vec3(0, 0, 0);
		std::vector<GameObject*> children;
		GameObject(Mesh* mesh) {
			this->mesh = mesh;
			initBuffers();
		}
		void addChild(GameObject* mesh) {
			children.push_back(mesh);
		}
		void setVelocity(glm::vec3 vel) {
			velocity = vel;
		}
		void setAngularVelocity(glm::vec3 angVel) {
			angularVelocity = angVel;
		}
		void initBuffers() {
			//if (mesh->EBO && mesh->VAO && mesh->VBO) return;
			glGenVertexArrays(1, &(mesh->VAO));
			glGenBuffers(1, &(mesh->VBO));
			glGenBuffers(1, &(mesh->EBO));
			// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
			glBindVertexArray(mesh->VAO);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh->verticesDataSize, mesh->verticesData, GL_STATIC_DRAW);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesDataSize, mesh->indicesData, GL_STATIC_DRAW);
			//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

			// vertex geometry data
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// vertex color data
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			// vertex texture coordinates
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

			glBindVertexArray(0);
		}
		~GameObject() {
			glDeleteVertexArrays(1, &(mesh->VAO));
			glDeleteBuffers(1, &(mesh->VBO));
			glDeleteBuffers(1, &(mesh->EBO));
			for (auto child : children) {
				delete child;
			}
		}

		void translate(glm::vec3 translation) {
			transformation = glm::translate(transformation, translation);
		}
		void rotateX(float angle) {
			transformation = glm::rotate(transformation, angle, glm::vec3(1, 0, 0));
		}
		void rotateY(float angle) {
			transformation = glm::rotate(transformation, angle, glm::vec3(0, 1, 0));
		}
		void rotateZ(float angle) {
			transformation = glm::rotate(transformation, angle, glm::vec3(0, 0, 1));
		}
		void scale(glm::vec3 scale) {
			transformation = glm::scale(transformation, scale);
		}
	};

	GLuint WIDTH, HEIGHT;
	GLFWwindow* window;
	ShaderProgram* shaderProgram;
	ShaderProgram* skyboxShader;
	Camera camera;
	GameObject* player;
	OnTickFun tickFun = nullptr;
	std::vector<GameObject*> gameObjects;
	//std::vector<Light*> lights;
	std::vector<PointLight*> pointLights;
	std::vector<DirectionalLight*> directionalLights;
	unsigned int skyboxVAO = 0, skyboxVBO = 0;
	unsigned int cubemapTexture;
	Game(GLuint width = 1000, GLuint height = 600, const char* name="flying car") {
		WIDTH = width;
		HEIGHT = height;
		if (glfwInit() != GL_TRUE)
		{
			cout << "GLFW initialization failed" << endl;
			return;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, name, nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window);
		//glfwSetKeyCallback(window,fun)

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT);
		glEnable(GL_DEPTH_TEST);
		//TODO: zmienic na modyfikowalne nazwy plikow z kodami shaderow
		skyboxShader = new ShaderProgram("sky.vert", "sky.frag");
		shaderProgram = new ShaderProgram("gl_05.vert", "gl_05.frag");
		camera.setPosition(glm::vec3(0, 0, 10));
		camera.setTarget(glm::vec3(0, 0, 0));
		initSkybox();
	}

	unsigned int loadTexture(char const* path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
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

			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			SOIL_free_image_data(data);
		}

		return textureID;
	}
	unsigned int loadCubemap(vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			//unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				SOIL_free_image_data(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				SOIL_free_image_data(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
	void initSkybox() {
		
		float skyboxVertices[] = {
			// positions          
			-10.0f,  10.0f, -10.0f,
			-10.0f, -10.0f, -10.0f,
			 10.0f, -10.0f, -10.0f,
			 10.0f, -10.0f, -10.0f,
			 10.0f,  10.0f, -10.0f,
			-10.0f,  10.0f, -10.0f,

			-10.0f, -10.0f,  10.0f,
			-10.0f, -10.0f, -10.0f,
			-10.0f,  10.0f, -10.0f,
			-10.0f,  10.0f, -10.0f,
			-10.0f,  10.0f,  10.0f,
			-10.0f, -10.0f,  10.0f,

			 10.0f, -10.0f, -10.0f,
			 10.0f, -10.0f,  10.0f,
			 10.0f,  10.0f,  10.0f,
			 10.0f,  10.0f,  10.0f,
			 10.0f,  10.0f, -10.0f,
			 10.0f, -10.0f, -10.0f,

			-10.0f, -10.0f,  10.0f,
			-10.0f,  10.0f,  10.0f,
			 10.0f,  10.0f,  10.0f,
			 10.0f,  10.0f,  10.0f,
			 10.0f, -10.0f,  10.0f,
			-10.0f, -10.0f,  10.0f,

			-10.0f,  10.0f, -10.0f,
			 10.0f,  10.0f, -10.0f,
			 10.0f,  10.0f,  10.0f,
			 10.0f,  10.0f,  10.0f,
			-10.0f,  10.0f,  10.0f,
			-10.0f,  10.0f, -10.0f,

			-10.0f, -10.0f, -10.0f,
			-10.0f, -10.0f,  10.0f,
			 10.0f, -10.0f, -10.0f,
			 10.0f, -10.0f, -10.0f,
			-10.0f, -10.0f,  10.0f,
			 10.0f, -10.0f,  10.0f
		};
		// skybox VAO
		
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// load textures
		// -------------
		vector<std::string> faces
		{
			"resources/right.jpg",
			"resources/left.jpg",
			"resources/top.jpg",
			"resources/bottom.jpg",
			"resources/front.jpg",
			"resources/back.jpg"
		};
		cubemapTexture = loadCubemap(faces);
		skyboxShader->Use();
		skyboxShader->setInt("skybox", 0);
	}

	void renderSkyBox() {
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader->Use();
		auto view = glm::mat4(glm::mat3(camera.getViewMatrix())); // remove translation from the view matrix
		skyboxShader->setMat4("view", view);
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(60.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 1.0f, 100.0f);
		skyboxShader->setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

	}

	void setKeyCallback(GLFWkeyfun fun) {
		glfwSetKeyCallback(window, fun);
	}

	void setMouseCallback(GLFWcursorposfun fun) {
		glfwSetCursorPosCallback(window, fun);
	}
	void setScrollWheelCallback(GLFWscrollfun fun) {
		glfwSetScrollCallback(window, fun);
	}

	GameObject* addMesh(Mesh* mesh) {
		GameObject* newGo = new GameObject(mesh);
		addGameObject(newGo);
		return newGo;
	}

	void addGameObject(GameObject* go) {
		if (gameObjects.size() == 0) player = go;
		gameObjects.push_back(go);
	}

	~Game() {
		for (auto go : gameObjects) delete go;
	}

	void addLight(PointLight* light) {
		pointLights.push_back(light);
	}
	void addLight(DirectionalLight* light) {
		directionalLights.push_back(light);
	}
	void initLights(ShaderProgram* shader) {
		shader->Use();
		int number = 0;
		auto shader_id = shader->get_programID();
		for (auto light : pointLights) {
			string numberStr = to_string(number);
			//numberStr = "1";
			glUniform3fv(glGetUniformLocation(shader_id, ("pointLights[" + numberStr + "].position").c_str()), 1, glm::value_ptr(light->position));
			glUniform3fv(glGetUniformLocation(shader_id, ("pointLights[" + numberStr + "].intensity").c_str()), 1, glm::value_ptr(light->intensity));
			number++;
		}
		glUniform1i(glGetUniformLocation(shader_id, "pointLightCount"), (GLuint)pointLights.size());
		number = 0;
		for (auto light : directionalLights) {
			string numberStr = to_string(number);
			//numberStr = "1";
			glUniform3fv(glGetUniformLocation(shader_id, ("directionalLights[" + numberStr + "].direction").c_str()), 1, glm::value_ptr(light->direction));
			glUniform3fv(glGetUniformLocation(shader_id, ("directionalLights[" + numberStr + "].intensity").c_str()), 1, glm::value_ptr(light->intensity));
			number++;
		}
		glUniform1i(glGetUniformLocation(shader_id, "directionalLightCount"), (GLuint)directionalLights.size());
	}
	void setMaterial(ShaderProgram* shader, Material material) {
		auto shader_id = shader->get_programID();
		glUniform3fv(glGetUniformLocation(shader_id, "material.diffuseColor"), 1, glm::value_ptr(material.diffuseColor));
		glUniform3fv(glGetUniformLocation(shader_id, "material.specularColor"), 1, glm::value_ptr(material.specularColor));
		glUniform1i(glGetUniformLocation(shader_id, "material.glossiness"), material.glossiness);
	}
	void renderGameObject(GameObject* go, ShaderProgram* shader, glm::mat4 parentTransformation=glm::mat4()) {
		if (go->receivesShadows) {
			int index = 0;
			string indexStr = to_string(index);
			shader->setFloat("shadowSize", go->shadowSize);
			for (auto otherGO : gameObjects) {
				if (otherGO == go) continue;
				if (otherGO->castShadows) {
					shader->setVec3("shadowCasters[" + indexStr + "]", glm::vec3(otherGO->transformation * glm::vec4(0, 0, 0, 1))+otherGO->COMoffset);
				}
				index++;
			}
			shader->setInt("shadowCastersCount", index);
		}
		else shader->setInt("shadowCastersCount", 0);
		shader->Use();
		GLuint transformLoc = glGetUniformLocation(shader->get_programID(), "transform");
		glm::mat4 goTransformation = parentTransformation * go->transformation;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(goTransformation));
		glBindVertexArray(go->mesh->VAO);
		setMaterial(shader, go->material);
		glDrawElements(GL_TRIANGLES, go->mesh->triangles.size() * 3, GL_UNSIGNED_INT, 0);
		for (auto child : go->children) {
			renderGameObject(child, shader, goTransformation);
		}
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		//initLights(shader);
		//glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));
	}

	void link(GameObject* parent, GameObject* child) {
		auto iter = gameObjects.begin();
		while (iter < gameObjects.end()) {
			if ((*iter) == child) {
				gameObjects.erase(iter);
				break;
			}
			iter++;
		}
		parent->addChild(child);
	}

	void setOnTick(OnTickFun fun) {
		tickFun = fun;
	}
	
	void run() {

		int i = 1;
		while (!glfwWindowShouldClose(window))
		{
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			

			// Clear the colorbuffer
			
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//renderSkyBox();
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(60.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 1.0f, 100.0f);
			GLuint transformLoc = glGetUniformLocation(shaderProgram->get_programID(), "transform");
			GLint viewLoc = glGetUniformLocation(shaderProgram->get_programID(), "view");
			GLint projLoc = glGetUniformLocation(shaderProgram->get_programID(), "projection");
			GLint cameraPosLoc = glGetUniformLocation(shaderProgram->get_programID(), "cameraPos");
			GLint lightCountLoc = glGetUniformLocation(shaderProgram->get_programID(), "pointLightCount");
			
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glUniform3fv(cameraPosLoc, 1, glm::value_ptr(camera.getPosition()));
			glUniform1i(lightCountLoc, (GLuint)pointLights.size());
			//renderSkyBox();
			initLights(shaderProgram);
			//shaderProgram->Use();
			float dt = 0.001;
			glBindVertexArray(0);

			// NIE MAM POJECIA CZEMU, ALE SKYBOX MUSI BYC RENDEROWANY W TYM MIEJSCU, INACZEJ NIE DZIALA
			renderSkyBox();
			for (auto go : gameObjects) {
				go->translate(go->velocity * dt);
				go->rotateX(go->angularVelocity.x * dt);
				go->rotateY(go->angularVelocity.y * dt);
				go->rotateZ(go->angularVelocity.z * dt);
				renderGameObject(go, shaderProgram);
			}
			if (tickFun) tickFun();
			// Swap the screen buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
			
		}
		

		glfwTerminate();
	}
};

