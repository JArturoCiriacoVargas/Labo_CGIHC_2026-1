// Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector> //para estructuras de datos
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp> //para matrices de transformaciones geometricas
#include <gtc\type_ptr.hpp> // para enviar la uniform de opengl - gsls
#include "Mesh.h" // para dar orden al código
#include "Shader.h"
#include "Window.h"

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes

Window mainWindow;

std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader> shaderList;

//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";

static const char* vShaderR = "shaders/shaderR.vert";
static const char* vShaderG = "shaders/shaderG.vert";
static const char* vShaderB = "shaders/shaderB.vert";
static const char* vShaderC = "shaders/shaderC.vert";
static const char* vShaderG2 = "shaders/shaderG2.vert";

static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";


//shaders nuevos se crearían acá
float angulo = 0.0f;

//Pirámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = 
	{ 
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
	};

	GLfloat vertices[] = 
	{
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3
	};

	Mesh* obj1 = new Mesh();
	obj1 -> CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

//Vértices de un cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = 
	{
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

	GLfloat cubo_vertices[] = 
	{
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

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices,24, 36);
	meshList.push_back(cubo);
}

void CrearLetrasyFiguras()
{
	GLfloat vertices_letras[] = 
	{	
			//X			Y			Z			R		G		B
			-1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
			1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
			1.0f,	1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
			/*1.0f,	1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
			-1.0f,  1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
			-1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,*/
	};
	MeshColor* letras = new MeshColor();
	letras -> CreateMeshColor(vertices_letras,18);
	meshColorList.push_back(letras);
	//-----------------------------------------------------------------

	GLfloat vertices_trianguloazul[] = 
	{
	//    X			Y			Z			R		G		B
		-1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
		1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
		0.0f,	1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
	};
	MeshColor* trianguloazul = new MeshColor();
	trianguloazul -> CreateMeshColor(vertices_trianguloazul, 18);
	meshColorList.push_back(trianguloazul);
	//-----------------------------------------------------------------

	GLfloat vertices_trianguloverde[] =
	{
		//    X			Y			Z			R		G		B
			-1.0f,	-1.0f,		0.5f,			0.0f,	0.5f,	0.0f,
			1.0f,	-1.0f,		0.5f,			0.0f,	0.5f,	0.0f,
			0.0f,	1.0f,		0.5f,			0.0f,	0.5f,	0.0f,
	};
	MeshColor* trianguloverde = new MeshColor();
	trianguloverde -> CreateMeshColor(vertices_trianguloverde, 18);
	meshColorList.push_back(trianguloverde);
	//-----------------------------------------------------------------

	GLfloat vertices_cuadradorojo[] =
	{
		//X			Y			Z			R		G		B
		-0.5f,	-0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.5f,	-0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		-0.5f,	-0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
		-0.5f,	0.5f,		0.5f,			1.0f,	0.0f,	0.0f,
	};
	MeshColor* cuadradorojo = new MeshColor();
	cuadradorojo -> CreateMeshColor(vertices_cuadradorojo, 36);
	meshColorList.push_back(cuadradorojo);
	//-----------------------------------------------------------------

	GLfloat vertices_cuadradoverde[] =
	{
		//X			Y			Z			R		G		B
		-0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		-0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
	};
	MeshColor* cuadradoverde = new MeshColor();
	cuadradoverde -> CreateMeshColor(vertices_cuadradoverde, 36);
	meshColorList.push_back(cuadradoverde);
	//-----------------------------------------------------------------

	GLfloat vertices_cuadradocafe[]
	{
		//X			Y			Z			R		G		B
		-0.5f,	-0.5f,		0.5f,			0.0f,	0.0f,	1.0f,
		0.5f,	-0.5f,		0.5f,			0.0f,	0.0f,	1.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	0.0f,	1.0f,
		-0.5f,	-0.5f,		0.5f,			0.0f,	0.0f,	1.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	0.0f,	1.0f,
		-0.5f,	0.5f,		0.5f,			0.0f,	0.0f,	1.0f,

		/*-0.5f,	-0.5f,		0.5f,			0.478f,	0.255f,	0.067f,
		0.5f,	-0.5f,		0.5f,			0.478f,	0.255f,	0.067f,
		0.5f,	0.5f,		0.5f,			0.478f,	0.255f,	0.067f,
		-0.5f,	-0.5f,		0.5f,			0.478f,	0.255f,	0.067f,
		0.5f,	0.5f,		0.5f,			0.478f,	0.255f,	0.067f,
		-0.5f,	0.5f,		0.5f,			0.478f,	0.255f,	0.067f,*/
	};
	MeshColor* cuadradocafe = new MeshColor();
	cuadradocafe -> CreateMeshColor(vertices_cuadradocafe, 36);
	meshColorList.push_back(cuadradocafe);

	
}

