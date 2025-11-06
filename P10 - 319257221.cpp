
#define STB_IMAGE_IMPLEMENTATION	// Para cargar imágenes
#include <stdio.h>					// .
#include <string.h>					// .
#include <cmath>					// .
#include <vector>					// .
#include <math.h>					// .
#include <glew.h>					// .
#include <glfw3.h>					// .
#include <glm.hpp>					// .
#include <gtc/matrix_transform.hpp>	// .
#include <gtc/type_ptr.hpp>			// .
#include "Window.h"					// Limpieza en el código
#include "Mesh.h"					// .
#include "Shader_light.h"			// .
#include "Camera.h"					// . 
#include "Texture.h"				// .
#include "Sphere.h"					// .
#include "Model.h"					// .
#include "Skybox.h"					// .
#include "CommonValues.h"			// .
#include "DirectionalLight.h"		// Iluminación
#include "PointLight.h"				// .
#include "SpotLight.h"				// .
#include "Material.h"				// .
#include "ResourceManager.h"		// Recursos
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
//#include<assimp/Importer.hpp>		// Para probar el importer

// ------------------------------------------------------------------ Ventana
Window mainWindow;

std::vector<Mesh*> meshList;		// Vector de Meshs
std::vector<Shader> shaderList;		// Vector de Shaders

// ------------------------------------------------------------------ Camara
Camera camera;

static double limitFPS = 1.0 / 60.0;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// ------------------------------------------------------------------ Shader
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// ------------------------------------------------------------------ Material
Material Material_brillante;
Material Material_opaco;

// ------------------------------------------------------------------ Iluminacion
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

DirectionalLight mainLight;					// Luz Direccional (Sol)
PointLight PL_0[MAX_POINT_LIGHTS];			// Arreglos de luces PointLight
SpotLight PL_1[MAX_SPOT_LIGHTS];			//.
SpotLight SP_RGB[MAX_SPOT_LIGHTS];			// Arreglos de luces SpotLight
SpotLight SP_lamparas[MAX_SPOT_LIGHTS];		//.

// ------------------------------------------------------------------ Funciones

// Función - Cálculo de Normales por promedio de vértices 
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

