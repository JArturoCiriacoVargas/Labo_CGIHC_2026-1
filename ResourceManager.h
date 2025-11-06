#pragma once
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"
#include <vector>
#include <string>

struct Animal 
{
    std::string nombre;
    std::string ruta;
    Model modelo;
};

class ResourceManager {
public:
    // Constructor
    ResourceManager();

    // Skybox
    Skybox skybox;

    // Texturas
    Texture plainTexture;
    Texture pastoTexture;
    Texture croquisTexture;

    // Modelos para las piramides
    Model ArenaCentral;
    Model PiramideChicken;
    Model PiramideChilly;
    Model PiramideRikoche;
    Model PiramideTotoro;

    // Lista de modelos para el Zoológico
    std::vector<Animal> Zoologico;

    // Modelos Principales
    Model ChickenLittle;
    Model Chilly_Willy;
    Model Rikoche;
    Model Totoro, Totoro_mediano, Totoro_chiquito;

    // Universo Chiquito Chicken
    Model ChickenCasa;
    std::vector<Model> EdificiosLittle;
    Model CalleChicken;
    Model CieloHexagonal;
    Model Hexagono;
    Model MariposaAzul, MariposaAzul_AlaDerecha, MariposaAzul_AlaIzquierda;
    std::vector<Model> Arboles;
    Model AbbyPatosa, Runt, PezOutWater, Kirby, BuckGallo, Melvin, Tina, Ace, RobotAlien, TheDog;
};