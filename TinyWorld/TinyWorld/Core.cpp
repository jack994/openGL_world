
#include "TDObject.h"
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <stdio.h>
#include <stdlib.h>
#define M_PI 3.1415926535897932384626433832795

GLFWwindow*		window;
int				windowWidth = 1200;
int				windowHeight = 800;
bool			running = true;

float           aspect;
glm::mat4		proj_matrix;
glm::mat4		mv_matrix;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	
float pitch = 0.0f;
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
float fov = 45.0f;

// moon radius 
GLfloat         radius = 4.5f;

// Light (moon)
glm::vec3		lightDisp = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		la = glm::vec3(0.25f, 0.25f, 0.25f);
glm::vec3		ld = glm::vec3(0.8f, 0.8f, 0.8f);
glm::vec3		ls = glm::vec3(1.0f, 1.0f, 1.0f);

// Light (torch)
glm::vec3		ldT = glm::vec3(1.0f, 0.20f, 0.20f);
glm::vec3		lsT = glm::vec3(1.0f, 0.20f, 0.20f);

// torch on or off
GLfloat OnOff = 0.0f;
int ds = 0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
bool   keyStatus[1024];

//animation interpolation variables
GLfloat startAngle = 0.0f;
GLfloat endAngle = 360.0f;
GLfloat startTime = 0.0f;
GLfloat endTime = 20.0f;
GLfloat currTimeInter = 0.0f;
int interpolationNumber = 1;
int currInterpol = interpolationNumber;

const int numOfObj = 5;

TDObject objectModel[numOfObj];
TDObject lightModel;

glm::vec3 modelPositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(-0.3f,  0.82f, -0.5f),
	glm::vec3(-0.3f,  0.76f, 0.1f) };

vector < glm::vec4 > instancedTreesPositions = {
	glm::vec4(0.2f,  0.76f,  0.0f, 20.0f),
	glm::vec4(0.4f,  0.76f,  0.2f, 40.0f),
	glm::vec4(0.3f,  0.74f, -0.9f, 130.0f),
	glm::vec4(0.1f,  0.73f, -1.5f, 100.0f),
	glm::vec4(0.0f,  0.72f,  -1.7f, 25.0f),
	glm::vec4(0.2f,  0.76f, -0.4f, 320.0f),
	glm::vec4(-1.0f,  0.75f, -0.3f, 70.0f),
	glm::vec4(-0.8f,  0.75f, -0.4f, 150.0f),
	glm::vec4(-0.8f,  0.75f, 0.1f, 10.0f),
	glm::vec4(-0.5f,  0.72f, -1.4f, 65.0f),
	glm::vec4(-0.9,  0.74f, 0.0f, 0.0f),
    glm::vec4(-0.6f,  0.75f, 0.0f, 270.0f),
glm::vec4(-0.7f,  0.75f, 0.1f, 200.0f) ,
glm::vec4(0.4f,  0.76f, 0.0f, 250.0f) ,
glm::vec4(-0.1f,  0.72f, -1.6f, 70.0f) ,
glm::vec4(-0.2f,  0.75f, -0.4f, 270.0f) ,
glm::vec4(-0.6f,  0.75f, 0.1f, 10.0f) };

vector < glm::mat4 > mv_mat_instanced;
vector < glm::mat4 > mv_mat_clouds;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void errorCallbackGLFW(int error, const char* description);
void hintsGLFW();
void endProgram();
string readShader(string name);
unsigned int compileShader(unsigned int type, const string& source);
unsigned int createShader(const string& vertexShader, const string& fragmentShader);
glm::vec4 colorCalc(glm::vec4 from, glm::vec4 to, GLfloat currT, GLfloat startT, GLfloat endT);
glm::vec4 interColor(GLfloat currT);
GLfloat interpolate(GLfloat currT);
void render(double currentTime);
void update(double currentTime);
void setupRender();
void calculateInstMatrix();
void calculateInstClouds();
void startup();
void debugGL();

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

