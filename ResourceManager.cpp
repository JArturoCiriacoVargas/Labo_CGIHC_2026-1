#include "ResourceManager.h"

ResourceManager::ResourceManager() {

    // Skybox
    std::vector<std::string> blueSky;
    blueSky.push_back("Texturas/Skybox/blueSky/derecha.png");		// right (voltear verticalmente)
    blueSky.push_back("Texturas/Skybox/blueSky/izquierda.png");		// left
    blueSky.push_back("Texturas/Skybox/blueSky/abajo.png");			// bottom
    blueSky.push_back("Texturas/Skybox/blueSky/arriba.png");		// up (girar 90° en sentido antihorario)
    blueSky.push_back("Texturas/Skybox/blueSky/atras.png");			// back
    blueSky.push_back("Texturas/Skybox/blueSky/enfrente.png");		// front
    skybox = Skybox(blueSky);

    // Texturas
    plainTexture = Texture("Texturas/plain.png");       plainTexture.LoadTextureA();
    pastoTexture = Texture("Texturas/Pasto.png");       pastoTexture.LoadTextureA();
    croquisTexture = Texture("Texturas/Croquis.jpg");   croquisTexture.LoadTextureA();

    // Modelos de las piramides
    //ArenaCentral.LoadModel("Modelos/Decoracion Prehispanica/Arena Central.obj");
    /*PiramideChilly.LoadModel("Modelos/Piramides/Piramide de Quetzalcoatl.obj");
    PiramideRikoche.LoadModel("Modelos/Piramides/Templo de Lucha.obj");
    PiramideTotoro.LoadModel("Modelos/Piramides/Templo de Kukulcan.obj");*/

    // Modelos de los animales del Zoológico
    /*Zoologico = 
    {
        {"Zorro",        "Modelos/Zoologico/Zorro.obj"},
        {"Tigre",        "Modelos/Zoologico/Tigre.obj"},
        {"Rinoceronte",  "Modelos/Zoologico/Rinoceronte.obj"},
        {"Pantera",      "Modelos/Zoologico/Pantera.obj"},
        {"Panda Rojo",   "Modelos/Zoologico/Panda Rojo.obj"},
        {"Orangutan",    "Modelos/Zoologico/Orangutan.obj"},
        {"Mapache",      "Modelos/Zoologico/Mapache.obj"}
    };
    for (auto& animal : Zoologico)
        animal.modelo.LoadModel(animal.ruta);*/

    // Personajes principales
    ChickenLittle.LoadModel("Modelos/Universo Chicken Little/Chicken Little.obj");

    /*Chilly_Willy.LoadModel("Modelos/Universo Chilly Willy/Chilly Willy.obj");
    Rikoche.LoadModel("Modelos/Universo Rikoche/Rikoche.obj");
    Totoro.LoadModel("Modelos/Universo Totoro/Totoro.obj");
    Totoro_mediano.LoadModel("Modelos/Universo Totoro/Totoro_mediano.obj");
    Totoro_chiquito.LoadModel("Modelos/Universo Totoro/Totoro_chiquito.obj");*/

    // Universo Chiquito Chicken
    PiramideChicken.LoadModel("Modelos/Piramides/Piramide de Chicken.obj");

    EdificiosLittle.push_back(Model("Modelos/Universo Chicken Little/Chicken Casa.obj"));
    EdificiosLittle.push_back(Model("Modelos/Universo Chicken Little/Ayuntamiento.obj"));
    EdificiosLittle.push_back(Model("Modelos/Universo Chicken Little/Estacion de Bomberos.obj"));
    EdificiosLittle.push_back(Model("Modelos/Universo Chicken Little/Torre de Agua.obj"));
    EdificiosLittle.push_back(Model("Modelos/Universo Chicken Little/Edificio Naranja.obj"));
    EdificiosLittle.push_back(Model("Modelos/Universo Chicken Little/Edificio Tienda.obj"));
    EdificiosLittle.push_back(Model("Modelos/Universo Chicken Little/Edificio Oaks.obj"));

    CalleChicken.LoadModel(     "Modelos/Universo Chicken Little/Calle Chicken.obj");
    CieloHexagonal.LoadModel(   "Modelos/Universo Chicken Little/Cielo Hexagonal.obj");
    Hexagono.LoadModel(         "Modelos/Universo Chicken Little/Hexagono.obj");

    Arboles.push_back(Model("Modelos/Universo Chicken Little/Arbol 1.obj"));
    Arboles.push_back(Model("Modelos/Universo Chicken Little/Arbol 2.obj"));

    MariposaAzul.LoadModel("Modelos/Universo Chicken Little/Mariposa Azul - completa.obj");
    MariposaAzul_AlaDerecha.LoadModel("Modelos/Universo Chicken Little/Mariposa Azul - Ala Derecha.obj");
    MariposaAzul_AlaIzquierda.LoadModel("Modelos/Universo Chicken Little/Mariposa Azul - Ala Izquierda.obj");

    AbbyPatosa.LoadModel(   "Modelos/Universo Chicken Little/Abby Patosa.obj");
    Runt.LoadModel(         "Modelos/Universo Chicken Little/Runt.obj");
    PezOutWater.LoadModel(  "Modelos/Universo Chicken Little/PezOutWater.obj");
    Kirby.LoadModel(        "Modelos/Universo Chicken Little/Kirby.obj");
    BuckGallo.LoadModel(    "Modelos/Universo Chicken Little/Buck Gallo.obj");
    Melvin.LoadModel(       "Modelos/Universo Chicken Little/Melvin.obj");
    Tina.LoadModel(         "Modelos/Universo Chicken Little/Tina.obj");
    Ace.LoadModel(          "Modelos/Universo Chicken Little/Ace.obj");
    RobotAlien.LoadModel(   "Modelos/Universo Chicken Little/Robot Alien.obj");
    TheDog.LoadModel(       "Modelos/Universo Chicken Little/The Dog.obj");
}