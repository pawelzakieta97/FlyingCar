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
#include <vector>


#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>

class Mesh
{
public:
	
	class Vertex {

	public:
		struct Position {
			GLfloat x, y, z;
		};
		struct Normal {
			GLfloat x, y, z;
		};
		struct Color {
			GLfloat r, g, b;
		};
		struct UV {
			GLfloat u, v;
		};
		Position position;
		Normal normal;
		Color color;
		UV uv;
		Vertex(GLfloat x, GLfloat y, GLfloat z,
			GLfloat nx, GLfloat ny, GLfloat nz,
			GLfloat u, GLfloat v,
			GLfloat r, GLfloat g, GLfloat b
		);
		Vertex(Position p, Normal n, UV u, Color c);
		Vertex(Position p, Normal n, UV u);
		Vertex();
	};
	struct Triangle {
		GLuint a, b, c;
		Triangle(GLuint aa, GLuint bb, GLuint cc) : a(aa), b(bb), c(cc) {};
		Triangle() {}
	};
	GLuint VBO=0, EBO=0, VAO=0;
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<Mesh> children;
	GLfloat* verticesData = nullptr;
	int verticesDataSize = 0;
	GLuint* indicesData = nullptr;
	int indicesDataSize = 0;
	void updateData(bool normal, bool uv, bool color);
	Mesh(std::vector<Vertex> verts, std::vector<Triangle> idx) {
		vertices = verts;
		triangles = idx;
		//transformation = glm::translate(trans)
	}
	Mesh(string filename);
	Mesh() {
		vertices = std::vector<Vertex>();
		triangles = std::vector<Triangle>();
	}
	void setColor(GLfloat r, GLfloat g, GLfloat b) {
		for (std::vector<Vertex>::iterator it = vertices.begin(); it < vertices.end(); it++) {
			it->color.r = r;
			it->color.g = g;
			it->color.b = b;
		}
		updateData(true, false, true);
	}
	
};

