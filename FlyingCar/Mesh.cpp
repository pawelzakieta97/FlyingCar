#include "Mesh.h"

Mesh::Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat u=0, GLfloat v=0, GLfloat r = 0.5, GLfloat g = 0.5, GLfloat b = 0.5)
{
	position.x = x;
	position.y = y;
	position.z = z;
	normal.x = nx;
	normal.y = ny;
	normal.z = nz;
	color.r = r;
	color.g = g;
	color.b = b;
	uv.u = u;
	uv.v = v;
}

Mesh::Vertex::Vertex(Position p, Normal n, UV u, Color c)
{
	position = p;
	normal = n;
	color = c;
	uv = u;
}

Mesh::Vertex::Vertex(Position p, Normal n, UV u)
{
	position = p;
	normal = n;
	uv = u;
	color.r = 0.5;
	color.g = 0.5;
	color.b = 0.5;
}

Mesh::Vertex::Vertex()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	normal.x = 0;
	normal.y = 0;
	normal.z = 0;
	color.r = 0.5;
	color.g = 0.5;
	color.b = 0.5;
	uv.u = 0;
	uv.v = 0;
}

void Mesh::updateData(bool normal = true, bool uv = false, bool color = true)
{
	
	int lineLength = 3;
	if (normal) lineLength += 3;
	if (uv) lineLength += 2;
	if (color) lineLength += 3;

	if (verticesData) {
		delete verticesData;
	}
	verticesData = new GLfloat[vertices.size()*lineLength];
	verticesDataSize = vertices.size() * lineLength * sizeof(GLfloat);
	int index = 0;
	for (auto vertex : vertices) {
		verticesData[index] = vertex.position.x;
		verticesData[index+1] = vertex.position.y;
		verticesData[index+2] = vertex.position.z;
		index += 3;
		if (color) {
			verticesData[index] = vertex.color.r;
			verticesData[index + 1] = vertex.color.g;
			verticesData[index + 2] = vertex.color.b;
			index += 3;
		}
		if (normal) {
			verticesData[index] = vertex.normal.x;
			verticesData[index + 1] = vertex.normal.y;
			verticesData[index + 2] = vertex.normal.z;
			index += 3;
		}
		if (uv) {
			verticesData[index] = vertex.uv.u;
			verticesData[index + 1] = vertex.uv.v;
			index += 2;
		}
		
	}


	if (indicesData) {
		delete indicesData;
	}
	indicesData = new GLuint[triangles.size() * 3];
	indicesDataSize = triangles.size() * 3 * sizeof(GLuint);
	index = 0;
	for (auto triangle : triangles) {
		indicesData[index] = triangle.a;
		indicesData[index+1] = triangle.b;
		indicesData[index+2] = triangle.c;
		index += 3;
	}
}

Mesh::Mesh(string filename)
{
	std::ifstream infile(filename);
	std::string line;


	std::vector<Mesh::Vertex::Position> positions;
	std::vector<Mesh::Vertex::Normal> normals;
	std::vector<Mesh::Vertex::UV> uvs;

	while (std::getline(infile, line)) {
		if (line[0] == 'v') break;
	}
	while (line[0] == 'v') {
		string tmp;
		Mesh::Vertex::Position position;
		std::istringstream(line) >> tmp >> position.x >> position.y >> position.z;
		positions.push_back(position);
		std::getline(infile, line);
	}

	while (std::getline(infile, line)) {
		if (line.length()>1 && line[1] == 'n') break;
	}
	while (line.length() > 1 && line[1] == 'n') {
		string tmp;
		Mesh::Vertex::Normal normal;
		std::istringstream(line) >> tmp >> normal.x >> normal.y >> normal.z;
		normals.push_back(normal);
		std::getline(infile, line);
	}

	while (std::getline(infile, line)) {
		if (line.length() > 1 && line[1] == 't') break;
	}
	while (line.length() > 1 && line[1] == 't') {
		string tmp;
		GLfloat tmp1;
		Mesh::Vertex::UV uv;
		std::istringstream(line) >> tmp >> uv.u >> uv.v >> tmp1;
		uvs.push_back(uv);
		std::getline(infile, line);
	}

	while (std::getline(infile, line)) {
		if (line.length() > 0 && line[0] == 'f') break;
	}
	while (line.length() > 0 && line[0] == 'f') {
		
		std::stringstream ss(line);
		string tmp;
		ss >> tmp;
		string vertex_info;
		while (ss >> vertex_info) {
			std::stringstream vertex_ss(vertex_info);
			std::string token;
			string delimiter = "/";
			int pos_idx, n_idx, uv_idx;
			vertex_ss >> pos_idx;
			vertex_ss.ignore();
			vertex_ss >> uv_idx;
			vertex_ss.ignore();
			vertex_ss >> n_idx;
			vertex_ss.ignore();
			vertices.push_back(Mesh::Vertex(positions[pos_idx-1], normals[n_idx-1], uvs[uv_idx-1]));
		}
		Triangle triangle;
		triangle.a = vertices.size() - 3;
		triangle.b = vertices.size() - 2;
		triangle.c = vertices.size() - 1;
		triangles.push_back(triangle);
		std::getline(infile, line);
	}
	infile.close();
	updateData(true, false, true);

}

