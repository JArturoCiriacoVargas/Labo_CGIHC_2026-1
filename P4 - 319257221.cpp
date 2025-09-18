																									/*
PRACTICA 4. MODELADO JERARQUICO
Se implementa el uso de matrices adicionales para almacenar información de transformaciones
geométricas que se quiere heredar entre diversas instancias para que estén unidas

																									*/

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtc\random.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Sphere.h"
#include "Window.h"
#include "Camera.h"

using std::vector;

const float toRadians = 3.14159265f / 180.0;
const float PI = 3.14159265f;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

static double limitFPS = 1.0 / 60.0;

Camera camera;
Window mainWindow;

vector<Mesh*> meshList;
vector<Shader>shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

Sphere sp = Sphere(1.0, 20, 20);


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
		-0.5f, -0.5f,  0.5f,	// 0
		0.5f, -0.5f,  0.5f,		// 1
		0.5f,  0.5f,  0.5f,		// 2
		-0.5f,  0.5f,  0.5f,	// 3
		// back
		-0.5f, -0.5f, -0.5f,	// 4
		0.5f, -0.5f, -0.5f,		// 5
		0.5f,  0.5f, -0.5f,		// 6
		-0.5f,  0.5f, -0.5f		// 7
	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};

	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	// 0
		0.5f,-0.5f,0.0f,	// 1 
		0.0f,0.5f, -0.25f,	// 2
		0.0f,-0.5f,-0.5f,	// 3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);
}

