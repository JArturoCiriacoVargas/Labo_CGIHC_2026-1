#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

// Constantes ---------------------------------------------------------
const float toRadians = 3.14159265f / 180.0f;
static double limitFPS = 1.0 / 60.0;

// Objetos globales ---------------------------------------------------------
Window mainWindow;
Camera camera;
Skybox skybox;

using std::vector;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Texture brickTexture, dirtTexture, plainTexture, pisoTexture, dadoTexture, dado8Texture, logofiTexture, rinTexture, llantaTexture, autoTexture, ParabrisasTexture, CofreTexture;
Model Kitt_M, Llanta_M, Dadoobj_M, Dadodae_M, Dadofbx_M, RinPorsche_M, LlantaPorsche_M, PorscheGT_M, Parabrisas_M, Cofre_M, Exterior_M;

GLfloat deltaTime = 0.0f, lastTime = 0.0f;

// Shaders ---------------------------------------------------------
static const char* vShader = "shaders/shader_texture.vert";
static const char* fShader = "shaders/shader_texture.frag";

// Función para calcular normales ---------------------------------------------------------
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset) {
    for (size_t i = 0; i < indiceCount; i += 3) {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;

        glm::vec3 v1(vertices[in1] - vertices[in0],
            vertices[in1 + 1] - vertices[in0 + 1],
            vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0],
            vertices[in2 + 1] - vertices[in0 + 1],
            vertices[in2 + 2] - vertices[in0 + 2]);

        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;

        for (int j = 0; j < 3; j++) {
            vertices[in0 + j] += normal[j];
            vertices[in1 + j] += normal[j];
            vertices[in2 + j] += normal[j];
        }
    }

    for (size_t i = 0; i < verticeCount / vLength; i++) {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        for (int j = 0; j < 3; j++) vertices[nOffset + j] = vec[j];
    }
}
// Crear geometrías base ---------------------------------------------------------
void CreateObjects() {
    unsigned int indices[] = { 0,3,1, 1,3,2, 2,3,0, 0,1,2 };

    GLfloat vertices[] = {
        //  x       y       z			u	  v			nx	  ny    nz
            -1.0f,  -1.0f,  -0.6f,	    0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
            0.0f,   -1.0f,  1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
            1.0f,   -1.0f,  -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
            0.0f,   1.0f,   0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = { 0,2,1, 1,2,3 };

    GLfloat floorVertices[] = {
        //  x       y       z			u	    v			nx      ny      nz
            -10.0f, 0.0f,   -10.0f,	    0.0f,   0.0f,		0.0f,   -1.0f,  0.0f,
            10.0f,  0.0f,   -10.0f,	    10.0f,  0.0f,	    0.0f,   -1.0f,  0.0f,
            -10.0f, 0.0f,   10.0f,  	0.0f,   10.0f,	    0.0f,   -1.0f,  0.0f,
            10.0f,  0.0f,   10.0f,		10.0f,  10.0f,	    0.0f,   -1.0f,  0.0f
    };

    unsigned int vegetacionIndices[] = { 0,1,2, 0,2,3, 4,5,6, 4,6,7 };

    GLfloat vegetacionVertices[] = {
        //  x       y       z			u	    v		    nx      ny      nz
            -0.5f,  -0.5f,  0.0f,		0.0f,   0.0f,	    0.0f,   0.0f,   0.0f,
            0.5f,   -0.5f,  0.0f,		1.0f,   0.0f,	    0.0f,   0.0f,   0.0f,
            0.5f,   0.5f,   0.0f,		1.0f,   1.0f,	    0.0f,   0.0f,   0.0f,
            -0.5f,  0.5f,   0.0f,		0.0f,   1.0f,	    0.0f,   0.0f,   0.0f,

            0.0f,   -0.5f,  -0.5f,		0.0f,   0.0f,	    0.0f,   0.0f,   0.0f,
            0.0f,   -0.5f,  0.5f,		1.0f,   0.0f,	    0.0f,   0.0f,   0.0f,
            0.0f,   0.5f,   0.5f,		1.0f,   1.0f,	    0.0f,   0.0f,   0.0f,
            0.0f,   0.5f,   -0.5f,		0.0f,   1.0f,	    0.0f,   0.0f,   0.0f,
    };

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);

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
}
// Crear dado ---------------------------------------------------------
void CreateDado() {
    std::vector <unsigned int> cubo_indices = { 0,1,2,2,3,0, 8,9,10,10,11,8, 12,13,14,14,15,12, 16,17,18,18,19,16, 20,21,22,22,23,20, 4,5,6,6,7,4 };

    std::vector<GLfloat> cubo_vertices = {
            // front
            //x		y		z		S		T			NX		NY		NZ
            -0.5f, -0.5f,  0.5f,	0.00f,  0.00f,		0.0f,	0.0f,	-1.0f,	//0 abajo izquierda
            0.5f, -0.5f,  0.5f,		1.00f,	0.00f,		0.0f,	0.0f,	-1.0f,	//1 abajo derecha
            0.5f,  0.5f,  0.5f,		1.00f,	1.00f,		0.0f,	0.0f,	-1.0f,	//2 arriba derecha
            -0.5f,  0.5f,  0.5f,	0.00f,	1.00f,		0.0f,	0.0f,	-1.0f,	//3 arriba izquierda
            // right
            //x		y		z		S		T
            0.5f, -0.5f,  0.5f,	    0.76f,  0.34f,		-1.0f,	0.0f,	0.0f,   
            0.5f, -0.5f,  -0.5f,	0.99f,	0.34f,		-1.0f,	0.0f,	0.0f,
            0.5f,  0.5f,  -0.5f,	0.99f,	0.66f,		-1.0f,	0.0f,	0.0f,
            0.5f,  0.5f,  0.5f,	    0.76f,	0.66f,		-1.0f,	0.0f,	0.0f,
            // back
            -0.5f, -0.5f, -0.5f,	0.24f,  0.34f,		0.0f,	0.0f,	1.0f,
            0.5f, -0.5f, -0.5f,		0.01f,	0.34f,		0.0f,	0.0f,	1.0f,
            0.5f,  0.5f, -0.5f,		0.01f,	0.65f,		0.0f,	0.0f,	1.0f,
            -0.5f,  0.5f, -0.5f,	0.24f,	0.65f,		0.0f,	0.0f,	1.0f,

            // left
            //x		y		z		S		T
            -0.5f, -0.5f,  -0.5f,	0.51f,  0.34f,		1.0f,	0.0f,	0.0f,
            -0.5f, -0.5f,  0.5f,	0.74f,	0.34f,		1.0f,	0.0f,	0.0f,
            -0.5f,  0.5f,  0.5f,	0.74f,	0.65f,		1.0f,	0.0f,	0.0f,
            -0.5f,  0.5f,  -0.5f,	0.51f,	0.65f,		1.0f,	0.0f,	0.0f,

            // bottom
            //x		y		z		S		T
            -0.5f, -0.5f,  0.5f,	0.74f,  0.67f,		0.0f,	1.0f,	0.0f,
            0.5f,  -0.5f,  0.5f,	0.51f,	0.67f,		0.0f,	1.0f,	0.0f,
             0.5f,  -0.5f,  -0.5f,	0.51f,	0.99f,		0.0f,	1.0f,	0.0f,
            -0.5f, -0.5f,  -0.5f,	0.74f,	0.99f,		0.0f,	1.0f,	0.0f,

            //UP
             //x		y		z		S		T
             -0.5f, 0.5f,  0.5f,	0.51f,  0.01f,		0.0f,	-1.0f,	0.0f,
             0.5f,  0.5f,  0.5f,	0.74f,	0.01f,		0.0f,	-1.0f,	0.0f,
              0.5f, 0.5f,  -0.5f,	0.74f,	0.33f,		0.0f,	-1.0f,	0.0f,
             -0.5f, 0.5f,  -0.5f,	0.51f,	0.33f,		0.0f,	-1.0f,	0.0f,
    };
    Mesh* dado = new Mesh();
    dado->CreateMesh(&cubo_vertices[0], &cubo_indices[0], (float)cubo_vertices.size(), (unsigned int)cubo_indices.size());
    meshList.push_back(dado);
}
// Crear dado 8 caras
void CreateDado8()
{
    // índices: cada triángulo con orden CCW (3 por cara)
    std::vector<unsigned int> dado8_indices = 
    {    0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17, 18,19,20, 21,22,23   };

    std::vector<GLfloat> dado8_vertices = {
        
        // parte superior
        // x,   y,      z,      u,      v,          nx,     ny,    nz
        // cara 4
        0.0f,  0.5f,  0.0f,     0.50f, 0.75f,       0.0f,  1.0f,  0.0f, // 0
        0.0f,  0.0f,  0.5f,     0.25f, 0.50f,       0.0f,  0.0f,  1.0f, // 1
        0.5f,  0.0f,  0.0f,     0.75f, 0.50f,       1.0f,  0.0f,  0.0f, // 2

        // cara 1
        0.0f,  0.5f,  0.0f,     0.50f, 0.25f,       0.0f,  1.0f,  0.0f, // 3
        0.0f,  0.0f,  0.5f,     0.25f, 0.50f,       0.0f,  0.0f,  1.0f, // 4
        -0.5f, 0.0f,  0.0f,     0.75f, 0.50f,       -1.0f, 0.0f,  0.0f, // 5

        // cara 3
        0.0f,  0.5f,  0.0f,     1.00f, 0.25f,       0.0f,  1.0f,  0.0f, // 6
        0.5f,  0.0f,  0.0f,     0.75f, 0.50f,       1.0f,  0.0f,  0.0f, // 7
        0.0f,  0.0f, -0.5f,     0.50f, 0.25f,       0.0f,  0.0f, -1.0f, // 8

        // cara 2
        0.0f,  0.5f,  0.0f,     1.00f, 0.75f,       0.0f,  1.0f,  0.0f, // 9
        -0.5f, 0.0f,  0.0f,     0.75f, 0.50f,       -1.0f, 0.0f,  0.0f, // 10
        0.0f,  0.0f, -0.5f,     0.50f, 0.75f,       0.0f,  0.0f, -1.0f, // 11

        // parte inferior
        // x,   y,      z,      u,      v,          nx,     ny,    nz
        // cara 8
        0.0f, -0.5f,  0.0f,     0.25f, 1.00f,       0.0f, -1.0f,  0.0f, // 12
        0.0f,  0.0f,  0.5f,     0.50f, 0.75f,       0.0f,  0.0f,  1.0f, // 13
        0.5f,  0.0f,  0.0f,     0.00f, 0.75f,       1.0f,  0.0f,  0.0f, // 14

        // cara 5
        0.0f, -0.5f,  0.0f,     0.25f, 0.00f,       0.0f, -1.0f,  0.0f, // 15
        0.0f,  0.0f,  0.5f,     0.50f, 0.25f,       0.0f,  0.0f,  1.0f, // 16
        -0.5f, 0.0f,  0.0f,     0.00f, 0.25f,       -1.0f, 0.0f,  0.0f, // 17

        // cara 7
        0.0f, -0.5f,  0.0f,     0.00f, 0.25f,       0.0f, -1.0f,  0.0f, // 18
        0.5f,  0.0f,  0.0f,     0.25f, 0.50f,       1.0f,  0.0f,  0.0f, // 19
        0.0f,  0.0f, -0.5f,     0.50f, 0.25f,       0.0f,  0.0f, -1.0f, // 20

        // cara 6
        0.0f, -0.5f,  0.0f,     0.00f, 0.75f,       0.0f, -1.0f,  0.0f, // 21
        -0.5f, 0.0f,  0.0f,     0.25f, 0.50f,       -1.0f, 0.0f,  0.0f, // 22
        0.0f,  0.0f, -0.5f,     0.50f, 0.75f,       0.0f,  0.0f, -1.0f  // 23
    };
    Mesh* dado8 = new Mesh();
    dado8->CreateMesh(&dado8_vertices[0], &dado8_indices[0], (float)dado8_vertices.size(), (unsigned int)dado8_indices.size());
    meshList.push_back(dado8);
}
// Cargar texturas y modelos ---------------------------------------------------------
void LoadAssets() {
    brickTexture    =  Texture("Textures/brick.png"); brickTexture.LoadTextureA();
    dirtTexture     =  Texture("Textures/dirt.png");  dirtTexture.LoadTextureA();
    plainTexture    =  Texture("Textures/plain.png"); plainTexture.LoadTextureA();
    pisoTexture     =  Texture("Textures/piso.tga");  pisoTexture.LoadTextureA();
    dadoTexture     =  Texture("Textures/dado_animales_final.tga");    dadoTexture.LoadTextureA();
    dado8Texture    =  Texture("Textures/dado8.png");                  dado8Texture.LoadTextureA();
    logofiTexture   =  Texture("Textures/escudo_fi_color.tga");        logofiTexture.LoadTextureA();
    rinTexture      =  Texture("Textures/rin metal.png");              rinTexture.LoadTextureA();
    llantaTexture   =  Texture("Textures/caucho2.png");                llantaTexture.LoadTextureA();
    autoTexture     =  Texture("Textures/parabrisas.png");             autoTexture.LoadTextureA();
    ParabrisasTexture = Texture("Textures/parabrisas.png");             autoTexture.LoadTextureA();
    CofreTexture    = Texture("Textures/cofre.png");             CofreTexture.LoadTextureA();
    

    Kitt_M.LoadModel    ("Models/kitt_optimizado.obj");
    Llanta_M.LoadModel  ("Models/llanta_optimizada.obj");
    Dadodae_M.LoadModel ("Models/dadodae2.dae");
    Dadofbx_M.LoadModel ("Models/dadofbx.fbx");
    Dadoobj_M.LoadModel ("Models/dadoobj.obj");
    RinPorsche_M.LoadModel("Models/Porsche GT2 RS_Rin.obj");
    LlantaPorsche_M.LoadModel("Models/Porsche GT2 RS_LlantaSinRin.obj");
    PorscheGT_M.LoadModel("Models/Porsche GT3 RS_ChasisSinLlantasCofre.obj");
    Parabrisas_M.LoadModel("Models/Porsche GT3 RS_Parabrisas.obj");
    Cofre_M.LoadModel("Models/Porsche GT2 RS_Cofre.obj");
    Exterior_M.LoadModel("Models/Porsche GT3 RS_Exterior.obj");
    

    std::vector<std::string> faces = {
        "Textures/Skybox/cupertin-lake_rt.tga",
        "Textures/Skybox/cupertin-lake_lf.tga",
        "Textures/Skybox/cupertin-lake_dn.tga",
        "Textures/Skybox/cupertin-lake_up.tga",
        "Textures/Skybox/cupertin-lake_bk.tga",
        "Textures/Skybox/cupertin-lake_ft.tga"
    };
    skybox = Skybox(faces);
}
// Crear shaders ---------------------------------------------------------
void CreateShaders() {
    Shader* shader = new Shader();
    shader->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader);
}
// Función para dibujar un mesh con su color y transformaciones
void drawMesh(Shader& shader, GLuint uniformModel, GLuint uniformColor, glm::mat4 transform, glm::vec3 color, Mesh* meshToRender)
{
    glUniform3fv(uniformColor, 1, glm::value_ptr(color));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(transform));
    meshToRender->RenderMesh();
}
// Función para dibujar un modelo con su color y transformaciones
void drawModel(Shader& shader, GLuint uniformModel, GLuint uniformColor, glm::mat4 transform, glm::vec3 color, Model& modelToRender)
{
    glUniform3fv(uniformColor, 1, glm::value_ptr(color));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(transform));
    modelToRender.RenderModel();
}

