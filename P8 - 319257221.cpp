/*
Práctica 7: Iluminación 1
*/
//para cargar imagen
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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//helicoptero
GLfloat giroHelices = 0.0f;

//texturas
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dado8Texture;

//modelos
Model Blackhawk_M;
Model BlackhawkHelices_M;
Model Farola_M;
Model PorscheGT_Parabrisas_M;
Model PorscheGT_Llanta_M;
Model PorscheGT_Chasis_M;
Model PorscheGT_Cofre_M;
Model Hexagono_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Iluminacion
unsigned int pointLightCount = 0;			//contador de luces puntuales
unsigned int spotLightCount = 0;			//contador de luces spotlight
DirectionalLight mainLight;					//luz directional
PointLight pointLights[MAX_POINT_LIGHTS];	//arreglo de luces tipo pointlight
SpotLight spotLights[MAX_SPOT_LIGHTS];		//arreglo de luces tipo spotlight


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

//funcion para crear las primitivas
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

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
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

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

//funcion para crear los shaders
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// Crear dado 8 caras
void CreateDado8()
{
	std::vector<unsigned int> dado8_indices =
	{ 0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23 };

	std::vector<GLfloat> dado8_vertices = {
		// parte superior
		// x,    y,    z,      u,    v,			nx, ny, nz
		// cara 4 (parte superior...)
		0.0f,  0.5f,  0.0f,		0.50f,0.75f,	-0.5f,  -0.5f,  -0.5f, // 0
		0.0f,  0.0f,  0.5f,		0.25f,0.50f,	-0.5f,  -0.5f,  -0.5f, // 1
		0.5f,  0.0f,  0.0f,		0.75f,0.50f,	-0.5f,  -0.5f,  -0.5f, // 2

		// cara 1
		0.0f,  0.5f,  0.0f,		0.50f,0.25f,	0.5f,  -0.5f,  -0.5f, // 3
		0.0f,  0.0f,  0.5f,		0.25f,0.50f,	0.5f,  -0.5f,  -0.5f, // 4
		-0.5f, 0.0f,  0.0f,		0.75f,0.50f,	0.5f,  -0.5f,  -0.5f, // 5

		// cara 3
		0.0f,  0.5f,  0.0f,		1.00f,0.25f,	-0.5f,  -0.5f,  0.5f, // 6
		0.5f,  0.0f,  0.0f,		0.75f,0.50f,	-0.5f,  -0.5f,  0.5f, // 7
		0.0f,  0.0f, -0.5f,		0.50f,0.25f,	-0.5f,  -0.5f,  0.5f, // 8

		// cara 2
		0.0f,  0.5f,  0.0f,		1.00f,0.75f,	0.5f,  -0.5f,  0.5f, // 9
		-0.5f, 0.0f,  0.0f,		0.75f,0.50f,	0.5f,  -0.5f,  0.5f, //10
		0.0f,  0.0f, -0.5f,		0.50f,0.75f,	0.5f,  -0.5f,  0.5f, //11

		// cara 8 (parte inferior...)
		0.0f, -0.5f,  0.0f,		0.25f,1.00f,	-0.5f,  0.5f,  -0.5f, //12
		0.0f,  0.0f,  0.5f,		0.50f,0.75f,	-0.5f,  0.5f,  -0.5f, //13
		0.5f,  0.0f,  0.0f,		0.00f,0.75f,	-0.5f,  0.5f,  -0.5f, //14

		// cara 5
		0.0f, -0.5f,  0.0f,		0.25f,0.00f,	0.5f,  0.5f,  -0.5f, //15
		0.0f,  0.0f,  0.5f,		0.50f,0.25f,	0.5f,  0.5f,  -0.5f, //16
		-0.5f, 0.0f,  0.0f,		0.00f,0.25f,	0.5f,  0.5f,  -0.5f, //17

		// cara 7
		0.0f, -0.5f,  0.0f,		0.00f,0.25f,	-0.5f,  0.5f,  0.5f, //18
		0.5f,  0.0f,  0.0f,		0.25f,0.50f,	-0.5f,  0.5f,  0.5f, //19
		0.0f,  0.0f, -0.5f,		0.50f,0.25f,	-0.5f,  0.5f,  0.5f, //20

		// cara 6
		0.0f, -0.5f,  0.0f,		0.00f,0.75f,	0.5f,  0.5f,  0.5f, //21
		-0.5f, 0.0f,  0.0f,		0.25f,0.50f,	0.5f,  0.5f,  0.5f, //22
		0.0f,  0.0f, -0.5f,		0.50f,0.75f,	0.5f,  0.5f,  0.5f,  //23
	};
	Mesh* dado8 = new Mesh();
	dado8->CreateMesh(&dado8_vertices[0], &dado8_indices[0], (float)dado8_vertices.size(), (unsigned int)dado8_indices.size());
	meshList.push_back(dado8);
}