void CrearCilindro(int res, float R) {

	int n, i;

	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
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

	//ciclo for para crear la circunferencia inferior
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

	//ciclo for para crear la circunferencia superior
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

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

void CrearCono(int res, float R) {

	int n, i;

	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
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

	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4
	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,	// 0
		0.5f,-0.5f,-0.5f,	// 1
		-0.5f,-0.5f,-0.5f,	// 2
		-0.5f,-0.5f,0.5f,	// 3
		0.0f,0.5f,0.0f,		// 4
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	//Inicializamos nuestra ventana
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa
	//Llamadas a funciones para crear las geometrias
	CrearCubo();					//índice 0 en MeshList
	CrearPiramideTriangular();		//índice 1 en MeshList
	CrearCilindro(15, 1.0f);		//índice 2 en MeshList
	CrearCono(25, 2.0f);			//índice 3 en MeshList
	CrearPiramideCuadrangular();	//índice 4 en MeshList

	//Llamada a la función para crear el Shader
	CreateShaders();

																											/*
	PARA EL USO DEL COMANDO -> 	glm::lookAt(vector de posición, vector de orientación, vector up));

	La clase <Camera> recibe 6 datos:

		1.	glm::vec3 vector de posición
		2.	glm::vec3 vector up
		3.	GlFloat yaw rotación para girar hacia la derecha e izquierda
		4.	GlFloat pitch rotación para inclinar hacia arriba y abajo
		5.	GlFloat velocidad de desplazamiento
		6.	GlFloat velocidad de vuelta o de giro

	Se usa el Mouse y las teclas W-A-S-D, la posición inicial está en 0,0,1 y ve hacia 0,0,-1.
																											*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	//Inicializamos las variables uniform para nuestro modelo
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;

	//Para cambiar entre el tipo de proyección
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	sp.init(); //Inicializamos la esfera
	sp.load(); //Enviamos la esfera al shader

	//Inicializamos una matriz para nuestro modelo y otra auxiliar
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);

	//Inicializamos nuestra variable color para enviarla a la variable uniform
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibimos los eventos del usuario
		glfwPollEvents();

		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiamos la ventana
		glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

																						/*
		Información: Teclas de la F a la K para rotaciones de articulaciones

		  Tecla:	|		Rotación de:
			F			   Articulacion 1
			G			   Articulacion 2
			H              Articulacion 3
			J              Articulacion 4
																						*/


		/*--------------------------------------------------	CABINA DE LA GRÚA	--------------------------------------------------*/
		//Reiniciamos la matriz modelo con la matriz identidad
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -4.0f));

		modelaux = model;
		model = glm::scale(model, glm::vec3(4.5f, 2.5f, 2.5f));

		color = glm::vec3(0.0f, 0.5f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cubo
		meshList[0]->RenderMesh();


		/*--------------------------------------------------	BASE DE LA CABINA	--------------------------------------------------*/
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 3.0f, 3.0f));

		//Lo coloreamos de color morado
		color = glm::vec3(0.5f, 0.1f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos una piramide cuadrangular
		meshList[4]->RenderMesh();


		/*--------------------------------------------------	LLANTAS DE LA BASE DE LA CABINA	--------------------------------------------------*/
		//--- Llanta derecha del frente
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -3.5f, 0.0f));
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -1.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Para que queden mirando hacia nosotros
		model = glm::rotate(model, glm::radians(mainWindow.getrueda1()), glm::vec3(0.0f, 1.0f, 0.0f)); //tecla I para rotar la llanta
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.0f));

		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cilindro con 15 vértices
		meshList[2]->RenderMeshGeometry();

		//--- Llanta derecha de atrás
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -3.5f, 0.0f));
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, -1.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrueda2()), glm::vec3(0.0f, 1.0f, 0.0f));//tecla O para rotar la llanta
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.0f));

		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cilindro con 15 vértices
		meshList[2]->RenderMeshGeometry();

		//--- Llanta izquierda del frente
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -3.5f, 0.0f));
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 1.4f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrueda3()), glm::vec3(0.0f, 1.0f, 0.0f));//tecla K para rotar la llanta
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.0f));

		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cilindro con 15 vértices
		meshList[2]->RenderMeshGeometry();

		//--- Llanta izquierda de atrás
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -3.5f, 0.0f));
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.4f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrueda4()), glm::vec3(0.0f, 1.0f, 0.0f));//tecla L para rotar la llanta
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.0f));

		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cilindro con 15 vértices
		meshList[2]->RenderMeshGeometry();

		/*--------------------------------------------------	ARTICULACIÓN 1	--------------------------------------------------*/
		model = modelaux;

		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));

		//Lo coloreamos de color rojo
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos una esfera
		sp.render();


		/*--------------------------------------------------	BRAZO 1	--------------------------------------------------*/
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));


		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Lo coloreamos de color azul
		color = glm::vec3(0.0f, 0.1f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Dibujamos un cubo
		meshList[0]->RenderMesh();


		/*--------------------------------------------------	ARTICULACIÓN 2	--------------------------------------------------*/
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));

		//Lo coloreamos de color rojo
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos una esfera
		sp.render();


		/*--------------------------------------------------	BRAZO 2	--------------------------------------------------*/
		//model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));

		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));

		//Lo coloreamos de color azul
		color = glm::vec3(0.0f, 0.1f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cubo
		meshList[0]->RenderMesh();


		/*--------------------------------------------------	ARTICULACIÓN 3	--------------------------------------------------*/
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));

		//Lo coloreamos de color naranja
		color = glm::vec3(1.0f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos una esfera
		sp.render();


		/*--------------------------------------------------	BRAZO 3	--------------------------------------------------*/
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));

		//Lo coloreamos de color azul
		color = glm::vec3(0.0f, 0.1f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cubo
		meshList[0]->RenderMesh();


		/*--------------------------------------------------	ARTICULACIÓN 4	--------------------------------------------------*/

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 1.0f, 0.0f));

		//Lo coloreamos de color verde
		color = glm::vec3(0.3f, 1.0f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos una esfera
		sp.render();


		/*--------------------------------------------------	CANASTA DE LA GRUA	--------------------------------------------------*/

		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.85f, 2.15f, 3.0f));

		//Lo coloreamos de color amarillo
		color = glm::vec3(1.0f, 1.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//Dibujamos un cubo
		meshList[0]->RenderMesh();
		






		///*--------------------------------------------------	ANIMAL ROBOT	--------------------------------------------------*/
		////Reiniciamos la matriz modelo con la matriz identidad
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 4.5f, -4.0f));

		//glm::mat4 modelPatas = model;

		//// Dibujar cuerpo
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(7.5f, 5.5f, 10.5f));

		//color = glm::vec3(0.80f, 0.50f, 0.20f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		////Dibujamos un cubo
		//meshList[0]->RenderMesh();



		///*-------------------------------------------------- CABEZA --------------------------------------------------*/
		//model = modelPatas;
		//model = glm::translate(model, glm::vec3(0.0f, 2.0f, 6.5f));
		//model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();


		///*-------------------------------------------------- OREJA DERECHA --------------------------------------------------*/
		//model = modelPatas;
		//model = glm::translate(model, glm::vec3(-1.5f, 4.5f, 6.0f));
		//model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.6f, 2.0f, 0.5f));

		//color = glm::vec3(0.90f, 0.91f, 0.98f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cono
		//meshList[3]->RenderMeshGeometry();


		///*-------------------------------------------------- OREJA IZQUIERDA --------------------------------------------------*/
		//model = modelPatas;
		//model = glm::translate(model, glm::vec3(1.5f, 4.5f, 6.0f)); 
		//model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.6f, 2.0f, 0.5f));

		//color = glm::vec3(0.90f, 0.91f, 0.98f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cono
		//meshList[3]->RenderMeshGeometry();


		///*-------------------------------------------------- PATA DELANTERA DERECHA --------------------------------------------------*/
		//model = modelPatas;
		//model = glm::translate(model, glm::vec3(-2.5f, -2.0f, 3.5f));
		//// Articulacion 1 pegada al cuerpo
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla F para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Pata parte superior
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshList[0]->RenderMesh();

		//// Articulacion 2 rodilla
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla G para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Pata parte inferiorss
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cubo
		//meshList[0]->RenderMesh();

		//// Patita final
		//model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));

		//color = glm::vec3(0.90f, 0.91f, 0.98f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una piramide cuadrangular
		//meshList[4]->RenderMesh();


		///*-------------------------------------------------- PATA DELANTERA IZQUIERDA --------------------------------------------------*/
		//model = modelPatas;
	
		//model = glm::translate(model, glm::vec3(2.5f, -2.0f, 3.5f));
		//// Articulacion 1 pegada al cuerpo
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla H para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//sp.render();

		//// Pata parte superior
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));
		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshList[0]->RenderMesh();

		//// Articulacion 2 rodilla
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla J para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Pata parte inferiorss
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cubo
		//meshList[0]->RenderMesh();

		//// Patita final
		//model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));

		//color = glm::vec3(0.90f, 0.91f, 0.98f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una piramide cuadrangular
		//meshList[4]->RenderMesh();


		///*-------------------------------------------------- PATA TRASERA IZQUIERDA --------------------------------------------------*/
		//model = modelPatas;
		//model = glm::translate(model, glm::vec3(2.5f, -2.0f, -3.5f));
		//// Articulacion 1 pegada al cuerpo
		//model = glm::rotate(model, glm::radians(mainWindow.getrueda1()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla I para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Pata parte superior
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cubo
		//meshList[0]->RenderMesh();

		//// Articulacion 2 rodilla
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getrueda2()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla O para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Pata parte inferiorss
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cubo
		//meshList[0]->RenderMesh();

		//// Patita final
		//model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));

		//color = glm::vec3(0.90f, 0.91f, 0.98f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una piramide cuadrangular
		//meshList[4]->RenderMesh();


		///*-------------------------------------------------- PATA TRASERA DERECHA --------------------------------------------------*/
		//model = modelPatas;
		//model = glm::translate(model, glm::vec3(-2.5f, -2.0f, -3.5f));
		//// Articulacion 1 pegada al cuerpo
		//model = glm::rotate(model, glm::radians(mainWindow.getrueda3()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla K para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Pata parte superior
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cubo
		//meshList[0]->RenderMesh();

		//// Articulacion 2 rodilla
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getrueda4()), glm::vec3(1.0f, 0.0f, 0.0f)); //tecla L para rotar

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Pata parte inferiorss
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.0f, 4.0f, 1.0f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cubo
		//meshList[0]->RenderMesh();

		//// Patita final
		//model = glm::translate(model, glm::vec3(0.0f, -0.3f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));

		//color = glm::vec3(0.90f, 0.91f, 0.98f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una piramide cuadrangular
		//meshList[4]->RenderMesh();


		///*-------------------------------------------------- COLA --------------------------------------------------*/
		//model = modelPatas;
		//model = glm::translate(model, glm::vec3(0.0f, 0.5f, -5.5f));

		//modelaux = model;
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 3.0f, 0.5f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cilindro
		//meshList[2]->RenderMeshGeometry();

		//// Articulacion cola
		//model = modelaux;
		//model = glm::translate(model, glm::vec3(0.0f, -1.0f, -1.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getcola()), glm::vec3(0.0f, 0.0f, 1.0f)); // Tecla Y | Tecla U
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

		//color = glm::vec3(0.60f, 0.62f, 0.64f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos una esfera
		//sp.render();

		//// Segundo segmento de la cola
		//model = glm::translate(model, glm::vec3(0.0f, -2.0f, -1.9f)); 
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f)); 

		//color = glm::vec3(0.90f, 0.91f, 0.98f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//// Dibujamos un cilindro
		//meshList[2]->RenderMeshGeometry();


		//Terminamos el programa
		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}