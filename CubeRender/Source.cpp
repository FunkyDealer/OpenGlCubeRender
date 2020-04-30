#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // value_ptr

using namespace std;
using namespace glm;

int screenWidth = 800;
int screenHeight = 600;
char title[20] = "Exercicio Cubo";

void init();
void display(vector<vec3> obj, mat4 mvp);
void InputManager(GLFWwindow* window, int key, int scancode, int action, int mods);
void modsInput(GLFWwindow* window, unsigned int codepoint, int mods);
void setFullScreen(GLFWwindow* window);
void setWindowedScreen(GLFWwindow* window);
void scrollCallback(GLFWwindow * window, double xoffset, double yoffset);
vector<vec3> Load3dModel();

GLfloat ZOOM = 10.0f;
GLfloat ANGLE = 0.0f; //ANGLE OF MODEL
float ROTATESPEED = 0.01f;
vec3 DEFAULT_DIR = vec3(1.0f, 1.0f, 0.0f);


vec3 UP = vec3(0.0f, 1.0f, 0.0f);
vec3 DOWN = vec3(0.0f, -1.0f, 0.0f);
vec3 RIGHT = vec3(1.0f, 0.0f, 0.0f);
vec3 LEFT = vec3(-1.0f, 1.0f, 0.0f);

float nearPlane = 0.1f;
float farPlane = 100.f;

float aspectRatio = float(screenWidth) / float(screenHeight);

int main() {
	GLFWwindow *window;

	if (!glfwInit()) return -1;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor(); //gets monitor
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor); //Gets Video Settings

	cout << "w: " << videoMode->width << "h: " << videoMode->height << "@ " << videoMode->refreshRate << "fps" << endl;	

	window = glfwCreateWindow(screenWidth, screenHeight, title, nullptr, nullptr); //Creates window with glfw


	if (!window) { //If window is null
		glfwTerminate(); //Terminate glfw //~bug preventing measure
		return -1;
	}

	glfwMakeContextCurrent(window); //make the context the window

	glewInit();
	init();

	//Callback Functions
	glfwSetKeyCallback(window, InputManager); //Input with keys
	glfwSetScrollCallback(window, scrollCallback); //Input with mousewheel
	glfwSetCharModsCallback(window, modsInput); //Input with Mods

	mat4 projection = perspective(radians(45.0f), aspectRatio, nearPlane, farPlane);


	glfwSwapInterval(1); //sets v-Sync

	

	while (!glfwWindowShouldClose(window)) { //Indica pedido de fecho glfwSetWindowShouldClose(window, 1); //Pede para fechar

		// View
		mat4 view = lookAt(
			vec3(0.0f, 0.0f, ZOOM),	// Posicao da cmaara no mundo
			vec3(0.0f, 0.0f, -1.0f),	// Direcao para a qual a camara esta apontada
			vec3(0.0f, 1.0f, 0.0f)		// Vector vertical
		);

		projection = perspective(radians(45.0f), aspectRatio, nearPlane, farPlane);

		mat4 LocalWorld = mat4(1.0f); //Model World //identity matrix

		LocalWorld = rotate(LocalWorld, ANGLE += ROTATESPEED, normalize(RIGHT + UP)); //Rotate Model Automatically

		mat4 mvp = projection * view * LocalWorld; //

		vector<vec3> obj = Load3dModel();

		display(obj, mvp);

		glfwSwapBuffers(window); //Buffers
		glfwPollEvents(); //Events
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
}


