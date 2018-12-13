#include "TDObject.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TDObject::TDObject()
{
	angleX = 0.0f;
	angleY = 0.0f;
	disp = 0.0;
}

TDObject::~TDObject()
{
	delete texture;
}

void TDObject::bufferObject() {

	glGenVertexArrays(1, &vao);	// Create Vertex Array Object
	glBindVertexArray(vao);		// Bind VertexArrayObject

	glGenBuffers(3, buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, out_vertices.size() * sizeof(glm::vec3), &out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, out_uvs.size() * sizeof(glm::vec2), &out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, out_normals.size() * sizeof(glm::vec3), &out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);	// Vertices									
	glBindVertexBuffer(0, buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);	// UV									
	glBindVertexBuffer(1, buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);	// Normals									
	glBindVertexBuffer(2, buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);
}

void TDObject::readTexture(string name, GLuint textureName) {

	// Flip images as OpenGL expects 0.0 coordinates on the y-axis to be at the bottom of the image.
	stbi_set_flip_vertically_on_load(true);

	// Load image Information.
	int iWidth, iHeight, iChannels;
	unsigned char *iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);

	// Load and create a texture 
	glBindTexture(GL_TEXTURE_2D, textureName); // All upcoming operations now have effect on this texture object

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, iWidth, iHeight);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGBA, GL_UNSIGNED_BYTE, iData);

	// Set the texture wrapping parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate mipmaps (next lecture)
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);// Unbind texture when done, so we won't accidentily mess up our texture.

}

