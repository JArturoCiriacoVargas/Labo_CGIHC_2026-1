													/*
Práctica 5: Optimización y Carga de Modelos
													*/

#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"
//#include<assimp/Importer.hpp>


const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Model PorscheGT2_ChasisSinLlantasCofre, PorscheGT2_Llanta, PorscheGT2_Cofre, PorscheGT2_Exterior;

// Variables para la traslacion del auto, necesitamos saber la posicion inicial del auto, a que velocidad se va a mover, en que sentido va ir si gira y que velocidad de giro 
glm::vec3 posicionAuto = glm::vec3(0.0f, -0.7f, 0.0f);
float velocidadAuto = 0.1f;
float giroAuto = 0.0f;
float velocidadGiro = 0.2f;

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
static const char* vShader = "shaders/shader_m.vert";
static const char* fShader = "shaders/shader_m.frag";

Sphere sp = Sphere(0.1, 20, 20);

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	sp.init();
	sp.load();

	camera = Camera(glm::vec3(0.0f, 2.5f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.6f, 1.0f);

	PorscheGT2_ChasisSinLlantasCofre = Model();
	PorscheGT2_ChasisSinLlantasCofre.LoadModel("Models/Porsche GT2 RS_ChasisSinLlantasCofre.obj");

	PorscheGT2_Exterior = Model();
	PorscheGT2_Exterior.LoadModel("Models/Porsche GT2 RS_Exterior.obj");

	PorscheGT2_Cofre = Model();
	PorscheGT2_Cofre.LoadModel("Models/Porsche GT2 RS_Cofre.obj");

	PorscheGT2_Llanta = Model();
	PorscheGT2_Llanta.LoadModel("Models/Porsche GT2 RS_Llanta.obj");

	std::vector<std::string> skyboxFaces;

	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint
		uniformColor = 0, uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0), modelaux(1.0);

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();

		glm::vec3 direccion = glm::vec3(sin(glm::radians(giroAuto)), 0.0f, cos(glm::radians(giroAuto)));

		//Condiciones para movernos en la direccion con la velocidad del auto
		if (mainWindow.getsKeys()[GLFW_KEY_UP])
		{
			posicionAuto += direccion * velocidadAuto;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_UP] && mainWindow.getsKeys()[GLFW_KEY_X]) { // avanza más rapido
			posicionAuto += direccion * velocidadAuto * 2.0f;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_DOWN])
		{
			posicionAuto -= direccion * velocidadAuto;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_LEFT]) 
		{
			giroAuto += velocidadGiro;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_RIGHT]) 
		{
			giroAuto -= velocidadGiro;
		}

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		model = glm::mat4(1.0);			// Iniciamos el modelo
		glm::mat4 modelBase = model;	// Guardamos la base del modelo donde empezamos a dibujar los modelos
		modelaux = model;				// Nos ayuda para dibujar a partir de la base del modelo para no cargar todo lo anterior

		//																	SUELO
		color = glm::vec3(0.725f, 0.361f, 0.220f);

		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		meshList[2]->RenderMesh();
																																						/*
		PRÁCTICA:
		1. Importar su modelo de coche propio dentro del escenario a una escala adecuada.

		2. Importar sus 4 llantas y acomodarlas jerárquicamente, agregar el mismo valor de rotación a las llantas para que al presionar puedan 
		   rotar hacia adelante y hacia atrás.

		3. Importar el cofre del coche, acomodarlo jerárquicamente y agregar la rotación para poder abrir y cerrar. 

		4. Agregar traslación con teclado para que pueda avanzar y retroceder de forma independiente.
																																						*/
		
		//																	- PORSCHE 911 GT2 EVO -
		

		//																	< Modelo del auto sin llantas ni cofre >
		color = glm::vec3(1.0f, 1.0f, 0.0f);

		model = modelBase;
		model = glm::translate(model, posicionAuto);
		model = glm::rotate(model, glm::radians(giroAuto), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 modelAuto(1.0), modelaux(1.0);
		modelAuto = model;

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PorscheGT2_ChasisSinLlantasCofre.RenderModel();

		//																	< Modelo del exterior del auto >
		color = glm::vec3(0.85f, 0.85f, 0.85f);

		model = modelAuto;
		model = glm::translate(model, glm::vec3(0.0f, -84.6f, 0.0f));

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PorscheGT2_Exterior.RenderModel();

		//																	< Modelo del cofre del auto >
		
		//																	Articulación Cofre
		model = modelAuto;

		color = glm::vec3(0.0f, 0.0f, 1.0f);

		model = glm::translate(model, glm::vec3(0.0f, 6.5f, 6.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(-1.0f, 0.0f, 0.0f));


		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render();

		//																	Cofre

		color = glm::vec3(0.75f, 0.75f, 0.75f);

		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -31.5f, -6.0f));

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PorscheGT2_Cofre.RenderModel();

		//																	< Modelo de la llantas del auto >
		
		//																	Articulación llanta delantera izquierda
		model = modelAuto;

		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::translate(model, glm::vec3(6.5f, 1.5f, 10.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));


		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render();

		//																	Llanta delantera izquierda
		color = glm::vec3(0.0f, 0.0f, 0.0f);

		modelaux = model;
		model = glm::translate(model, glm::vec3(-13.0f, -3.0f, -10.55f));

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PorscheGT2_Llanta.RenderModel();

		//																	Articulación llanta delantera derecha
		model = modelAuto;

		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::translate(model, glm::vec3(-7.2f, 1.5f, 10.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));


		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render();

		//																	Llanta delantera derecha
		color = glm::vec3(0.0f, 0.0f, 0.0f);

		modelaux = model;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-14.0f, -3.0f, -10.55f));

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PorscheGT2_Llanta.RenderModel();

		//																	Articulación llanta trasera izquierda
		model = modelAuto;

		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::translate(model, glm::vec3(6.5f, 1.5f, -9.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));


		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render();

		//																	Llanta trasera izquierda
		color = glm::vec3(0.0f, 0.0f, 0.0f);

		modelaux = model;
		model = glm::translate(model, glm::vec3(-13.0f, -3.0f, -10.55f));

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PorscheGT2_Llanta.RenderModel();

		//																	Articulación llanta trasera derecha
		model = modelAuto;

		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::translate(model, glm::vec3(-7.2f, 1.5f, -9.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));


		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render();

		//																	Llanta trasera derecha
		color = glm::vec3(0.0f, 0.0f, 0.0f);

		modelaux = model;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-14.0f, -3.0f, -10.55f));

		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		PorscheGT2_Llanta.RenderModel();






		//Terminamos el programa
		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}