unsigned int compileShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char * message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile shader: " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader." << endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

unsigned int createShader(const string& vertexShader, const string& fragmentShader) {

	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void startup() {

	calculateInstMatrix();
	calculateInstClouds();

	string vs_text = readShader("vs_model.glsl");
	string vs_textINST = readShader("vs_model_instanced.glsl");
	string fs_text = readShader("fs_shaderTorch.glsl");

	for (int i = 0; i < numOfObj; i++) {		
		if (i == 1 || i == 4) { //tree and cloud to instance
			objectModel[i].program = createShader(vs_textINST, fs_text);
		}
		else {
			objectModel[i].program = createShader(vs_text, fs_text);
		}
	}

	// Read object files
	objectModel[0].readObject("island.obj");
	objectModel[1].readObject("tree.obj");
	objectModel[2].readObject("castle.obj");
	objectModel[3].readObject("inter.obj");
	objectModel[4].readObject("cloud.obj");

	for (int i = 0; i < numOfObj; i++) {
		objectModel[i].mv_location = glGetUniformLocation(objectModel[i].program, "mv_matrix");
		objectModel[i].proj_location = glGetUniformLocation(objectModel[i].program, "proj_matrix");
		objectModel[i].tex_location = glGetUniformLocation(objectModel[i].program, "tex");
		objectModel[i].lightColor_location = glGetUniformLocation(objectModel[i].program, "ia");
		objectModel[i].lightColor_location = glGetUniformLocation(objectModel[i].program, "ka");
	}

	string vs_text_light = readShader("vs_light.glsl");
	string fs_text_light = readShader("fs_light.glsl");

    lightModel.program = createShader(vs_text_light, fs_text_light);

	lightModel.readObject("moon.obj");

	lightModel.mv_location = glGetUniformLocation(lightModel.program, "mv_matrix");
	lightModel.proj_location = glGetUniformLocation(lightModel.program, "proj_matrix");
	lightModel.tex_location = glGetUniformLocation(lightModel.program, "tex");

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);

	for (int i = 0; i < numOfObj; i++) {
		objectModel[i].bufferObject();
	}

	lightModel.bufferObject();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

//in loop before render, checks button clicks
void update(double currentTime) {
	// calculate movement
	GLfloat cameraSpeed = 1.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) cameraPos += cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_S]) cameraPos -= cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_A]) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	else {	
		// change Interpolation strategy
		if (keyStatus[GLFW_KEY_1])
			interpolationNumber = 1;
		if (keyStatus[GLFW_KEY_2])
			interpolationNumber = 2;
		// turn on and off the torch
		if (keyStatus[GLFW_KEY_K]) {
			OnOff = 1.0f;
		}
		if (keyStatus[GLFW_KEY_L]) {
			OnOff = 0.0f;
		}
		if (keyStatus[GLFW_KEY_P]) {
			ds = 1;
		}
		if (keyStatus[GLFW_KEY_O]) {
			ds = 0;
			ldT = glm::vec3(0.2f, 0.20f, 1.00f);
			lsT = glm::vec3(0.2f, 0.20f, 1.00f);
		}
		if (keyStatus[GLFW_KEY_I]) {
			ds = 0;
			ldT = glm::vec3(1.0f, 0.20f, 0.20f);
			lsT = glm::vec3(1.0f, 0.20f, 0.20f);
		}
	}
}