//main
int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateDado8();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 30.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/pasto.png");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	dado8Texture = Texture("Textures/dado8.png");
	dado8Texture.LoadTextureA();

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/Black Hawk uh-60_SinHelices.obj");
	BlackhawkHelices_M = Model();
	BlackhawkHelices_M.LoadModel("Models/Black Hawk uh-60_Helices.obj");

	Farola_M = Model();
	Farola_M.LoadModel("Models/Farola.obj");

	// Cargamos los modelos del Porsche GT
	PorscheGT_Llanta_M.LoadModel("Models/Porsche GT RS/Porsche GT RS_llanta.obj");
	PorscheGT_Chasis_M.LoadModel("Models/Porsche GT RS/Porsche GT RS_chasis.obj");
	PorscheGT_Parabrisas_M.LoadModel("Models/Porsche GT RS/Porsche GT RS_parabrisas.obj");
	PorscheGT_Cofre_M.LoadModel("Models/Porsche GT RS/Porsche GT RS_cofrenuevo2.obj");

	Hexagono_M.LoadModel("Models/Hexagono.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	mainLight = DirectionalLight(	// SOL
		0.73f, 0.73f, 0.73f,	// Color
		0.05f, 0.15f,			// Intensidades ambiente y difusa más bajas
		-0.2f, -0.8f, -0.3f		// Dirección
	);
	//
	spotLights[0] = SpotLight(	// LUZ TRASERA DEL AUTO
		1.0f, 0.0f, 0.0f,		// Color
		0.55f, 4.65f,			// Intensidad ambiental | Intensidad Difusa
		0.0f, 0.0f, 0.0f,		// Posicion
		0.0f, 0.0f, 0.0f,		// Direccion de la luz
		1.0f, 0.014f, 0.00007f,	// Atenuación (Constante | Lineal | Exponencial)
		35.0f					// Angulo edge
	); spotLightCount++;
	spotLights[1] = SpotLight(	// LUZ DELANTERA DEL AUTO
		0.0f, 1.0f, 0.0f,		// Color
		0.55f, 4.65f,			// Intensidad ambiental | Intensidad Difusa
		0.0f, 0.0f, 0.0f,		// Posicion
		0.0f, 0.0f, 0.0f,		// Direccion de la luz
		1.0f, 0.014f, 0.00007f,	// Atenuación (Constante | Lineal | Exponencial)
		35.0f					// Angulo edge
	); spotLightCount++;
	spotLights[2] = SpotLight(	// LUZ DEL HELICOPTERO
		1.0f, 1.0f, 0.0f,		// Color
		0.15f, 1.65f,			// Intensidad ambiental | Intensidad Difusa
		0.0f, 0.0f, 0.0f,		// Posicion
		0.0f, -1.0f, 0.0f,		// Direccion de la luz
		1.0f, 0.014f, 0.00007f,	// Atenuación (Constante | Lineal | Exponencial)
		35.0f					// Angulo edge
	); spotLightCount++;
	spotLights[3] = SpotLight(	// LUZ DEL COFRE
		0.6f, 0.0f, 0.8f,		// Color
		0.55f, 4.65f,			// Intensidad ambiental | Intensidad Difusa
		0.0f, 0.0f, 0.0f,		// Posicion
		0.0f, 0.0f, 0.0f,		// Direccion de la luz
		1.0f, 0.014f, 0.00007f,	// Atenuación (Constante | Lineal | Exponencial)
		35.0f					// Angulo edge
	); spotLightCount++;

	pointLights[0] = PointLight(// LUZ DE LA LÁMPARA
		1.0f, 1.0f, 1.0f,		// Color
		0.0f, 0.0f,				// Intensidad ambiental | Intensidad Difusa
		0.0f, 0.0f, 0.0f,		// Posicion
		0.3f, 0.2f, 0.001f);	// Atenuación (Constante | Lineal | Exponencial)
	pointLightCount++;
	pointLights[1] = PointLight(// LUZ DEL HEXAGONO
		0.2f, 0.4f, 1.0f,		// Color
		0.0f, 0.0f,				// Intensidad ambiental | Intensidad Difusa
		0.0f, 0.0f, 0.0f,		// Posicion
		0.5f, 0.1f, 0.01f		// Atenuación (constante | lineal | Exponencial)
		);
	pointLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelAuto(1.0);
	glm::mat4 modelBase(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	
	float angulo = 0.0f;
	glm::vec3 posLamp = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posLightLamp = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posHelicop = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posLightHelicop = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posAuto = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posLightAuto = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posCofre = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posLightCofre = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posLightFront = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posLightBack = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 posHexagono = glm::vec3(0.0f, 0.0f, 0.0f);
	float angCofre = 0.0f;
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		//shader
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		
		// Suelo -----------------------------------------------------------------------------
		model = glm::mat4(1.0);
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		// Porsche 911 GT --------------------------------------------------------------------
		
		// 1. Porsche
		model = modelBase;
		posAuto = glm::vec3(0.0f, 0.0f, -50.0f + mainWindow.getPozzAuto());
		model = glm::translate(model, posAuto);

		posLightFront = glm::vec3(0.0f, 4.0f, 19.0f);
		posLightBack = glm::vec3(0.0f, 5.0f, -18.0f);

		if (mainWindow.getLightFront() > 0.0f)
		{
			spotLights[0].SetFlash(glm::vec3(0.0f), glm::vec3(0.0f));
			spotLights[1].SetFlash(posAuto + posLightFront, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (mainWindow.getLightFront() < 1.0f)
		{
			spotLights[0].SetFlash(posAuto + posLightBack, glm::vec3(0.0f, 0.0f, -1.0f));
			spotLights[1].SetFlash(glm::vec3(0.0f), glm::vec3(0.0f));
		}

		modelAuto = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PorscheGT_Chasis_M.RenderModel();

		// 2. Parabrisas del auto
		model = modelAuto;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PorscheGT_Parabrisas_M.RenderModel();

		// 4️. Cofre del auto
		model = modelAuto;
		angCofre = mainWindow.getRotCofre();
		posCofre = glm::vec3(0.0f, 6.60f, 6.75f);
		posLightCofre = glm::vec3(0.0f, sin(glm::radians(angCofre)) * 7.0f, cos(glm::radians(angCofre)) * 7.0f);
		spotLights[3].SetFlash(posAuto + posCofre + posLightCofre, glm::vec3(0.0f, sin(glm::radians(angCofre)), cos(glm::radians(angCofre))));

		model = glm::translate(model, posCofre);
		model = glm::rotate(model, glm::radians(angCofre), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PorscheGT_Cofre_M.RenderModel();

		// 5. Llantas del auto

		// Lado del piloto
		model = modelAuto;
		model = glm::translate(model, glm::vec3(6.65f, 2.5f, 10.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getRotLlanta()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PorscheGT_Llanta_M.RenderModel();

		// Lado del copiloto
		model = modelAuto;
		model = glm::translate(model, glm::vec3(-6.65f, 2.5f, 10.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getRotLlanta()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PorscheGT_Llanta_M.RenderModel();

		// Lado del piloto trasera
		model = modelAuto;
		model = glm::translate(model, glm::vec3(6.65f, 2.5f, -10.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getRotLlanta()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PorscheGT_Llanta_M.RenderModel();

		// Lado del copiloto trasera
		model = modelAuto;
		model = glm::translate(model, glm::vec3(-6.65f, 2.5f, -10.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getRotLlanta()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PorscheGT_Llanta_M.RenderModel();

		// Modelo de Hexagono con Ilumación --------------------------------------------------
		model = glm::mat4(1.0);
		posHexagono = glm::vec3(9.0f, 10.0f, 0.0f);
		model = glm::translate(model, posHexagono);
		pointLights[1].SetPos(posHexagono + glm::vec3(0.0f, 0.0f, 0.0f));
		if (mainWindow.getlightHexagon() < 1.0f)
		{
			pointLights[1].SetIntensity(0.0f, 0.0f);
		}
		else
		{
			pointLights[1].SetIntensity(0.7f, 4.0f);
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Hexagono_M.RenderModel();

		// Farola ----------------------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, posLamp);
		posLightLamp = glm::vec3(9.0f, 39.8f, 0.0f);
		pointLights[0].SetPos(glm::vec3(posLamp + posLightLamp));

		if (mainWindow.getlightOff() < 1.0f) 
		{
			pointLights[0].SetIntensity(0.0f, 0.0f);
		}
		else 
		{
			pointLights[0].SetIntensity(0.55f, 5.0f);
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Farola_M.RenderModel();

		// Dado de 8 caras -------------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, 20.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.5f, 4.5f, 4.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dado8Texture.UseTexture();
		meshList[4]->RenderMesh();

		// ------------------------------------------------------------------------------------
		//Instancia del helicóptero
		model = glm::mat4(1.0);
		posHelicop = glm::vec3(0.0f, 65.0f, -80.0f + mainWindow.getPoszHelicoptero());
		model = glm::translate(model, posHelicop);
		posLightHelicop = glm::vec3(0.0f, -5.0f, 0.0f);
		spotLights[2].SetPos(posHelicop + posLightHelicop);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Helices
		modelaux = model;
		giroHelices += 0.9f;
		modelaux = glm::rotate(modelaux, glm::radians(giroHelices * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
		BlackhawkHelices_M.RenderModel();

		//blending: transparencia o traslucidez ----------------------------------------------
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		////Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//AgaveTexture.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		//glDisable(GL_BLEND);
		// -----------------------------------------------------------------------------------

		glUseProgram(0);
		mainWindow.swapBuffers();
	}	return 0;
}