void TDObject::readObject(string name) {
	cout << "Loading model " << name << "\n";

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > obj_vertices;
	std::vector< glm::vec2 > obj_uvs;
	std::vector< glm::vec3 > obj_normals;

	std::ifstream dataFile(name);

	string rawData;		// store the raw data.
	int countLines = 0;
	if (dataFile.is_open()) {
		string dataLineRaw;
		while (getline(dataFile, dataLineRaw)) {
			rawData += dataLineRaw;
			rawData += "\n";
			countLines++;
		}
		dataFile.close();
	}

	cout << "Finished reading model file " << name << "\n";

	istringstream rawDataStream(rawData);
	string dataLine;
	int linesDone = 0;
	while (std::getline(rawDataStream, dataLine)) {
		if (dataLine.find("v ") != string::npos) {	//does this line have a vector?
			glm::vec3 vertex;

			int foundStart = dataLine.find(" ");  
			int foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; 
			foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; 
			foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_vertices.push_back(vertex);
		}
		else if (dataLine.find("vt ") != string::npos) {	// does this line have a uv coordinates?
			glm::vec2 uv;

			int foundStart = dataLine.find(" ");  
			int foundEnd = dataLine.find(" ", foundStart + 1);
			uv.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; 
			foundEnd = dataLine.find(" ", foundStart + 1);
			uv.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_uvs.push_back(uv);
		}
		else if (dataLine.find("vn ") != string::npos) { // does this line have a normal coordinates?
			glm::vec3 normal;

			int foundStart = dataLine.find(" ");  
			int foundEnd = dataLine.find(" ", foundStart + 1);
			normal.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; 
			foundEnd = dataLine.find(" ", foundStart + 1);
			normal.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; 
			foundEnd = dataLine.find(" ", foundStart + 1);
			normal.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_normals.push_back(normal);
		}
		else if (dataLine.find("f ") != string::npos) { // does this line defines a triangle face?
			string parts[3];

			int foundStart = dataLine.find(" ");  
			int foundEnd = dataLine.find(" ", foundStart + 1);
			parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; 
			foundEnd = dataLine.find(" ", foundStart + 1);
			parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; 
			foundEnd = dataLine.find(" ", foundStart + 1);
			parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			for (int i = 0; i < 3; i++) {		// for each part

				vertexIndices.push_back(stoul(parts[i].substr(0, parts[i].find("/"))));
				int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);
				if ((firstSlash + 1) != (secondSlash)) {	// there are texture coordinates.
					uvIndices.push_back(stoul(parts[i].substr(firstSlash + 1, secondSlash - firstSlash + 1)));
				}
				normalIndices.push_back(stoul(parts[i].substr(secondSlash + 1)));
			}
		}

		else if (dataLine.find("mtllib ") != string::npos) { // does this object have a material?
			materials.push_back(dataLine.substr(dataLine.find(" ") + 1));
		}

		linesDone++;

		if (linesDone % 50000 == 0) {
			cout << "Parsed " << linesDone << " of " << countLines << " from model...\n";
		}

	}

	for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
		out_vertices.push_back(obj_vertices[vertexIndices[i + 2] - 1]);
		out_normals.push_back(obj_normals[normalIndices[i + 2] - 1]);
		out_uvs.push_back(obj_uvs[uvIndices[i + 2] - 1]);

		out_vertices.push_back(obj_vertices[vertexIndices[i + 1] - 1]);
		out_normals.push_back(obj_normals[normalIndices[i + 1] - 1]);
		out_uvs.push_back(obj_uvs[uvIndices[i + 1] - 1]);

		out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
		out_normals.push_back(obj_normals[normalIndices[i] - 1]);
		out_uvs.push_back(obj_uvs[uvIndices[i + 0] - 1]);
	}

	// Load Materials
	for (unsigned int i = 0; i < materials.size(); i++) {

		std::ifstream dataFileMat(materials[i]);

		string rawDataMat;		// store the raw data.
		int countLinesMat = 0;
		if (dataFileMat.is_open()) {
			string dataLineRawMat;
			while (getline(dataFileMat, dataLineRawMat)) {
				rawDataMat += dataLineRawMat;
				rawDataMat += "\n";
			}
			dataFileMat.close();
		}

		istringstream rawDataStreamMat(rawDataMat);
		string dataLineMat;
		while (std::getline(rawDataStreamMat, dataLineMat)) {
			if (dataLineMat.find("map_Kd ") != string::npos) {	// does this line have a texture map?
				textures.push_back(dataLineMat.substr(dataLineMat.find(" ") + 1));
			}
			else if (dataLineMat.find("Ns ") != string::npos) {	// Shininess
				Ns = std::strtof(dataLineMat.substr(dataLineMat.find(" ") + 1).c_str(),0);
			}
			else if (dataLineMat.find("Ka ") != string::npos) {	// Ambient
				std::istringstream iss(dataLineMat);
				std::vector<std::string> results(std::istream_iterator<std::string>{iss},
					std::istream_iterator<std::string>());
				Ka = glm::vec3(std::strtof(results[1].c_str(), NULL), std::strtof(results[2].c_str(), NULL), std::strtof(results[3].c_str(), NULL));
			}
			else if (dataLineMat.find("Kd ") != string::npos) {	// Diffuse
				std::istringstream iss(dataLineMat);
				std::vector<std::string> results(std::istream_iterator<std::string>{iss},
					std::istream_iterator<std::string>());
				Kd = glm::vec3(std::strtof(results[1].c_str(), NULL), std::strtof(results[2].c_str(), NULL), std::strtof(results[3].c_str(), NULL));
			}
			else if (dataLineMat.find("Ks ") != string::npos) {	// specular
				std::istringstream iss(dataLineMat);
				std::vector<std::string> results(std::istream_iterator<std::string>{iss},
					std::istream_iterator<std::string>());
				Ks = glm::vec3(std::strtof(results[1].c_str(), NULL), std::strtof(results[2].c_str(), NULL), std::strtof(results[3].c_str(), NULL));
			}
		}
	}

	texture = new GLuint[textures.size()];		// Warning possible memory leak here - there is a new here, where is your delete?
	glGenTextures(textures.size(), texture);

	for (int i = 0; i < textures.size(); i++) {
		readTexture(textures[i], texture[i]);
	}

	cout << "done";
}