void CreateShaders()
{
	Shader* shader1 = new Shader();	//shader [0] para usar índices: objetos: cubo y  pirámide
	shader1 -> CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();	//shader [1] para usar color como parte del VAO: letras 
	shader2 -> CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);

	Shader* shaderR = new Shader();	//shader [2]
	shaderR->CreateFromFiles(vShaderR, fShaderColor);
	shaderList.push_back(*shaderR);

	Shader* shaderG = new Shader(); //shader [3]
	shaderG->CreateFromFiles(vShaderG, fShaderColor);
	shaderList.push_back(*shaderG);

	Shader* shaderB = new Shader(); //shader [4]
	shaderB->CreateFromFiles(vShaderB, fShaderColor);
	shaderList.push_back(*shaderB);

	Shader* shaderC = new Shader(); //shader [5]
	shaderC->CreateFromFiles(vShaderC, fShaderColor);
	shaderList.push_back(*shaderC);

	Shader* shaderG2 = new Shader(); //shader [6]
	shaderG2->CreateFromFiles(vShaderG2, fShaderColor);
	shaderList.push_back(*shaderG2);

}

int main()
{
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	mainWindow = Window(800, 600); // iniciamos una ventana de 800x600 pixeles
	mainWindow.Initialise();

	CreaPiramide();			//índice 0 en MeshList
	CrearCubo();			//índice 1 en MeshList
	CrearLetrasyFiguras();	//usa MeshColor, índices en MeshColorList
	CreateShaders();

	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// Projection: Matriz de Dimensión 4x4 para indicar si vemos en 2D (ortogonal) o en 3D (perspectiva)
	glm::mat4 projection =	glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	//glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	

	// Fuera del while se usa para inicializar la matriz con una identidad
	// Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0); 
	
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		angulo += 0.001f;
		//Recibir eventos del usuario | Limpiar la ventana | Se agrega limpiar el buffer de profundidad
		glfwPollEvents();
		glClearColor(1.0f,1.0f,1.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		//shaderList[1].useShader(); //para las letras
		//uniformModel = shaderList[1].getModelLocation();
		//uniformProjection = shaderList[1].getProjectLocation();
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		//model = glm::mat4(1.0f);

		////	-----	LETRA A    -----
		//
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-0.85f, 0.0f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.07f, 1.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[3]->RenderMeshColor();

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-0.55f, 0.0f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.07f, 1.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[3]->RenderMeshColor();

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-0.70f, 0.0f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.12f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[3]->RenderMeshColor();

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-0.70f, 0.44f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.12f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[3]->RenderMeshColor();

		////	-----	LETRA C    -----

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-0.15f, 0.0f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.07f, 1.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[4]->RenderMeshColor();

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, -0.44f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.12f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[4]->RenderMeshColor();

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, 0.44f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.12f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[4]->RenderMeshColor();

		////	-----	LETRA J    -----

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.65f, 0.0f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.095f, 1.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[5]->RenderMeshColor();

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.55f, -0.44f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.25f, 0.12f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[5]->RenderMeshColor();

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.65f, 0.44f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.12f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshColorList[5]->RenderMeshColor();


		// -----	PIRÁMIDE AZUL	-----
		shaderList[4].useShader();
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.545f, -2.0f));
		model = glm::scale(model, glm::vec3(1.4f, 0.5f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// -----	PIRÁMIDE VERDE	-----
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

					// DER
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, -0.4f, -2.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.55f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

					// IZQ
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, -0.4f, -2.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.55f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// -----	CUBO ROJO	-----
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.35f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f, -1.3f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// -----	CUBOS VERDES	-----
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

					// CEN
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.777f, -1.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.45f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

					// DER
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.235f, -0.1f, -1.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.45f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

					// IZQ
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.235f, -0.1f, -1.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.45f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		// -----	CUBOS CAFÉS	   -----
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

					// DER
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, -0.85f, -2.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.35f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

					// IZQ
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, -0.85f, -2.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.35f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();

	}
	return 0;
}