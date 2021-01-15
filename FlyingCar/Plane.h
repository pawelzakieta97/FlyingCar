#pragma once
#include "Mesh.h"
class Plane : public Mesh
{
public:
	GLfloat length = 1, width = 1;
	Plane(GLfloat length, GLfloat width) {
		this->length = length;
		this->width = width;
		updateVertices();
	}
	void updateVertices() {
		Vertex a, b, c, d;
		a.position.x = length / 2; a.position.z = width / 2; a.position.y = 0;
		b.position.x = -length / 2; b.position.z = width / 2; b.position.y = 0;
		c.position.x = -length / 2; c.position.z = -width / 2; c.position.y = 0;
		d.position.x = length / 2; d.position.z = -width / 2; d.position.y = 0;

		a.normal.x = 0; a.normal.y = 1; a.normal.z = 0;
		b.normal.x = 0; b.normal.y = 1; b.normal.z = 0;
		c.normal.x = 0; c.normal.y = 1; c.normal.z = 0;
		d.normal.x = 0; d.normal.y = 1; d.normal.z = 0;

		vertices = std::vector<Vertex>({ a,b,c,d });
		triangles = std::vector<Triangle>({ Triangle(3,2,1), Triangle(1,0,3) });
		updateData(true, false, true);
	}
};

