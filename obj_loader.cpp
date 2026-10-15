#include "obj_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <GL/freeglut.h>

using namespace std;

// ============================================================
// ESTRUTURAS INTERNAS DO OBJ
// ============================================================

struct Normal {
    float x;
    float y;
    float z;
};

struct CoordenadaTextura {
    float u;
    float v;
};

struct IndiceFace {
    int vertice = -1;
    int textura = -1;
    int normal = -1;
};

struct Face {
    vector<IndiceFace> pontos;
};

// ============================================================
// DADOS DO MODELO
// ============================================================

static vector<Vertice> vertices;
static vector<Normal> normais;
static vector<CoordenadaTextura> texturas;
static vector<Face> faces;

static Vertice centroModelo = { 0.0f, 0.0f, 0.0f };
static float escalaBaseModelo = 1.0f;

// ============================================================
// FUNCOES AUXILIARES
// ============================================================

static int converterIndiceObj(const string& valor, int tamanhoAtual) {

    if (valor.empty())
        return -1;

    int indice = stoi(valor);

    // OBJ comeca os indices em 1.
    if (indice > 0)
        return indice - 1;

    // Indices negativos contam a partir do final da lista.
    if (indice < 0)
        return tamanhoAtual + indice;

    return -1;
}

static IndiceFace lerIndiceFace(const string& token) {

    IndiceFace indice;

    size_t primeiraBarra = token.find('/');

    // Formato: v
    if (primeiraBarra == string::npos) {
        indice.vertice = converterIndiceObj(token, (int)vertices.size());
        return indice;
    }

    indice.vertice = converterIndiceObj(
        token.substr(0, primeiraBarra),
        (int)vertices.size()
    );

    size_t segundaBarra = token.find('/', primeiraBarra + 1);

    // Formato: v/vt
    if (segundaBarra == string::npos) {
        indice.textura = converterIndiceObj(
            token.substr(primeiraBarra + 1),
            (int)texturas.size()
        );
        return indice;
    }

    // Formatos: v/vt/vn ou v//vn
    string valorTextura = token.substr(
        primeiraBarra + 1,
        segundaBarra - primeiraBarra - 1
    );

    string valorNormal = token.substr(segundaBarra + 1);

    indice.textura = converterIndiceObj(valorTextura, (int)texturas.size());
    indice.normal = converterIndiceObj(valorNormal, (int)normais.size());

    return indice;
}

static void calcularEnquadramentoModelo() {

    if (vertices.empty())
        return;

    float minX = vertices[0].x;
    float maxX = vertices[0].x;
    float minY = vertices[0].y;
    float maxY = vertices[0].y;
    float minZ = vertices[0].z;
    float maxZ = vertices[0].z;

    for (const Vertice& v : vertices) {
        minX = min(minX, v.x);
        maxX = max(maxX, v.x);
        minY = min(minY, v.y);
        maxY = max(maxY, v.y);
        minZ = min(minZ, v.z);
        maxZ = max(maxZ, v.z);
    }

    centroModelo.x = (minX + maxX) / 2.0f;
    centroModelo.y = (minY + maxY) / 2.0f;
    centroModelo.z = (minZ + maxZ) / 2.0f;

    float tamanhoX = maxX - minX;
    float tamanhoY = maxY - minY;
    float tamanhoZ = maxZ - minZ;
    float maiorDimensao = max(tamanhoX, max(tamanhoY, tamanhoZ));

    escalaBaseModelo = maiorDimensao > 0.0f
        ? 3.0f / maiorDimensao
        : 1.0f;
}

static Normal calcularNormalFace(
    const IndiceFace& a,
    const IndiceFace& b,
    const IndiceFace& c
) {

    Normal normal = { 0.0f, 0.0f, 1.0f };

    if (a.vertice < 0 || b.vertice < 0 || c.vertice < 0 ||
        a.vertice >= (int)vertices.size() ||
        b.vertice >= (int)vertices.size() ||
        c.vertice >= (int)vertices.size()) {
        return normal;
    }

    const Vertice& v0 = vertices[a.vertice];
    const Vertice& v1 = vertices[b.vertice];
    const Vertice& v2 = vertices[c.vertice];

    float ux = v1.x - v0.x;
    float uy = v1.y - v0.y;
    float uz = v1.z - v0.z;

    float vx = v2.x - v0.x;
    float vy = v2.y - v0.y;
    float vz = v2.z - v0.z;

    normal.x = uy * vz - uz * vy;
    normal.y = uz * vx - ux * vz;
    normal.z = ux * vy - uy * vx;

    float tamanho = sqrt(
        normal.x * normal.x +
        normal.y * normal.y +
        normal.z * normal.z
    );

    if (tamanho > 0.0f) {
        normal.x /= tamanho;
        normal.y /= tamanho;
        normal.z /= tamanho;
    }

    return normal;
}