void render(double currentTime) {

	glViewport(0, 0, windowWidth, windowHeight);

	// interpolation
	if (currTimeInter >= endTime) {
		currTimeInter = 0.0f;
		currInterpol = interpolationNumber;
	}
	currTimeInter += deltaTime;
	GLfloat angle = interpolate(currTimeInter);

	if (ds) {
		glm::vec3 fr = glm::vec3(0.3f, 0.80f, 0.00f);
		glm::vec3 tt = glm::vec3(1.0f, 0.00f, 5.00f);
		glm::vec3 discoLight;

		if((GLfloat)fmod((float)currTimeInter, 5.0) < 2.5f)
			discoLight = glm::vec3(colorCalc(glm::vec4(fr, 1.0f), glm::vec4(tt, 1.0f), (GLfloat)fmod((float)currTimeInter, 5.0), 0.0f, 2.5f));
		else
			discoLight = glm::vec3(colorCalc(glm::vec4(tt, 1.0f), glm::vec4(fr, 1.0f), (GLfloat)fmod((float)currTimeInter, 5.0), 2.5f, 5.0f));
		
		ldT = discoLight;
		lsT = discoLight;	
	}	

	// Clear colour buffer
	glm::vec4 backgroundColor = interColor(currTimeInter);
	glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear Deep buffer
	static const GLfloat one = 1.0f; 
	glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	glUseProgram(lightModel.program);
	glBindVertexArray(lightModel.vao);
	glUniformMatrix4fv(lightModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightModel.texture[0]);
	glUniform1i(lightModel.tex_location, 0);

	//circular rotation of the moon
	lightDisp.x = cos(glm::radians(angle)) * radius;
	lightDisp.y = sin(glm::radians(angle)) * radius;

	glm::mat4 modelMatrixLight = glm::translate(glm::mat4(1.0f), glm::vec3(lightDisp.x, lightDisp.y, lightDisp.z));
	glm::mat4 mv_matrixLight = viewMatrix * modelMatrixLight;

	glUniformMatrix4fv(lightModel.mv_location, 1, GL_FALSE, &mv_matrixLight[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, lightModel.out_vertices.size());

	for (int i = 0; i < numOfObj; i++) {

		glUseProgram(objectModel[i].program);
		glBindVertexArray(objectModel[i].vao);
		glUniformMatrix4fv(objectModel[i].proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		objectModel[i].mv_location = glGetUniformLocation(objectModel[i].program, "mv_matrix");
		objectModel[i].proj_location = glGetUniformLocation(objectModel[i].program, "proj_matrix");
		objectModel[i].tex_location = glGetUniformLocation(objectModel[i].program, "tex");
		objectModel[i].lightColor_location = glGetUniformLocation(objectModel[i].program, "ia");
		objectModel[i].lightColor_location = glGetUniformLocation(objectModel[i].program, "ka");

		glUniformMatrix4fv(objectModel[i].proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		glUniform4f(glGetUniformLocation(objectModel[i].program, "viewPosition"), cameraPos.x, cameraPos.y, cameraPos.z, 1.0);
		glUniform4f(glGetUniformLocation(objectModel[i].program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0); //use light current position
		glUniform4f(glGetUniformLocation(objectModel[i].program, "ka"), objectModel[i].Ka.r, objectModel[i].Ka.g, objectModel[i].Ka.b, 1.0);
		glUniform3f(glGetUniformLocation(objectModel[i].program, "la"), la.r, la.g, la.b);
		glUniform4f(glGetUniformLocation(objectModel[i].program, "kd"), objectModel[i].Kd.r, objectModel[i].Kd.g, objectModel[i].Kd.b, 1.0);
		glUniform3f(glGetUniformLocation(objectModel[i].program, "ld"), ld.r, ld.g, ld.b);
		glUniform3f(glGetUniformLocation(objectModel[i].program, "ldT"), ldT.r, ldT.g, ldT.b);
		glUniform4f(glGetUniformLocation(objectModel[i].program, "ks"), objectModel[i].Ks.r, objectModel[i].Ks.g, objectModel[i].Ks.b, 1.0);
		glUniform3f(glGetUniformLocation(objectModel[i].program, "ls"), ls.r, ls.g, ls.b);
		glUniform3f(glGetUniformLocation(objectModel[i].program, "lsT"), lsT.r, lsT.g, lsT.b);
		glUniform1f(glGetUniformLocation(objectModel[i].program, "shininess"), objectModel[i].Ns);

		glUniform1f(glGetUniformLocation(objectModel[i].program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(objectModel[i].program, "lightLinear"), 0.022f);
		glUniform1f(glGetUniformLocation(objectModel[i].program, "lightQuadratic"), 0.0019f);

		glUniform4f(glGetUniformLocation(objectModel[i].program, "lightPositionT"), cameraPos.x, cameraPos.y, cameraPos.z, 1.0);
		glUniform4f(glGetUniformLocation(objectModel[i].program, "lightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
		glUniform1f(glGetUniformLocation(objectModel[i].program, "lightSpotCutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(objectModel[i].program, "lightSpotOuterCutOff"), glm::cos(glm::radians(15.0f)));
		glUniform1f(glGetUniformLocation(objectModel[i].program, "onoff"), OnOff); //is torch on or off?
			
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, objectModel[i].texture[0]);
		glUniform1i(objectModel[i].tex_location, 0);

		if (i == 1) {
			glUniformMatrix4fv(glGetUniformLocation(objectModel[i].program, "matArray"),
				mv_mat_instanced.size(), GL_FALSE, glm::value_ptr(mv_mat_instanced[0]));
			glUniformMatrix4fv(glGetUniformLocation(objectModel[i].program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
			glDrawArraysInstanced(GL_TRIANGLES, 0, objectModel[i].out_vertices.size(), mv_mat_instanced.size());
		}
		else if (i == 4) {
			glUniformMatrix4fv(glGetUniformLocation(objectModel[i].program, "matArray"),
				mv_mat_clouds.size(), GL_FALSE, glm::value_ptr(mv_mat_clouds[0]));
			glUniformMatrix4fv(glGetUniformLocation(objectModel[i].program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
			glDrawArraysInstanced(GL_TRIANGLES, 0, objectModel[i].out_vertices.size(), mv_mat_clouds.size());
		}
		else {
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
			modelMatrix = glm::translate(modelMatrix, modelPositions[i]);

			glm::mat4 mv_matrix = viewMatrix * modelMatrix;

			glUniformMatrix4fv(glGetUniformLocation(objectModel[i].program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(objectModel[i].program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, objectModel[i].out_vertices.size());
		}	
	}
}

//change moon/sun interpolation between linear an cosine
GLfloat interpolate(GLfloat currT) {

	GLfloat xNorm = (currT - startTime) / (endTime - startTime);
	GLfloat xRes;

	if (currInterpol == 1)
		xRes = xNorm;
	else if (currInterpol == 2)
		xRes = (1 - cos(xNorm * M_PI)) / 2;

	GLfloat newAngle = startAngle * (1 - xRes) + (endAngle * xRes);
	return newAngle;
}

glm::vec4 colorCalc(glm::vec4 from, glm::vec4 to, GLfloat currT, GLfloat startT, GLfloat endT) {

	GLfloat xNorm = (currT - startT) / (endT - startT);
	GLfloat xRes = xNorm;

	if (currInterpol == 1) {
		xRes = xNorm;
	}
	else if (currInterpol == 2) {
		xRes = (1 - cos(xNorm * M_PI)) / 2;
	}
	GLfloat new1 = from.x * (1 - xRes) + (to.x * xRes);
	GLfloat new2 = from.y * (1 - xRes) + (to.y * xRes);
	GLfloat new3 = from.z * (1 - xRes) + (to.z * xRes);

	return glm::vec4(new1, new2, new3, to.w);
}

// interpolation between colors, sunset and sunrise orange, midday blue, night black
glm::vec4 interColor(GLfloat currT) {

	if (currT < endTime / 4.0f) {
		glm::vec4 from(1.0f, 0.6f, 0.2f, 1.0f);
		glm::vec4 to(0.4f, 0.78f, 1.0f, 1.0f);
		return colorCalc(from, to, currT, startTime, endTime / 4.0f);
	}
	else if (currT < endTime / 2.0f) {
		glm::vec4 from(0.4f, 0.78f, 1.0f, 1.0f);
		glm::vec4 to(1.0f, 0.6f, 0.2f, 1.0f);
		return colorCalc(from, to, currT, startTime + (endTime / 4.0f), endTime / 2.0f);
	}
	else if (currT < (endTime / 4.0f) * 3.0f) {
		glm::vec4 from(1.0f, 0.6f, 0.2f, 1.0f);
		glm::vec4 to(0.0f, 0.0f, 0.2f, 1.0f);
		return colorCalc(from, to, currT, startTime + (endTime / 2.0f), (endTime / 4.0f) * 3.0f);
	}
	else {
		glm::vec4 from(0.0f, 0.0f, 0.2f, 1.0f);
		glm::vec4 to(1.0f, 0.6f, 0.2f, 1.0f);
		return colorCalc(from, to, currT, startTime + ((endTime / 4.0f) * 3.0f), endTime);
	}
}


// calculate the model matrices for the trees positions,
// needed for INSTANCING of the trees. patricess will be passed to VS
void calculateInstMatrix() {

	for (int i = 0; i < instancedTreesPositions.size(); i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(instancedTreesPositions[i].x, instancedTreesPositions[i].y, instancedTreesPositions[i].z));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(instancedTreesPositions[i].w), glm::vec3(0.0f, 1.0f, 0.0f));
		mv_mat_instanced.push_back(modelMatrix);
	}
}

// calculate the model matrices for the clouds positions,
// needed for INSTANCING of the clouds. matrices will be passed to VS
void calculateInstClouds() {

	for (int i = 0; i < 50; i++) {

		GLfloat rx = -15.0f + (static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX / 30.0f)));
		GLfloat ry = 2.0f + (static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX / 10.0f)));
		GLfloat rz = -15.0f + (static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX / 30.0f)));
		GLfloat rot = static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX / 360.0f));

		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(rx, ry, rz));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		mv_mat_clouds.push_back(modelMatrix);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this function is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int mouseX = static_cast<int>(xpos);
	int mouseY = static_cast<int>(ypos);

	if (firstMouse) {
		lastX = (GLfloat)mouseX; 
		lastY = (GLfloat)mouseY; firstMouse = false;
	}

	GLfloat xoffset = mouseX - lastX;
	GLfloat yoffset = lastY - mouseY; // Reversed
	lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity; 
	yoffset *= sensitivity;

	yaw += xoffset; 
	pitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	int yoffsetInt = static_cast<int>(yoffset);

	fov += yoffsetInt;
	if (fov >= 1.0f && fov <= 45.0f) fov -= (GLfloat)yoffset;
	if (fov <= 1.0f) fov = 1.0f;
	if (fov >= 45.0f) fov = 45.0f;
	proj_matrix = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;
	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS)
		keyStatus[key] = true;
	else if (action == GLFW_RELEASE)
		keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}


void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);	
	glfwTerminate();
}

void setupRender() {
	glfwSwapInterval(1); // Only render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}


void debugGL() {
	// Debugging info
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

int main()
{
	//capture the mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!glfwInit()) {			
		cout << "Could not initialise GLFW...";
		return 0;
	}
	// Display all GLFW errors.
	glfwSetErrorCallback(errorCallbackGLFW);
	// Setup glfw with various hints.
	hintsGLFW();										

	string title = "My OpenGL Application";
	/*const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	windowWidth = mode->width;
	windowHeight = mode->height;
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL);*/
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {		
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}
	// making the OpenGL context current
	glfwMakeContextCurrent(window);		

	glewExperimental = GL_TRUE;
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {		
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}
	// Setup callback to catch openGL errors.
	debugGL();										
	// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Set callback for resize		
	glfwSetKeyCallback(window, onKeyCallback);					       // Set Callback for keys
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	   // Set mouse cursor FPS.

	setupRender();	// setup some render variables.
	startup();		// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	do { // run until the window is closed (main loop)
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();           
		update(currentFrame);	 // update (button clicked etc)
		render(currentFrame);	 // call render function.
		glfwSwapBuffers(window); // swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	// Close and clean
	endProgram();			

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}