void display(vector<vec3> obj, mat4 mvp) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float *vertex_stream = static_cast<float*>(value_ptr(obj.front()));

	vector<vec3> colors{
	vec3(1.0f, 0.0f, 0.0f), // Red
	vec3(1.0f, 1.0f, 0.0f), // Yellow
	vec3(0.0f, 1.0f, 0.0f), // Green
	vec3(0.0f, 1.0f, 1.0f), // Cyan
	vec3(0.0f, 0.0f, 1.0f), // Blue
	vec3(1.0f, 0.0f, 1.0f)  // Magenta
	};

	glBegin(GL_QUADS);

	const GLint nPoints = 6 * 4 * 3;
	const GLint nVertexComponents = 2, nColorComponents = 4;

	for (int nv = 0; nv < 6 * 4 * 3; nv += 3) {
		glColor3f(colors[nv / (4 * 3)].r, colors[nv / (4 * 3)].g, colors[nv / (4 * 3)].b);
		vec4 vertex = vec4(vertex_stream[nv], vertex_stream[nv + 1], vertex_stream[nv + 2], 1.0f);
		// Circulo das coordenadas de recorte
		vec4 transformed_vertex = mvp * vertex;
		// Divisao de Perspetiva
		vec4 normalized_vertex = transformed_vertex / transformed_vertex.w;
		// Desenho do vertice
		glVertex3f(normalized_vertex.x, normalized_vertex.y, normalized_vertex.z);
	}

	glEnd();
}

vector<vec3> Load3dModel() {

	vec3 point[6 * 4] = {
		//Frente
		vec3(-1.0f, -1.0f, 1.0f),
		vec3(1.0f, -1.0f, 1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		vec3(-1.0f, 1.0f, 1.0f),
		// Back
		vec3(-1.0f, -1.0f, -1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, 1.0f, -1.0f),
		vec3(-1.0f, 1.0f, -1.0f),
		//direita
		vec3(1.0f, -1.0f, 1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(1.0f, 1.0f, -1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		//esquerda
		vec3(-1.0f, -1.0f, 1.0f),
		vec3(-1.0f, -1.0f, -1.0f),
		vec3(-1.0f, 1.0f, -1.0f),
		vec3(-1.0f, 1.0f, 1.0f),
		//cima
		vec3(-1.0f, 1.0f, 1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		vec3(1.0f, 1.0f, -1.0f),
		vec3(-1.0f, 1.0f, -1.0f),
		//baixo
		vec3(-1.0f, -1.0f, 1.0f),
		vec3(1.0f, -1.0f, 1.0f),
		vec3(1.0f, -1.0f, -1.0f),
		vec3(-1.0f,-1.0f, -1.0f)
	};

	vector<vec3> ret;
	for (auto i : point) ret.push_back(i);  //Put the point inside the list
		
	return ret;
}

//-----------------------------------Inputs bellow -----------------------------------------

void InputManager(GLFWwindow* window, int key, int scancode, int action, int mods) { //Keys Input manager
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			cout << "pressed esc" << endl;
			glfwSetWindowShouldClose(window, 1); //Pede para fechar
			break;
		case GLFW_KEY_1:
			setFullScreen(window);
			break;
		case GLFW_KEY_2:
			setWindowedScreen(window);
			break;
		}
	}
}

void modsInput(GLFWwindow* window, unsigned int codepoint, int mods) { //Mods Inputmanager

	if (codepoint == 'a' && mods == GLFW_MOD_SHIFT) {
		cout << "shift + a" << endl;
	}
}

void setFullScreen(GLFWwindow* window) { //Set Full screen
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

	int fullWidth = videoMode->width;
	int fullHeight = videoMode->height;

	cout << "width: " << fullWidth << " heigth " << fullHeight << endl;

	aspectRatio = float(fullWidth) / float(fullHeight);

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwSetWindowMonitor(window, monitor, 0, 0, fullWidth, fullHeight, videoMode->refreshRate);

}

void setWindowedScreen(GLFWwindow* window) { //Set Windowed
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

	aspectRatio = float(screenWidth) / float(screenHeight);

	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwSetWindowMonitor(window, NULL, videoMode->width / 3, videoMode->height / 3, screenWidth, screenHeight, videoMode->refreshRate);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if (yoffset == 1) ZOOM += fabs(ZOOM) * 0.1f;
	else if (yoffset == -1) ZOOM -= fabs(ZOOM) * 0.1f;
}