// Función - Creamos las primitivas
void CreatePrimitives()
{
	// --------------------------------------------------------- Suelo
	GLfloat vertices_Suelo[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-10.0f, 0.0f, -10.0f,	0.0f, 1.0f,		0.0f, -1.0f, 0.0f, // indice 0 - arriba a la izquierda
			10.0f, 0.0f, -10.0f,	1.0f, 1.0f,		0.0f, -1.0f, 0.0f, // indice 1 - arriba a la derecha
			-10.0f, 0.0f, 10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f, // indice 2 - abajo a la izquierda
			10.0f, 0.0f, 10.0f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f  // indice 3 - abajo a la derecha
	};
	unsigned int indices_Suelo[] = {
		0, 2, 1,
		1, 2, 3
	};

	Mesh* Suelo = new Mesh();
	Suelo->CreateMesh(vertices_Suelo, indices_Suelo, 32, 6);
	meshList.push_back(Suelo);

	// --------------------------------------------------------- Vegetación
	GLfloat vertices_Vegetacion[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

			0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	unsigned int indices_Vegetacion[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	Mesh* Vegetacion = new Mesh();
	Vegetacion->CreateMesh(vertices_Vegetacion, indices_Vegetacion, 64, 12);
	meshList.push_back(Vegetacion);

	calcAverageNormals(indices_Vegetacion, 12, vertices_Vegetacion, 64, 8, 5);
}

// Función - Creamos los shaders
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// Función - Creamos la iluminación
void SetLights()
{
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(
		0.83f, 0.78f, 0.92f,	// Color
		1.0f, 0.3f,				// Intensidades ambiente y difusa más bajas
		-1.0f, 0.0f, 0.0f		// Dirección
	);

	// Arreglo 1 para cambio de luces
	// Verde
	SP_lamparas[0] = SpotLight(0.0f, 1.0f, 0.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		20.0f, 15.0f, 0.0f,                     // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.2f, 0.0f, 0.0f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Azul
	SP_lamparas[1] = SpotLight(0.0f, 0.0f, 1.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		10.0f, 15.0f, 0.0f,                     // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.0f, 0.01f, 0.0f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Roja
	SP_lamparas[2] = SpotLight(1.0f, 0.0f, 0.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		0.0f, 15.0f, 0.0f,                      // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.0f, 0.0f, 0.01f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;
	// Blanca
	SP_lamparas[3] = SpotLight(1.0f, 1.0f, 1.0f,// Color
		1.0f, 5.0f,                             // Intensidad ambiental | Intensidad Difusa
		-10.0f, 15.0f, 0.0f,                    // Posición
		0.0f, -1.0f, 0.0f,                      // Dirección de la luz
		0.2f, 0.01f, 0.01f,						// Atenuación (Constante | Lineal | Exponencial)
		20.0f);                                 // Ángulo edge
	spotLightCount++;

	// Arreglo 2 para cambio de luces
	// Blanca
	SP_RGB[0] = SP_lamparas[3];
	// Verde
	SP_RGB[1] = SP_lamparas[0];
	// Azul
	SP_RGB[2] = SP_lamparas[1];
	// Roja
	SP_RGB[3] = SP_lamparas[2];
}

// Función - Renderizar modelo
void RenderModel(glm::mat4 model, GLuint uniformModel, glm::vec3 origen, glm::vec3 pos, Model& modelRender)
{	
	model = glm::translate(model, origen + pos);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	modelRender.RenderModel();
}

// Función - Renderizar mesh
void RenderMeshWithTexture(Mesh* mesh, glm::vec3 color, glm::vec3 position, glm::vec3 scale,
	GLuint uniformModel, GLuint uniformColor, GLuint uniformSpecularIntensity, GLuint uniformShininess,
	Material* material = nullptr, Texture* texture = nullptr)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);                    // Reiniciar la matriz
	model = glm::translate(model, position);	// Posición
	model = glm::scale(model, scale);			// Escala

	if (texture != nullptr)
		texture->UseTexture();

	if (material != nullptr)
		material->UseMaterial(uniformSpecularIntensity, uniformShininess);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));

	mesh->RenderMesh();
}











///////////////////////////////KEYFRAMES/////////////////////

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0,
ciclo3, ciclo4, ciclo5, ciclo6;
bool animacion = false;

//NEW// Keyframes
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 100 //Número de cuadros máximos
int i_max_steps = 100; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;		//Variable para GiroAvion
	float giroAvionInc;		//Variable para IncrementoGiroAvion
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;			//El número de cuadros guardados actualmente desde 0 para no sobreescribir
bool play = false;
int playIndex = 0;


void saveFrame(void) //tecla L
{
	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	//Se agregan nuevas líneas para guardar más variables si es necesario

	//no volatil,se requiere agregar una forma de escribir a un archivo para guardar los frames
	std::ofstream file("keyframes.txt", std::ios::app);

	if (file.is_open()) {
		file << "Frame " << FrameIndex << ":\n";
		file << movAvion_x << " "
			<< movAvion_y << " "
			<< giroAvion << "\n";
	}
	else
	{
		printf("Error: no se pudo abrir keyframes.txt para escribir.\n");
	}
	FrameIndex++;
}

void loadFrames()
{
	std::ifstream file("keyframes.txt");

	if (!file.is_open())
	{
		printf("No se encontró keyframes.txt\n");
		return;
	}

	std::string line;
	int frameCount = 0;

	while (std::getline(file, line))
	{
		if (line.find("Frame") != std::string::npos) continue;

		std::istringstream iss(line);
		float x, y, giro;

		if (iss >> x >> y >> giro)
		{
			int index = FrameIndex + frameCount;
			KeyFrame[index].movAvion_x = x;
			KeyFrame[index].movAvion_y = y;
			KeyFrame[index].giroAvion = giro;
			frameCount++;
		}
	}
	file.close();
	FrameIndex += frameCount;
	printf("Se cargaron %d frames desde keyframes.txt\n", frameCount);
}

void resetElements(void) //Tecla 0
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

}

void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolación del próximo cuadro
			{

				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animación
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}
	}
}

