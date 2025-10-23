#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

	rotax = 0.0f;
	rotay = 0.0f;
	rotaz = 0.0f;
	articulacion1 = 0.0f;
	articulacion2 = 0.0f;
	articulacion3 = 0.0f;
	articulacion4 = 0.0f;
	articulacion5 = 0.0f;
	articulacion6 = 0.0f;
	rueda1 = 0.0f;
	rueda2 = 0.0f;
	rueda3 = 0.0f;
	rueda4 = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "PracticaXX:Nombre de la practica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}
void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}
GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}
void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}
	//movimiento auto
	if (key == GLFW_KEY_O)
	{
		theWindow->poszAuto += 1.0f;
		theWindow->rotLlanta += 3.5f;
		theWindow->lightFront = 1.0f;
	}
	if (key == GLFW_KEY_L)
	{
		theWindow->poszAuto -= 1.0f;
		theWindow->rotLlanta -= 3.5f;
		theWindow->lightFront = 0.0f;
	}
	//rotacion del cofre
	if (key == GLFW_KEY_I)
	{
		theWindow->rotCofre += 4.0f;
		if (theWindow->rotCofre >= 45.0f) {
			theWindow->rotCofre = 45.0f;
		}
	}
	if (key == GLFW_KEY_K)
	{
		theWindow->rotCofre -= 4.0f;
		if (theWindow->rotCofre <= 0.0f) {
			theWindow->rotCofre = 0.0f;
		}
	}
	//movimiento helicoptero
	if (key == GLFW_KEY_UP)
	{
		theWindow->poszHelicoptero += 4.0f;
	}
	if (key == GLFW_KEY_DOWN)
	{
		theWindow->poszHelicoptero -= 4.0f;
	}
	if (key == GLFW_KEY_U && action == GLFW_PRESS) { theWindow->useArreglo1 = 1.0f; }
	if (key == GLFW_KEY_I) { theWindow->useArreglo1 = 0.0f; }

	//lámpara
	if (key == GLFW_KEY_P && action == GLFW_PRESS) 
	{
		theWindow->lightOff = (theWindow->lightOff < 1.0f) ? 1.0f : 0.0f;
	}
	//hexagono
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		theWindow->lightHexagon = (theWindow->lightHexagon < 1.0f) ? 1.0f : 0.0f;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}
void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

void Window::Clear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
