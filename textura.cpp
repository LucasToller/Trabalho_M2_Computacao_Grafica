#include "textura.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <GL/freeglut.h>

using namespace std;

struct BitMapFile {
    int sizeX = 0;
    int sizeY = 0;
    vector<unsigned char> data;
};

static bool lerBMP24(const string& caminho, BitMapFile& bmp) {

    ifstream arquivo(caminho, ios::binary);

    if (!arquivo.is_open()) {
        cerr << "Aviso: textura nao encontrada: " << caminho << endl;
        return false;
    }

    uint16_t assinatura = 0;
    arquivo.read(reinterpret_cast<char*>(&assinatura), sizeof(assinatura));

    if (assinatura != 0x4D42) {
        cerr << "Aviso: arquivo nao e BMP: " << caminho << endl;
        return false;
    }

    uint32_t offset = 0;
    arquivo.seekg(10);
    arquivo.read(reinterpret_cast<char*>(&offset), sizeof(offset));

    int32_t largura = 0;
    int32_t altura = 0;

    arquivo.seekg(18);
    arquivo.read(reinterpret_cast<char*>(&largura), sizeof(largura));
    arquivo.read(reinterpret_cast<char*>(&altura), sizeof(altura));

    uint16_t bitsPorPixel = 0;
    arquivo.seekg(28);
    arquivo.read(reinterpret_cast<char*>(&bitsPorPixel), sizeof(bitsPorPixel));

    uint32_t compressao = 0;
    arquivo.read(reinterpret_cast<char*>(&compressao), sizeof(compressao));

    if (largura <= 0 || altura == 0 || bitsPorPixel != 24 || compressao != 0) {
        cerr << "Aviso: a textura deve ser BMP 24 bits sem compressao: "
             << caminho << endl;
        return false;
    }

    int alturaAbsoluta = altura > 0 ? altura : -altura;
    int bytesLinha = ((largura * 3 + 3) / 4) * 4;

    bmp.sizeX = largura;
    bmp.sizeY = alturaAbsoluta;
    bmp.data.resize(largura * alturaAbsoluta * 3);

    vector<unsigned char> linha(bytesLinha);
    arquivo.seekg(offset);

    for (int y = 0; y < alturaAbsoluta; y++) {

        arquivo.read(reinterpret_cast<char*>(linha.data()), bytesLinha);
        if (!arquivo)
            return false;

        int destinoY = altura > 0 ? (alturaAbsoluta - 1 - y) : y;

        for (int x = 0; x < largura; x++) {
            int origem = x * 3;
            int destino = (destinoY * largura + x) * 3;

            // BMP usa BGR. OpenGL recebe RGB.
            bmp.data[destino + 0] = linha[origem + 2];
            bmp.data[destino + 1] = linha[origem + 1];
            bmp.data[destino + 2] = linha[origem + 0];
        }
    }

    return true;
}

bool carregarTexturaBMP24(const string& caminho, unsigned int& texturaId) {

    BitMapFile bmp;

    if (!lerBMP24(caminho, bmp))
        return false;

    glGenTextures(1, &texturaId);
    glBindTexture(GL_TEXTURE_2D, texturaId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        bmp.sizeX,
        bmp.sizeY,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        bmp.data.data()
    );

    cout << "Textura carregada: " << caminho
         << " (" << bmp.sizeX << "x" << bmp.sizeY << ")" << endl;

    return true;
}