void instrucciones()
{
	printf("\nTeclas para uso de Keyframes:\n"
		"1.-Presionar barra espaciadora para reproducir animacion.\n"
		"2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
	printf("3.-Presiona L para guardar frame\n"
		"4.-Presiona P para habilitar guardar nuevo frame\n"
		"5.-Presiona 1 para mover en x\n"
		"6.-Presiona 2 para habilitar mover en x\n"
		"7.-Presiona 3 para mover en y\n"
		"8.-Presiona 4 para habilitar mover en y\n"
		"9.-Presiona 5 para rotar 180°\n"
		"10.-Presiona 6 para habilitar rotar 180°\n"
		"11.-Presiona SHIFT + 'cualquier tecla para mover' para ir en sentido contrario\n");
}
///////////////* FIN KEYFRAMES*////////////////////////////

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;

		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("giroAvion es: %f\n", giroAvion);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}

	bool decremento = keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT];
	float incremento = 1.0f * (decremento ? -1.0f : 1.0f);

	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += incremento;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}
	if (keys[GLFW_KEY_3])
	{
		if (ciclo3 < 1)
		{
			movAvion_y += incremento;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo3++;
			ciclo4 = 0;
			printf("\n Presiona la tecla 4 para habilitar nuevamente la variable Y\n");
		}
	}

	if (keys[GLFW_KEY_4])
	{
		if (ciclo4 < 1)
		{
			ciclo3 = 0;
			ciclo4++;
			printf("\n Ya puedes modificar tu variable Y presionando la tecla 3\n");
		}
	}
	if (keys[GLFW_KEY_5])
	{
		if (ciclo5 < 1)
		{
			giroAvion += 180.0f; // cambia el valor según tu escala de rotación
			printf("\n giroAvion es: %f\n", giroAvion);
			ciclo5++;
			ciclo6 = 0;
			printf("\n Presiona la tecla 6 para habilitar nuevamente la rotación\n");
		}
	}
	if (keys[GLFW_KEY_6])
	{
		if (ciclo6 < 1)
		{
			ciclo5 = 0;
			ciclo6++;
			printf("\n Ya puedes modificar la rotación presionando la tecla 5\n");
		}
	}
}












