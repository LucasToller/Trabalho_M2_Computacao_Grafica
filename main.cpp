#include <iostream>
#include <cstdlib>
#include <GL/freeglut.h>

#include "obj_loader.h"
#include "textura.h"

using namespace std;

// ============================================================
// TRANSFORMACOES
// ============================================================

float posX = 0.0f;
float posY = 0.0f;
float posZ = -6.0f;

float rotX = 0.0f;
float rotY = 0.0f;
float rotZ = 0.0f;

float escala = 1.0f;

// ============================================================
// MODELO, TEXTURA E LUZES
// ============================================================

Vertice centroModelo = { 0.0f, 0.0f, 0.0f };
float escalaBaseModelo = 1.0f;

unsigned int texturaModelo = 0;
bool texturaCarregada = false;

// A textura inicia desligada porque o .mtl original do mba1.obj
// nao foi fornecido. A tecla T demonstra o mapeamento com o BMP exemplo.
bool texturaLigada = false;

bool luz1Ligada = true;
bool luz2Ligada = true;
bool luz3Ligada = true;

// ============================================================
// PROTOTIPOS
// ============================================================

void inicializarOpenGL();
void configurarMaterial();
void configurarLuzes();
void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardSpecial(int key, int x, int y);
void resetarTransformacoes();

// ============================================================
// MAIN
// ============================================================

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 650);
    glutCreateWindow("Trabalho M2 - Computacao Grafica");

    inicializarOpenGL();

    if (!carregarObj("data/mba1.obj"))
        return 1;

    centroModelo = obterCentroModelo();
    escalaBaseModelo = obterEscalaBaseModelo();

    texturaCarregada = carregarTexturaBMP24(
        "Textures/launch.bmp",
        texturaModelo
    );

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);

    glutMainLoop();
    return 0;
}

// ============================================================
// OPENGL
// ============================================================

void inicializarOpenGL() {

    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    glShadeModel(GL_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

// ============================================================
// MATERIAL
// ============================================================

void configurarMaterial() {

    GLfloat ambiente[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    GLfloat difusa[] = { 0.85f, 0.85f, 0.85f, 1.0f };
    GLfloat especular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambiente);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, difusa);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
}

// ============================================================
// TRES PONTOS DE LUZ
// ============================================================

void configurarLuzes() {

    // Luz 1 - branca
    GLfloat ambiente1[] = { 0.10f, 0.10f, 0.10f, 1.0f };
    GLfloat difusa1[] = { 0.85f, 0.85f, 0.85f, 1.0f };
    GLfloat especular1[] = { 1.00f, 1.00f, 1.00f, 1.0f };
    GLfloat posicao1[] = { 4.0f, 4.0f, 4.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa1);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular1);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao1);

    luz1Ligada ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);

    // Luz 2 - tom quente
    GLfloat ambiente2[] = { 0.05f, 0.03f, 0.02f, 1.0f };
    GLfloat difusa2[] = { 0.70f, 0.35f, 0.20f, 1.0f };
    GLfloat especular2[] = { 0.80f, 0.50f, 0.35f, 1.0f };
    GLfloat posicao2[] = { -4.0f, 2.5f, 2.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambiente2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, difusa2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, especular2);
    glLightfv(GL_LIGHT1, GL_POSITION, posicao2);

    luz2Ligada ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1);

    // Luz 3 - tom frio
    GLfloat ambiente3[] = { 0.02f, 0.03f, 0.06f, 1.0f };
    GLfloat difusa3[] = { 0.20f, 0.40f, 0.75f, 1.0f };
    GLfloat especular3[] = { 0.35f, 0.55f, 1.00f, 1.0f };
    GLfloat posicao3[] = { 0.0f, -3.0f, 4.0f, 1.0f };

    glLightfv(GL_LIGHT2, GL_AMBIENT, ambiente3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, difusa3);
    glLightfv(GL_LIGHT2, GL_SPECULAR, especular3);
    glLightfv(GL_LIGHT2, GL_POSITION, posicao3);

    luz3Ligada ? glEnable(GL_LIGHT2) : glDisable(GL_LIGHT2);
}

// ============================================================
// DISPLAY
// ============================================================

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Luzes antes das transformacoes: permanecem fixas na cena.
    configurarLuzes();
    configurarMaterial();

    glPushMatrix();

    glTranslatef(posX, posY, posZ);

    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

    glScalef(
        escala * escalaBaseModelo,
        escala * escalaBaseModelo,
        escala * escalaBaseModelo
    );

    // Centraliza o modelo carregado em torno da origem.
    glTranslatef(
        -centroModelo.x,
        -centroModelo.y,
        -centroModelo.z
    );

    bool usarTextura = texturaLigada && texturaCarregada && modeloPossuiTextura();
    desenharModelo(usarTextura, texturaModelo);

    glPopMatrix();

    glutSwapBuffers();
}

// ============================================================
// PERSPECTIVA
// ============================================================

void reshape(int width, int height) {

    if (height == 0)
        height = 1;

    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// TECLADO
// ============================================================

void keyboard(unsigned char key, int x, int y) {

    switch (key) {

    case 27:
        exit(0);
        break;

    // Escala
    case '+':
    case '=':
        escala += 0.1f;
        break;

    case '-':
        if (escala > 0.2f)
            escala -= 0.1f;
        break;

    // Translação no eixo Z
    case 'q':
    case 'Q':
        posZ -= 0.2f;
        break;

    case 'e':
    case 'E':
        posZ += 0.2f;
        break;

    // Rotacao positiva e negativa
    case 'x': rotX += 5.0f; break;
    case 'X': rotX -= 5.0f; break;
    case 'y': rotY += 5.0f; break;
    case 'Y': rotY -= 5.0f; break;
    case 'z': rotZ += 5.0f; break;
    case 'Z': rotZ -= 5.0f; break;

    // Liga/desliga as luzes
    case '1':
        luz1Ligada = !luz1Ligada;
        cout << "Luz 1: " << (luz1Ligada ? "ligada" : "desligada") << endl;
        break;

    case '2':
        luz2Ligada = !luz2Ligada;
        cout << "Luz 2: " << (luz2Ligada ? "ligada" : "desligada") << endl;
        break;

    case '3':
        luz3Ligada = !luz3Ligada;
        cout << "Luz 3: " << (luz3Ligada ? "ligada" : "desligada") << endl;
        break;

    // A textura comeca desligada e pode ser demonstrada com T.
    case 't':
    case 'T':
        texturaLigada = !texturaLigada;
        cout << "Textura: " << (texturaLigada ? "ligada" : "desligada") << endl;
        break;

    case 'r':
    case 'R':
        resetarTransformacoes();
        break;
    }

    glutPostRedisplay();
}

void keyboardSpecial(int key, int x, int y) {

    switch (key) {
    case GLUT_KEY_UP:    posY += 0.2f; break;
    case GLUT_KEY_DOWN:  posY -= 0.2f; break;
    case GLUT_KEY_RIGHT: posX += 0.2f; break;
    case GLUT_KEY_LEFT:  posX -= 0.2f; break;
    }

    glutPostRedisplay();
}

void resetarTransformacoes() {
    posX = 0.0f;
    posY = 0.0f;
    posZ = -6.0f;
    rotX = 0.0f;
    rotY = 0.0f;
    rotZ = 0.0f;
    escala = 1.0f;
}