#ifndef TEXTURA_H
#define TEXTURA_H

#include <string>

// Carrega um BMP de 24 bits e cria uma textura OpenGL.
bool carregarTexturaBMP24(const std::string& caminho, unsigned int& texturaId);

#endif