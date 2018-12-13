#ifndef FILE_FROM_OBJ
#define FILE_FROM_OBJ
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iterator> 
#endif

using namespace std;

class TDObject
{
public:

	GLuint          vao;
	GLuint          buffer[2];
	float			angleX;
	float			angleY;
	float			disp;
	vector < glm::vec3 > out_vertices;
	vector< string > materials, textures;
	vector < glm::vec2 > out_uvs;
	vector < glm::vec3 > out_normals;
	GLuint          program;
	GLint           mv_location;
	GLint           proj_location;
	GLint			tex_location;
	GLuint		matColor_location;
	GLuint		lightColor_location;
	GLuint * texture;

	//material
	GLfloat Ns = 30.0f;
	glm::vec3 Ka, Kd, Ks;

	void bufferObject();
	void readTexture(string name, GLuint textureName);
	void readObject(string name);

	TDObject();
	~TDObject();
};

