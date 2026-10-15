#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>

struct Vertice {
    float x;
    float y;
    float z;
};

// Carrega v, vt, vn e f do arquivo OBJ.
bool carregarObj(const std::string& caminho);

// Desenha o modelo em GL_TRIANGLES.
// A textura so e usada quando usarTextura for true.
void desenharModelo(bool usarTextura, unsigned int texturaId);

// Informacoes usadas pelo main para centralizar e enquadrar o modelo.
Vertice obterCentroModelo();
float obterEscalaBaseModelo();
bool modeloPossuiTextura();

#endif