static void enviarVertice(
    const IndiceFace& indice,
    const Normal& normalFace,
    bool usarTextura
) {

    if (indice.vertice < 0 || indice.vertice >= (int)vertices.size())
        return;

    // Usa a normal do OBJ quando existir.
    // Caso contrario usa a normal calculada para o triangulo.
    if (indice.normal >= 0 && indice.normal < (int)normais.size()) {
        const Normal& n = normais[indice.normal];
        glNormal3f(n.x, n.y, n.z);
    }
    else {
        glNormal3f(normalFace.x, normalFace.y, normalFace.z);
    }

    if (usarTextura) {
        if (indice.textura >= 0 && indice.textura < (int)texturas.size()) {
            const CoordenadaTextura& t = texturas[indice.textura];
            glTexCoord2f(t.u, t.v);
        }
        else {
            glTexCoord2f(0.0f, 0.0f);
        }
    }

    const Vertice& v = vertices[indice.vertice];
    glVertex3f(v.x, v.y, v.z);
}

// ============================================================
// LEITURA DO OBJ
// ============================================================

bool carregarObj(const string& caminho) {

    ifstream arquivo(caminho);

    if (!arquivo.is_open()) {
        cerr << "Erro: arquivo OBJ nao encontrado: " << caminho << endl;
        return false;
    }

    vertices.clear();
    normais.clear();
    texturas.clear();
    faces.clear();

    string linha;

    while (getline(arquivo, linha)) {

        if (linha.empty() || linha[0] == '#')
            continue;

        istringstream dados(linha);
        string tipo;
        dados >> tipo;

        if (tipo == "v") {
            Vertice v;
            dados >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (tipo == "vt") {
            CoordenadaTextura t;
            dados >> t.u >> t.v;
            texturas.push_back(t);
        }
        else if (tipo == "vn") {
            Normal n;
            dados >> n.x >> n.y >> n.z;
            normais.push_back(n);
        }
        else if (tipo == "f") {
            Face face;
            string token;

            while (dados >> token)
                face.pontos.push_back(lerIndiceFace(token));

            if (face.pontos.size() >= 3)
                faces.push_back(face);
        }
    }

    calcularEnquadramentoModelo();

    long long quantidadeTriangulos = 0;
    for (const Face& face : faces)
        quantidadeTriangulos += (long long)face.pontos.size() - 2;

    cout << "OBJ carregado: " << caminho << endl;
    cout << "Vertices: " << vertices.size() << endl;
    cout << "Coordenadas de textura: " << texturas.size() << endl;
    cout << "Normais: " << normais.size() << endl;
    cout << "Faces: " << faces.size() << endl;
    cout << "Triangulos apos triangulacao: " << quantidadeTriangulos << endl;

    return !vertices.empty() && !faces.empty();
}

// ============================================================
// DESENHO DO MODELO
// ============================================================

void desenharModelo(bool usarTextura, unsigned int texturaId) {

    if (vertices.empty() || faces.empty())
        return;

    usarTextura = usarTextura && !texturas.empty() && texturaId != 0;

    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaId);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    glBegin(GL_TRIANGLES);

    for (const Face& face : faces) {

        // Triangulacao em leque:
        // 0,1,2,3 -> (0,1,2) e (0,2,3)
        for (size_t i = 1; i + 1 < face.pontos.size(); i++) {

            const IndiceFace& a = face.pontos[0];
            const IndiceFace& b = face.pontos[i];
            const IndiceFace& c = face.pontos[i + 1];

            Normal normalFace = calcularNormalFace(a, b, c);

            enviarVertice(a, normalFace, usarTextura);
            enviarVertice(b, normalFace, usarTextura);
            enviarVertice(c, normalFace, usarTextura);
        }
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

Vertice obterCentroModelo() {
    return centroModelo;
}

float obterEscalaBaseModelo() {
    return escalaBaseModelo;
}

bool modeloPossuiTextura() {
    return !texturas.empty();
}