// -------------------------------------------------------------------------- Main
int main()
{
	auto start = std::chrono::high_resolution_clock::now(); // Inicio del cronómetro

	// Iniciamos el contexto
	mainWindow = Window(1366, 768); // Resolución: 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	// Recursos
	ResourceManager resources;

	CreatePrimitives();				// Cargamos las primitivas en meshList
	CreateShaders();				// Creamos los shaders
	SetLights();					// Creamos las luces

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	auto end = std::chrono::high_resolution_clock::now(); // Fin del cronómetro
	std::chrono::duration<double> loadTime = end - start;
	std::cout << "Tiempo total de carga: " << loadTime.count() << " segundos\n";

	// Variables uniform
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, 
			uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0;

	// Vector de proyección
	glm::mat4 projection = glm::perspective(
			45.0f,				// Ángulo de vision
			(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), // Aspecto
			0.1f, 2800.0f);		// Alcance: cerca | lejos
	
	// Matrices para los modelos
	glm::mat4 modelBase(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelZoo(1.0);		// Matriz para los animales
	glm::mat4 modelChicken(1.0);	// Matriz para los modelos de Chicken Little
	glm::mat4 modelTotoro(1.0);		// Matriz para los modelos de Totoro
	glm::mat4 modelChilly(1.0);		// Matriz para los modelos de Chilly Willy
	glm::mat4 modelRikoche(1.0);	// Matriz para los modelos de Rikoche

	// Posicion del origen
	glm::vec3 origen = glm::vec3(0.0f, 0.0f, 0.0f);

	// Posiciones de cada cuadrante
	glm::vec3 origenChicken = glm::vec3(-680.0f, 0.0f, -695.0f);// Cuadrante superior izq
	glm::vec3 origenChilly = glm::vec3(-690.0f, 0.0f, 690.0f);	// Cuadrante inferior izq
	glm::vec3 origenRikoche = glm::vec3(720.0f, 0.0f, 700.0f);	// Cuadrante inferior der
	glm::vec3 origenTotoro = glm::vec3(690.0f, 0.0f, -690.0f);	// Cuadrante superior der

	// Posiciones para los animales
	std::vector<glm::vec3> posAnimals;

	// Variables auxiliares
	glm::vec3 color = glm::vec3(1.0f);		// Para mandar un color
	glm::vec3 pos = glm::vec3(1.0f);		// Para mover los modelos
	glm::vec3 scal = glm::vec3(1.0f);		// Para escalar los modelos
	float rot = 0.0f;						// Para rotar los modelos
	
	// Ajustes para la cámara
	camera = Camera(
		//origenChicken + glm::vec3(325.0f, 50.0f, 325.0f),// Posicion inicial
		origen + glm::vec3(0.0f, 30.0f, 50.0f),// Posicion inicial
		glm::vec3(0.0f, 1.0f, 0.0f),			// Direccion de nuestro "arriba"
		//-135.0f, 0.0f,						// Rotacion horizontal | Rotacion vertical
		-90.0f, 0.0f,						// Rotacion horizontal | Rotacion vertical
		4.0f, 0.45f);					// Velocidad de movimiento | Velocidad de rotacion

	// KEYFRAMES ---------------------------------------------------------------------------
	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].giroAvion = 0;

	KeyFrame[1].movAvion_x = -15.0f;
	KeyFrame[1].movAvion_y = 50.0f;
	KeyFrame[1].giroAvion = 0;

	KeyFrame[2].movAvion_x = -45.0f;
	KeyFrame[2].movAvion_y = 40.0f;
	KeyFrame[2].giroAvion = 0;

	KeyFrame[3].movAvion_x = -30.0f;
	KeyFrame[3].movAvion_y = -5.0f;
	KeyFrame[3].giroAvion = 180;

	KeyFrame[4].movAvion_x = 20.0f;
	KeyFrame[4].movAvion_y = 25.0f;
	KeyFrame[4].giroAvion = 180.0f;

	KeyFrame[5].movAvion_x = 50.0f;
	KeyFrame[5].movAvion_y = 45.0f;
	KeyFrame[5].giroAvion = 180.0f;

	instrucciones();
	loadFrames();

	float speedAleteo = 15.0f;
	float angulo = 0.0f;
	float tiempo = 0.0f;
	float deltaTiempo = 0.015f;

	glm::vec3 posMariposaAlas = glm::vec3(0.0f, 0.0f, 0.0f);

	while (!mainWindow.getShouldClose()) // Loop Principal 
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// Recibir eventos del usuario 
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Limpiamos la window
		mainWindow.Clear();

		// Dibujamos el Skybox 
		resources.skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		// Configuracion para la iluminación
		
		// código...
		
		// KEYFRAMES -------------------------------------------------------------
		inputKeyframes(mainWindow.getsKeys());
		animate();


		// Info en el Shader 
		shaderList[0].UseShader();
		uniformModel =				shaderList[0].GetModelLocation();				// modelo
		uniformProjection =			shaderList[0].GetProjectionLocation();			// proyección
		uniformView =				shaderList[0].GetViewLocation();				// vista
		uniformEyePosition =		shaderList[0].GetEyePositionLocation();			// -
		uniformColor =				shaderList[0].getColorLocation();				// color
		uniformSpecularIntensity =	shaderList[0].GetSpecularIntensityLocation();	// intensidad especular
		uniformShininess =			shaderList[0].GetShininessLocation();			// brillo

		shaderList[0].SetDirectionalLight(&mainLight);								// iluminación direccional
		shaderList[0].SetPointLights(PL_0, pointLightCount);						// iluminación pointlight
		shaderList[0].SetSpotLights(SP_lamparas, spotLightCount);					// iluminación spotlight

		glUniformMatrix4fv(uniformProjection,	1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView,			1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// ------------------------------------------------------------------ Primitivas

		/*
				Mesh	indice
			Suelo		 [0]
			Vegetacion   [1]
				
		*/

		// Suelo
		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, -0.05f, 0.0f);
		scal = glm::vec3(100.0f, 1.0f, 100.0f);

		RenderMeshWithTexture(meshList[0], color, pos, scal, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess, 
			&Material_opaco, &resources.croquisTexture);

		// ------------------------------------------------------------------ Zoológico
		std::vector<glm::vec3> posAnimals = {
			{50.0f, 0.0f, 80.0f},		// Zorro
			{40.0f, 0.0f, 80.0f},		// Tigre
			{30.0f, 0.0f, 80.0f},		// Rinoceronte
			{20.0f, 0.0f, 80.0f},		// Pantera
			{10.0f, 0.0f, 80.0f},		// Panda Rojo
			{0.0f, 0.0f, 80.0f},		// Orangutan
			{-10.0f, 0.0f, 80.0f}		// Pedro
		};
		modelZoo = glm::mat4(1.0);
		for (size_t i = 0; i < resources.Zoologico.size(); i++)
			RenderModel(modelZoo, uniformModel, origen, posAnimals[i], resources.Zoologico[i].modelo);

		// ------------------------------------------------------------------ Arena Central
		modelBase = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelBase, uniformModel, origen, pos, resources.ArenaCentral);

		// ------------------------------------------------------------------ Universo de Chicken Little
		
		// Piramide de Chicken Little
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.PiramideChicken);

		// Edificios
		std::vector<glm::vec3> posEdificios = {
			{-100.0f, 0.0f, 250.0f},	// Casa
			{380.0f, 0.0f, -120.0f},	// Ayuntamiento
			{360.0f, 0.0f, -120.0f},	// Estacion de bomberos
			{350.0f, 0.0f, -120.0f},	// Torre de agua
			{-20.0f, 0.0f, 400.0f},		// Edificio naranja
			{-20.0f, 0.0f, 420.0f},		// Edificio Tienda
			{-20.0f, 0.0f, 440.0f},		// Edificio Oaks
		};
		modelChicken = glm::mat4(1.0);
		for (size_t i = 0; i < resources.EdificiosLittle.size(); i++) {
			RenderModel(modelChicken, uniformModel, origenChicken, glm::vec3(0.0f, 0.0f, 0.0f), resources.EdificiosLittle[i]);
		}

		// Calle Chicken
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.CalleChicken);

		// Arboles
		modelChicken = glm::mat4(1.0);
		for (size_t i = 0; i < resources.Arboles.size(); i++) {
			RenderModel(modelChicken, uniformModel, origenChicken, glm::vec3(0.0f, 0.0f, 0.0f), resources.Arboles[i]);
		}

		// Mariposa Azul
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 35.0f + movAvion_y, 0.0f + movAvion_x);
		RenderModel(modelChicken, uniformModel, origen, pos, resources.MariposaAzul);
		//modelaux = modelChicken;

		//tiempo += deltaTiempo;
		//angulo = 35.0f * sin(speedAleteo * tiempo);

		//posMariposaAlas = pos + glm::vec3(-1.0f, 0.0f, 0.0f);
		//modelChicken = modelaux;
		//modelChicken = glm::rotate(modelChicken, glm::radians(angulo), glm::vec3(0, 1, 0));
		//RenderModel(modelChicken, uniformModel, origen, posMariposaAlas, resources.MariposaAzul_AlaDerecha);

		//posMariposaAlas = pos + glm::vec3(1.0f, 0.0f, 0.0f);
		//modelChicken = modelaux;
		//modelChicken = glm::rotate(modelChicken, glm::radians(-angulo), glm::vec3(0, 1, 0));
		//RenderModel(modelChicken, uniformModel, origen, posMariposaAlas, resources.MariposaAzul_AlaIzquierda);

		// Cielo Hexagonal
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.CieloHexagonal);

		// Chicken Little
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(290.0f, 0.0f, 290.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.ChickenLittle);

		// Abby Patosa
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(280.0f, 0.0f, 280.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.AbbyPatosa);

		// Runt
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(265.0f, 0.0f, 265.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Runt);

		// PezOutWater
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(295.0f, 0.0f, 295.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.PezOutWater);

		// Kirby
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(300.0f, 0.0f, 300.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Kirby);

		// Buck Gallo
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(275.0f, 0.0f, 275.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.BuckGallo);

		// Melvin
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(305.0f, 0.0f, 305.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Melvin);

		// Tina
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(310.0f, 0.0f, 310.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Tina);

		// Ace
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(320.0f, 0.0f, 320.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.Ace);

		// Robot Alien
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(320.0f, 0.0f, 320.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.RobotAlien);

		// The Dog
		modelChicken = glm::mat4(1.0);
		pos = glm::vec3(325.0f, 0.0f, 325.0f);
		RenderModel(modelChicken, uniformModel, origenChicken, pos, resources.TheDog);
	
		// ------------------------------------------------------------------ Universo de Chilly Willy

		// Piramide de Chilli Willy
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelChilly, uniformModel, origenChilly, pos, resources.PiramideChilly);

		// Chilly Willy
		pos = glm::vec3(10.0f, 15.0f, 25.0f);
		RenderModel(modelChilly, uniformModel, origen, pos, resources.Chilly_Willy);

		// ------------------------------------------------------------------ Universo de Rikoche 
		
		// Piramide de Rikoche
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelRikoche, uniformModel, origenRikoche, pos, resources.PiramideRikoche);

		// Rikoche
		pos = glm::vec3(0.0f, 15.0f, 35.0f);
		RenderModel(modelRikoche, uniformModel, origen, pos, resources.Rikoche);

		// ------------------------------------------------------------------ Universo de Totoro 

		// Piramide de Totoro
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		RenderModel(modelTotoro, uniformModel, origenTotoro, pos, resources.PiramideTotoro);

		// Totoro (Ō-Totoro)
		pos = glm::vec3(0.0f, 10.0f, 20.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro);

		// Totoro mediano (Chū-Totoro)
		pos = glm::vec3(10.0f, 15.0f, 20.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_mediano);

		// Totoro chiquito (Chibi-Totoro)
		pos = glm::vec3(-7.0f, 15.0f, 20.0f);
		RenderModel(modelTotoro, uniformModel, origen, pos, resources.Totoro_chiquito);

		// ------------------------------------------------------------------ Modelos con Blending (transparencia o traslucidez)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// Instanciar modelos aqui...

		glDisable(GL_BLEND);

		// ------------------------------------------------------------------ End of program 
		glUseProgram(0);
		mainWindow.swapBuffers();
	}	
	return 0;
}