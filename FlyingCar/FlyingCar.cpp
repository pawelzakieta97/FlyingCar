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
#include "Game.h"
#include "PointLight.h"
#include "Plane.h"

//#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;


GLuint LoadMipmapTexture(GLuint texId, const char* fname)
{
	int width, height;
	unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr)
		throw exception("Failed to load texture file");

	GLuint texture;
	glGenTextures(1, &texture);

	glActiveTexture(texId);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

ostream& operator<<(ostream& os, const glm::mat4& mx)
{
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
			cout << mx[row][col] << ' ';
		cout << endl;
	}
	return os;
}
Game game = Game();

const float SENSITIVITY = 0.005;





int main()
{
	std::vector<int>vec = std::vector<int>({ 1,2,3,4 });
	
	
	Mesh floor();

	//loading teapot obj file to a mesh
	Mesh teapot("teapot.obj");
	Plane plane(20, 20);

	//setting teapot color to red
	// teapot.setColor(0.7, 0.1, 0.1);

	//creating 2 game objects from the same mesh. First mesh added is assumed to be the player
	auto tpt1 = game.addMesh(&teapot);
	tpt1->castShadows = true;
	tpt1->shadowSize = 2;
	tpt1->receivesShadows = false;
	tpt1->COMoffset = glm::vec3(0, 0.2, 0);
	// tpt1->scale(glm::vec3(0.05, 0.05, 0.05));
	// auto tpt2 = game.addMesh(&teapot);
	auto planeGO = game.addMesh(&plane);
	planeGO->receivesShadows = true;
	planeGO->castShadows = false;
	//setting objects' material parameters
	tpt1->material.diffuseColor = glm::vec3(0.0, 0.1, 0.7);
	tpt1->material.specularColor = glm::vec3(0.2, 0.2, 0.2);
	tpt1->material.glossiness = 40;
	//tpt2->material.diffuseColor = glm::vec3(0.0, 0.1, 0.7);
	//tpt2->material.glossiness = 6;
	planeGO->material.diffuseColor = glm::vec3(0.5, 0.5, 0.5);
	planeGO->material.specularColor = glm::vec3(0.0, 0.0, 0.0);
	planeGO->material.glossiness = 1;

	//linking 2 teapotes (tpt1 is parent, tpt2 is child. Any transformations to tpt1 like moving, 
	// rotating or scaling will also affect tpt2)
	//game.link(tpt1, tpt2);

	//moving tpt1 to the left
	tpt1->translate(glm::vec3(0, 0, -2.5));
	//tpt1->scale(glm::vec3(10, 10, 10));
	//moving tpt2 to the right, so that it is in the middle of the scene
	//tpt2->translate(glm::vec3(0, 0, 2.5));
	//scaling down the child teapot
	//tpt2->scale(glm::vec3(0.5, 0.5, 0.5));

	auto scroll_callback = [](GLFWwindow* window, double x, double y) {
		//cout << x << "\t" << y << endl;
		if (y == -1) {
			game.camera.changeCameraDistance(1.1);
		}
		else game.camera.changeCameraDistance(0.9);
		/*game.camera.setPanPitch((y - game.HEIGHT / 2) * SENSITIVITY);
		game.camera.setPanYaw((-x + game.WIDTH / 2) * SENSITIVITY);*/
	};

	auto mouse_callback = [](GLFWwindow* window, double x, double y) {
		//cout << x << "\t" << y << endl;
		game.camera.setPanPitch((y - game.HEIGHT / 2) * SENSITIVITY);
		game.camera.setPanYaw((-x + game.WIDTH / 2) * SENSITIVITY);
	};

	auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		cout << key << endl;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (key == GLFW_KEY_W) {
			if (action == GLFW_PRESS) game.player->setVelocity(glm::vec3(0, 0, 1));
			else if (action == GLFW_RELEASE) game.player->setVelocity(glm::vec3(0, 0, 0));
		}
		if (key == GLFW_KEY_S) {
			if (action == GLFW_PRESS) game.player->setVelocity(glm::vec3(0, 0, -1));
			else if (action == GLFW_RELEASE) game.player->setVelocity(glm::vec3(0, 0, 0));
		}
		if (key == GLFW_KEY_A) {
			if (action == GLFW_PRESS) game.player->setAngularVelocity(glm::vec3(0, 1, 0));
			else if (action == GLFW_RELEASE) game.player->setAngularVelocity(glm::vec3(0, 0, 0));
		}
		if (key == GLFW_KEY_D) {
			if (action == GLFW_PRESS) game.player->setAngularVelocity(glm::vec3(0, -1, 0));
			else if (action == GLFW_RELEASE) game.player->setAngularVelocity(glm::vec3(0, 0, 0));
		}
	};

	auto onTick = []() {
		//cout << x << "\t" << y << endl;
		if (glm::length(game.player->transformation * glm::vec4(0, 0, 0, 1)) > 10) {
			game.player->transformation = glm::mat4();
		}
	};

	game.setKeyCallback(key_callback);
	game.setMouseCallback(mouse_callback);
	game.setScrollWheelCallback(scroll_callback);
	game.setOnTick(onTick);
	//adding point lights
	PointLight light(glm::vec3(6.0, 2.0, 0.0), glm::vec3(10, 8, 8));
	PointLight light2(glm::vec3(-6.0, 2.0, 0.0), glm::vec3(8, 8, 10));
	DirectionalLight dirLight(glm::vec3(5, -5, 10), glm::vec3(2, 2, 2));

	//game.addLight(&light2);
	//game.addLight(&light);
	game.addLight(&dirLight);

	game.run();
}
