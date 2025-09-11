//Práctica 3: Modelado Geométrico y Cámara Sintética.
#include	<stdio.h>
#include	<string.h>
#include	<cmath>
#include	<vector>
#include	<glew.h>
#include	<glfw3.h>
#include	<glm.hpp>
#include	<gtc\matrix_transform.hpp>
#include	<gtc\type_ptr.hpp>
#include	<gtc\random.hpp>
#include	"Mesh.h"
#include	"Shader.h"
#include	"Sphere.h"
#include	"Window.h"
#include	"Camera.h"

//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";

using std::vector;

vector <Mesh*> meshList;
vector <Shader> shaderList;

Camera camera;
Window mainWindow;

Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
static double limitFPS = 1.0 / 60.0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3
	};

	GLfloat vertices_piramide_triangular[] = {
		-0.57f,	-0.5f,	0.0f,	//0
		0.57f,	-0.5f,	0.0f,	//1
		0.0f,	0.45f,	-0.25f,	//2
		0.0f,	-0.5f,	-0.93f,	//3

	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}

// Crear cilindro, cono y esferas con arreglos dinámicos vector
void CrearCilindro(int res, float R) {

	// Constantes utilizadas en los ciclos for
	int n, i;
	// Cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector <GLfloat> vertices;
	vector <unsigned int> indices;

	// Ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		// Caso para terminar el círculo
		else {
			x = R * cos((0)*dt);
			z = R * sin((0)*dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	// Ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	// Ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	// Se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

// Función para crear un cono
void CrearCono(int res,float R) {

	// Constantes utilizadas en los ciclos for
	int n, i;
	// Cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;
	
	vector <GLfloat> vertices;
	vector <unsigned int> indices;

	// Caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);
	
	// Ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);

	for (i = 0; i < res+2; i++) indices.push_back(i);

	// Se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

// Función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector <unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4
	};

	vector <GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

// Función para crear los shaders
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

// Función Principal
int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	// Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();					
	CrearPiramideTriangular();		
	CrearCilindro(5, 1.0f);			
	CrearCono(25, 2.0f);			
	CrearPiramideCuadrangular();	
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.05f, 0.3f);
	/*
	------	INFO	-----
	Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/



	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);
	
	// Loop mientras no se cierra la ventana
	sp.init(); // Inicializar esfera
	sp.load(); // Enviar la esfera al shader

	glm::mat4 model(1.0); // Inicializar matriz de Modelo 4x4

	glm::vec3 color = glm::vec3(0.0f,0.0f,0.0f); // Inicializar Color para enviar a variable Uniform

	float angulo = 0.0f;
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario
		glfwPollEvents();

		// Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad

		
		shaderList[0].useShader();
		angulo += 0.01;
		uniformModel =		shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView =		shaderList[0].getViewLocation();
		uniformColor =		shaderList[0].getColorLocation();
		
		// Indice Meshlist
		// 0 cubo
		// 1 piramide trinagular
		// 2 cilindro
		// 3 cono
		// 4 piramdide cuadrangular
		
		// ---------------------------------------------------------------------------------------------
		model = glm::mat4(1.0);

		// Traslación inicial para posicionar en -Z a los objetos
		
		/*model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));*/

		// Inicializamos el modelo para la PIRAMIDE BASE
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0);
		
		model = glm::translate(model,	glm::vec3(0.0f, 0.1f, -6.025f));
		model = glm::rotate(model,		glm::radians(-1.0f), glm::vec3(1, 0, 0));
		model = glm::scale(model,		glm::vec3(1.09f, 1.17f, 1.0f));
		// La línea de proyección solo se manda una vez a menos que en la ejecución se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformModel,		1, GL_FALSE, glm::value_ptr(model));		
		glUniformMatrix4fv(uniformProjection,	1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView,			1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); // Para cambiar el color del objeto
		meshList[1]->RenderMesh();
		
		// ---------------------------------	CARA ROJA DE LA PIRAMIDE	--------------------------------------------------------
		color = glm::vec3(1.0f, 0.0f, 0.0f);

		// -----------------------	Nivel 1 (una piramide)	-----------------------
		model = glm::mat4(1.0f);

		model = glm::translate(model,	glm::vec3(0.0f, 0.35f, -6.16f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		// -----------------------	Nivel 2 (tres pirámides)  -----------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.0f, 0.04f, -6.08f));
		model = glm::rotate(model,		glm::radians(153.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model,		glm::radians(180.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1,	glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(-0.175f, 0.025f, -6.08f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.175f, 0.025f, -6.08f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		// -----------------------	 Nivel 3 (cinco pirámides)	-----------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(-0.35f, -0.3f, -6.0f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.0f, -0.3f, -6.0f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.35f, -0.3f, -6.0f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(-0.175f, -0.28f, -6.0f));
		model = glm::rotate(model,		glm::radians(152.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model,		glm::radians(180.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.175f, -0.28f, -6.0f));
		model = glm::rotate(model,		glm::radians(152.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model,		glm::radians(180.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();


		// ---------------------------------	CARA VERDE DE LA PIRAMIDE	--------------------------------------------------------
		color = glm::vec3(0.0f, 1.0f, 0.0f);

		// -----------------------	Nivel 1 (una piramide)	-----------------------
		model = glm::mat4(1.0f);

		model = glm::translate(model,	glm::vec3(0.135f, 0.35f, -6.35f));
		model = glm::rotate(model,		glm::radians(122.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		// -----------------------	Nivel 2 (tres pirámides)  -----------------------

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.19f, 0.04f, -6.45f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model,		glm::radians(120.0f), glm::vec3(0, 1, 0));
		

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1,glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.325f, 0.025f, -6.26f));
		model = glm::rotate(model,		glm::radians(120.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.123f, 0.025f, -6.55f));
		model = glm::rotate(model,		glm::radians(122.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		// -----------------------	 Nivel 3 (cinco pirámides)	-----------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.505f, -0.3f, -6.17f));
		model = glm::rotate(model,		glm::radians(125.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.315f, -0.3f, -6.45f));
		model = glm::rotate(model,		glm::radians(124.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.13f, -0.3f, -6.74f));
		model = glm::rotate(model,		glm::radians(125.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.21f, -0.28f, -6.65f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model,		glm::radians(120.0f), glm::vec3(0, 1, 0));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(0.4f, -0.28f, -6.35f));
		model = glm::rotate(model,		glm::radians(120.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		// ---------------------------------	CARA AZUL DE LA PIRAMIDE	--------------------------------------------------------
		color = glm::vec3(0.0f, 0.0f, 1.0f);

		// -----------------------	Nivel 1 (una piramide)	-----------------------
		model = glm::mat4(1.0f);

		model = glm::translate(model,	glm::vec3(-0.135f, 0.35f, -6.35f));
		model = glm::rotate(model,		glm::radians(240.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		// -----------------------	Nivel 2 (tres pirámides)  -----------------------
		model = glm::mat4(1.0f);
		
		model = glm::translate(model, glm::vec3(-0.08f, 0.04f, -6.4f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0, 1, 0));
		

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.325f, 0.025f, -6.26f));
		model = glm::rotate(model, glm::radians(240.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.123f, 0.025f, -6.55f));
		model = glm::rotate(model, glm::radians(238.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		// -----------------------	 Nivel 3 (cinco pirámides)	-----------------------
		model = glm::mat4(1.0f);

		model = glm::translate(model,	glm::vec3(-0.515f, -0.3f, -6.17f));
		model = glm::rotate(model,		glm::radians(240.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(-0.316f, -0.3f, -6.45f));
		model = glm::rotate(model,		glm::radians(237.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(-0.14f, -0.3f, -6.74f));
		model = glm::rotate(model,		glm::radians(237.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1,glm::value_ptr(color));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(-0.16f, -0.28f, -6.5f));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model,		glm::radians(240.0f), glm::vec3(0, 1, 0));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0f);
		model = glm::translate(model,	glm::vec3(-0.4f, -0.28f, -6.35f));
		model = glm::rotate(model,		glm::radians(240.0f), glm::vec3(0, 1, 0));
		model = glm::scale(model,		glm::vec3(0.3f, 0.3f, 0.3f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[1]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}