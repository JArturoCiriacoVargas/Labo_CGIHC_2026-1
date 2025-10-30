#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	void Clear();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	GLfloat getrotay() { return rotay; }
	GLfloat getrotax() { return rotax; }
	GLfloat getrotaz() { return rotaz; }
	GLfloat getarticulacion1() { return articulacion1; }
	GLfloat getarticulacion2() { return articulacion2; }
	GLfloat getarticulacion3() { return articulacion3; }
	GLfloat getarticulacion4() { return articulacion4; }
	GLfloat getarticulacion5() { return articulacion5; }
	GLfloat getarticulacion6() { return articulacion6; }
	GLfloat getrueda1() { return rueda1; }
	GLfloat getrueda2() { return rueda2; }
	GLfloat getrueda3() { return rueda3; }
	GLfloat getrueda4() { return rueda4; }
	GLfloat getcola() { return cola; }

	//arco
	GLfloat getAbrirPuerta() { return abrirPuerta; }
	void setAbrirPuerta(GLfloat valor) { abrirPuerta = valor; }
	bool getAbriendo() { return abriendo; }
	//auto
	GLfloat getPozzAuto() { return poszAuto; }
	GLfloat getRotCofre() { return rotCofre; }
	GLfloat getRotLlanta() { return rotLlanta; }
	GLfloat getLightFront() { return lightFront; }
	//helicoptero
	GLfloat getPoszHelicoptero() { return poszHelicoptero; }
	//lámpara
	GLfloat getlightOff() { return lightOff; }
	//hexagono
	GLfloat getlightHexagon() { return lightHexagon; }

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat rotax, rotay, rotaz, articulacion1, articulacion2, articulacion3, articulacion4, articulacion5, articulacion6, rueda1, rueda2, rueda3, rueda4, cola;
	
	//arco
	GLfloat abrirPuerta = 0.0f;
	bool abriendo = false;
	//auto
	GLfloat poszAuto = 0.0f;
	GLfloat rotCofre = 0.0f;
	GLfloat rotLlanta = 0.0f;
	GLfloat lightFront = 0.0f;
	//helicoptero
	GLfloat poszHelicoptero = 0.0f;
	GLfloat useArreglo1;
	//lámpara
	GLfloat lightOff = 0.0f;
	//hexagono
	GLfloat lightHexagon = 0.0f;

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