// Main ----------------------------------------------------------------------
int main() 
{
    mainWindow = Window(1366, 768);
    mainWindow.Initialise();

    CreateObjects();
    CreateDado();
    CreateDado8();
    CreateShaders();
    LoadAssets();

    camera = Camera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0, uniformColor = 0;

    float angulo = 0.0f;

    // Bucle principal ---------------------------------------------------------
    while (!mainWindow.getShouldClose()) {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        mainWindow.Clear();

        skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

        shaderList[0].UseShader();
        uniformModel =      shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView =       shaderList[0].GetViewLocation();
        uniformColor =      shaderList[0].getColorLocation();

        angulo += 0.2;

        glm::mat4 model(1.0f), modelaux(1.0f);
        glm::vec3 color(1.0f);

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        // Piso
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(30.0f));

        pisoTexture.UseTexture();
        drawMesh(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), meshList[2]);

        // Dado de 8 caras
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 14.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
        model = glm::rotate(model, glm::radians(angulo), glm::vec3(0, 1, 0));
        dado8Texture.UseTexture();
        drawMesh(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), meshList[5]);

        // Auto importado
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.85f, 0.0f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), PorscheGT_M);

        // Exterior importado
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(0.0f, 0.0f, 0.75f), Exterior_M);

        // Parabrisas importado
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 6.4f, 0.5f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), Parabrisas_M);

        // Cofre importado
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.5f, 11.56f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), Cofre_M);

        // Rin delantero izquierdo
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 1.0f, 10.75f));;

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), RinPorsche_M);

        // Rin delantero derecho
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 1.0f, 10.75f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), RinPorsche_M);

        // Rin trasero izquierdo
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(6.3f, 1.0f, -9.90f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), RinPorsche_M);

        // Rin trasero derecho
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 1.0f, -9.90f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), RinPorsche_M);

        // Llanta delantera izquierda
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 1.0f, 10.75f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), LlantaPorsche_M);

        // Llanta delantera derecha
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 1.0f, 10.75f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), LlantaPorsche_M);

        // Llanta trasera izquierda
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(6.3f, 1.0f, -9.90f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), LlantaPorsche_M);

        // Llanta trasera derecha
        model = glm::mat4(1.0);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 1.0f, -9.90f));

        drawModel(shaderList[0], uniformModel, uniformColor, model, glm::vec3(1.0f), LlantaPorsche_M);


        glUseProgram(0);
        mainWindow.swapBuffers();
    }
    return 0;
}