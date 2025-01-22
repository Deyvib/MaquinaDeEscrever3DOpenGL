#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>
#include <vector>
#include <cmath>

GLint objectColorLoc;

const float light0_position[4] = {1, 1, 1, 0};

GLuint prog_hdlr;
GLint location_attribute_0, location_viewport;

float angX = -45.0, angY = 0.0; // Ângulos de rotação para a camera
GLUquadricObj *planet;
bool aperto, teclando, vermelho;

// Estrutura para representar uma tecla
struct Tecla
{
    char letra;
    double x, y, z;   // Coordenadas
    bool pressionada; // Indica se a tecla está pressionada
    double r, g, b;
};

struct Carimbo
{
    float angulo;
    float x, y, z;    // Posição 3D do carimbo
    bool pressionado; // Se a tecla associada foi pressionada
};

// Vetor dinâmico para armazenar letras da folha
std::vector<Tecla> textoNaFolha;
// Vetor para armazenar carimbos
std::vector<Carimbo> carimbos(36);

Tecla teclas[40]; // Array para armazenar as teclas do teclado

const char *teclado[] = {"1234567890", "QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"}; // Layout do teclado

float deslocamentoX = 0.0; // Variável global para controlar o deslocamento da parte móvel da folha
float alturaFolha = 1.0f;  // Controla a altura (enrolamento)

void InicializaCarimbos()
{
    float raio = 0.5f; // Controla o tamanho da parábola
    float a = 0.3f;    // Controla a curvatura da parábola
    float b = 0.7f;    // Ajusta a altura dos carimbos

    for (int i = 0; i < 36; i++)
    {
        // Distribui os carimbos ao longo de uma linha
        float x = (i - 17.5f) / 17.5f;
        float y = a * x * x + b;

        // Posiciona os carimbos em um arco parabólico
        carimbos[i].x = y;
        carimbos[i].y = 0.0;
        carimbos[i].z = raio * x;
        carimbos[i].pressionado = false; // Inicia não pressionado
    }
}

// Função para desenhar uma tecla
void DesenhaTecla(float x, float y, float z, bool pressionada)
{
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
    glTranslatef(x, y, z + (pressionada ? -0.05 : 0.0)); // Ajusta a posição se pressionada
    gluCylinder(planet, 0.1, 0.1, 0.05, 100, 100);
    glTranslatef(0.0, 0.0, 0.05);
    gluDisk(planet, 0.0, 0.1, 100, 40); // Disco superior
    glPopMatrix();
    // Haste da tecla
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.34f, 0.34f, 0.34f);
    glTranslatef(x, y, z - 0.55 + (pressionada ? -0.05 : 0.0));
    gluCylinder(planet, 0.02, 0.02, 0.6, 100, 40);
    glPopMatrix();
}

// Função para desenhar texto em 3D
void DesenharTexto3D(float x, float y, float z, const char *texto, bool naFolha, bool pressionada, double r, double g, double b)
{
    glLineWidth(1);
    glPushMatrix();
    glTranslatef(x + 0.01, y - 0.14, z); // Ajusta a posição das letras

    // Se for na folha, rotaciona para 75° para ficar em pé, rente com a folha
    if (naFolha)
    {
        glRotatef(75, 1.0, 0.0, 0.0);
        glUniform3f(objectColorLoc, r, g, b);
        glTranslatef(0.0f, 0.1f, -0.33f);
        glScalef(0.0008f, 0.0008f, 0.0008f); // Modifica o tamanho das letras da folha
    }
    else
    {
        glTranslatef(0.0f, 0.0f, 0.0f + (pressionada ? -0.05f : 0.0f)); // Ajusta a altura das letras do teclado se pressionada
        glUniform3f(objectColorLoc, 1.9, 1.9, 1.9);
        glScalef(0.001f, 0.001f, 0.001f);
        glLineWidth(2.0);
    }

    for (int i = 0; texto[i] != '\0'; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, texto[i]);
    }

    glPopMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
}

// Função para desenhar texto na folha
void DesenharTextoNaFolha()
{
    for (const auto &letra : textoNaFolha)
    {
        if (letra.z > 0.74)
        { // Condição para desenhar a letra, se baixo demais, ignore
            DesenharTexto3D(letra.x, letra.y, letra.z, std::string(1, letra.letra).c_str(), true, false, letra.r, letra.g, letra.b);
        }
    }
}

void DesenharFolha()
{
    glPushMatrix();
    glTranslatef(0.0f, 1.5, 0.75);
    glRotatef(70.0f, 1.0f, 0.0f, 0.0f); // Inclinação da folha
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-1.5f, 0.0f, 0.0f);
    glVertex3f(1.5f, 0.0f, 0.0f);
    glVertex3f(1.5f, alturaFolha * 1.06, 0.0f); // A folha tem seu tamanho do topo modificado para simular o enrolamento
    glVertex3f(-1.5f, alturaFolha * 1.06, 0.0f);
    glEnd();
    glPopMatrix();
}

void Mover()
{
    glPushMatrix();
    glTranslatef(1.29, 0.3, 0.2); // Posição inicial

    // Cilindro onde fica a folha enrolada (base da máquina)
    glPushMatrix();
    glTranslatef((-deslocamentoX), 0, 0); // Mover a base móvel da máquina de escrever
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
    glTranslatef(-1.8, 1.9, 0.9);
    glScalef(2.0, 3.0, 2.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    gluCylinder(planet, 0.1, 0.1, 1.8, 100, 40);
    glPopMatrix();

    DesenharFolha();

    // Cilindro de metal na frente da folha
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.5, 0.5, 0.5);
    glTranslatef(-1.8, 1.5, 1.3);
    glScalef(2.0, 3.0, 2.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    gluCylinder(planet, 0.02, 0.02, 1.8, 100, 40);
    glPopMatrix();

    // Sub-cilindro para simular a folha enrolada
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.99, 0.9, 0.9);
    glTranslatef(-1.5, 1.9, 0.9);
    glScalef(2.0, 3.0, 2.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    gluCylinder(planet, 0.11, 0.11, 1.5, 100, 40);
    glPopMatrix();

    // Cilindro da rolagem da folha
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.55, 0.55, 0.55);
    glTranslatef(1.8, 1.9, 0.9);
    glScalef(2.0, 3.0, 2.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    gluCylinder(planet, 0.04, 0.04, 0.1, 100, 40);
    glPopMatrix();

    // Engrenagem estrela para enrolar/desenrolar folha
    glPushMatrix();
    glTranslatef(2.0, 1.9, 0.9);
    glRotatef((-(alturaFolha * 90)), 1, 0, 0);
    glUniform3f(objectColorLoc, 0.15, 0.15, 0.15);
    glScalef(0.13, 0.4, 0.9);
    glutSolidSphere(0.3, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(2.0, 1.9, 0.9);
    glRotatef((-(90 + alturaFolha * 90)), 1, 0, 0);
    glUniform3f(objectColorLoc, 0.15, 0.15, 0.15);
    glScalef(0.13, 0.4, 0.9);
    glutSolidSphere(0.3, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(2.0, 1.9, 0.9);
    glRotatef((-(45 + alturaFolha * 90)), 1, 0, 0);
    glUniform3f(objectColorLoc, 0.15, 0.15, 0.15);
    glScalef(0.13, 0.4, 0.9);
    glutSolidSphere(0.3, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(2.0, 1.9, 0.9);
    glRotatef((-(135 + alturaFolha * 90)), 1, 0, 0);
    glUniform3f(objectColorLoc, 0.15, 0.15, 0.15);
    glScalef(0.13, 0.4, 0.9);
    glutSolidSphere(0.3, 20, 20);
    glPopMatrix();

    // Canto esquerdo do suporte do cilindro (roxo)
    glPushMatrix();
    glRotatef(-35, 1, 0, 0);
    glTranslatef(-1.8, 0.9, 1.9);
    glUniform3f(objectColorLoc, 0.3, 0.3, 0.7);
    glScalef(0.2, 1.8, 0.8);
    glutSolidSphere(0.42, 30, 30);
    glPopMatrix();

    // Canto direito do suporte do cilindro (roxo)
    glPushMatrix();
    glRotatef(-35, 1, 0, 0);
    glTranslatef(1.8, 0.9, 1.9);
    glUniform3f(objectColorLoc, 0.3, 0.3, 0.7);
    glScalef(0.2, 1.8, 0.8);
    glutSolidSphere(0.42, 30, 30);
    glPopMatrix();

    // Parte de trás (roxa)
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.3, 0.3, 0.7);
    glTranslatef(0.0, 2.46, 0.9);
    glScalef(3.5, 0.3, 0.7);
    glutSolidCube(1);
    glPopMatrix();

    // Parte de baixo (roxa)
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.3, 0.3, 0.7);
    glTranslatef(0.0, 2.11, 0.5);
    glScalef(3.5, 1.0, 0.1);
    glutSolidCube(1);
    glPopMatrix();

    // Cilindro da alavanca
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.55, 0.55, 0.55);
    glTranslatef(-2.0, 1.9, 0.9);
    glScalef(2.0, 3.0, 2.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    gluCylinder(planet, 0.04, 0.04, 0.1, 100, 40);
    glPopMatrix();

    // Esfera conectora do cilindro
    glPushMatrix();
    glTranslatef(-2.04, 1.9, 0.9);
    glUniform3f(objectColorLoc, 0.55, 0.55, 0.55);
    glScalef(0.4, 1.3, 1.0);
    glutSolidSphere(0.2, 30, 30);
    glPopMatrix();

    // Alavanca
    glPushMatrix();
    glRotatef((aperto ? 0 : -10), 0.0f, 0.0f, 1.0f);
    glTranslatef((aperto ? 0.0f : -0.35f), (aperto ? 0.0f : -0.35f), 0.0f);
    // Parte longa
    glPushMatrix();
    glRotatef(-30, 1.0, 0.0, 0.0);
    glTranslatef(-2.04, 0.6, 1.75);
    glUniform3f(objectColorLoc, 0.55, 0.55, 0.55);
    glScalef(0.05, 1.3, 0.2);
    glutSolidCube(1);
    glPopMatrix();

    // Parte Curta
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.55, 0.55, 0.55);
    glTranslatef(-2.04, 0.7, 1.54);
    glScalef(0.15, 1.1, 0.4);
    glutSolidSphere(0.3, 30, 30);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    glPushMatrix();

    DesenharTextoNaFolha();

    glPopMatrix();
    glPopMatrix();
}

void DesenhaFita()
{
    glPushMatrix();

    // Coordenadas dos carretéis
    float x1 = -1.0f, z1 = 0.1f; // Carretel esquerdo
    float x2 = 1.0f, z2 = 0.1f;  // Carretel direito
    float yBase = 0.9f;          // Altura base dos carretéis
    float yPeak = 3.4f;          // Altura máxima no centro
    float largura = 0.1f;        // Largura da fita

    glUniform3f(objectColorLoc, 0.1f, 0.1f, 0.1f); // Cor preta
    if (vermelho)
    {

        glTranslatef(0.0, 1.29, (teclando ? -0.25 : -0.45)); // Ajusta a posição inicial
    }
    else
    {
        glTranslatef(0.0, 1.29, (teclando ? -0.35 : -0.5)); // Ajusta a posição inicial
    }
    glRotatef(85, 1, 0, 0); // Inclina a fita para visualização

    glBegin(GL_TRIANGLE_STRIP);
    for (float t = -0.1f; t <= 1.1f; t += 0.04f)
    {
        float x = (1 - t) * x1 + t * x2;
        float z = (1 - t) * z1 + t * z2;

        // Elevação parabólica para o Y
        float y = (1 - t) * yBase + t * yBase + t * (1 - t) * (yPeak - yBase);

        // Ajustes nos extremos para conectar aos carretéis
        if (t <= 0.15f) // Extremo esquerdo
        {
            z += (0.1f - t) * 1.4f; // Projeta o extremo para trás
            x -= (0.1f - t) * 0.2f; // Move para a direção do carretel esquerdo
        }
        else if (t >= 0.85f) // Extremo direito
        {
            z += (t - 0.9f) * 1.4f; // Projeta o extremo para trás
            x += (t - 0.9f) * 0.2f; // Move para a direção do carretel direito
        }

        // Curvatura adicional no eixo Z para empurrar a fita "para fora"
        float curvaturaZ = sin(t * 3.14159) * 0.4f;
        z -= curvaturaZ;

        float curvaturaX = cos(t * 3.14159) * 0.1f; // Curvatura lateral
        x += curvaturaX;

        // Vetores para largura da fita
        float dy = largura / 2.0f;

        glVertex3f(x, y - dy, z);
        glVertex3f(x, y + dy, z);
    }
    glEnd();

    glPopMatrix();
}

void DesenhaFita2()
{
    glPushMatrix();

    // Coordenadas dos carretéis
    float x1 = -1.0f, z1 = 0.1f; // Carretel esquerdo
    float x2 = 1.0f, z2 = 0.1f;  // Carretel direito
    float yBase = 0.9f;          // Altura base dos carretéis
    float yPeak = 3.4f;          // Altura máxima no centro
    float largura = 0.1f;        // Largura da fita

    glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f); // Cor da fita
    if (vermelho)
    {
        glTranslatef(0.0, 1.29, (teclando ? -0.35 : -0.55)); // Ajusta a posição inicial
    }
    else
    {
        glTranslatef(0.0, 1.29, (teclando ? -0.45 : -0.6)); // Ajusta a posição inicial
    }
    glRotatef(85, 1, 0, 0); // Inclina a fita para visualização

    glBegin(GL_TRIANGLE_STRIP);
    for (float t = -0.1f; t <= 1.1f; t += 0.04f)
    {
        float x = (1 - t) * x1 + t * x2;
        float z = (1 - t) * z1 + t * z2;

        // Elevação parabólica para o Y
        float y = (1 - t) * yBase + t * yBase + t * (1 - t) * (yPeak - yBase);

        // Ajustes nos extremos para conectar aos carretéis
        if (t <= 0.15f) // Extremo esquerdo
        {
            z += (0.1f - t) * 1.4f; // Projeta o extremo para trás
            x -= (0.1f - t) * 0.2f; // Move para a direção do carretel esquerdo
        }
        else if (t >= 0.85f) // Extremo direito
        {
            z += (t - 0.9f) * 1.4f; // Projeta o extremo para trás
            x += (t - 0.9f) * 0.2f; // Move para a direção do carretel direito
        }

        // Curvatura adicional no eixo Z para empurrar a fita "para fora"
        float curvaturaZ = sin(t * 3.14159) * 0.4f;
        z -= curvaturaZ;

        float curvaturaX = cos(t * 3.14159) * 0.1f; // Curvatura lateral
        x += curvaturaX;

        // Vetores para largura da fita
        float dy = largura / 2.0f;

        glVertex3f(x, y - dy, z);
        glVertex3f(x, y + dy, z);
    }
    glEnd();

    glPopMatrix();
}

// Função principal
void Desenha(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glUseProgram(prog_hdlr);
    GLint objectColorLoc = glGetUniformLocation(prog_hdlr, "objectColor");

    glTranslatef(0.0f, 0.0f, -25.0f); // Zoom geral
    glRotatef(angX, 1.0, 0.0, 0.0);
    glRotatef(angY, 0.0, 1.0, 0.0);

    // Base azulada
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.7, 0.8, 0.7);
    glTranslatef(0.0, 0.5, -0.3);
    glScalef(3.7, 5.0, 0.5);
    glutSolidCube(1);

    // Canto esquerdo azulado
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glTranslatef(-0.465, 0.2, 1.2);
    glScalef(0.07, 0.6, 1.4);
    glutSolidCube(1);
    glPopMatrix();

    // Canto direito azulado
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glTranslatef(0.465, 0.2, 1.2);
    glScalef(0.07, 0.6, 1.4);
    glutSolidCube(1);
    glPopMatrix();

    // Parte de baixo azulada
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glTranslatef(0.0, -0.46, 0.7);
    glScalef(0.999, 0.09, 0.4);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();

    // Parte oval azul de baixo ondulada
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    glTranslatef(-2.0, 0.0, 0.0);
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glScalef(0.5, 2.5, 0.5);
    glutSolidSphere(0.42, 30, 30);
    glPopMatrix();

    // Parte oval azul da esquerda ondulada
    glPushMatrix();
    glRotatef(20, 1, 0, 0);
    glTranslatef(-1.7, -0.6, 0.5);
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glScalef(0.5, 2.3, 0.5);
    glutSolidSphere(0.42, 30, 30);
    glPopMatrix();

    // Parte oval azul da direita ondulada
    glPushMatrix();
    glRotatef(20, 1, 0, 0);
    glTranslatef(1.7, -0.6, 0.5);
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glScalef(0.5, 2.3, 0.5);
    glutSolidSphere(0.42, 30, 30);
    glPopMatrix();

    // Parte da divida das teclas com os carreteis em azul
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glTranslatef(0.0, 0.2, 0.2);
    glScalef(3.65, 0.4, 0.9);
    glutSolidCube(1);
    glPopMatrix();

    // Desenhando as teclas
    for (int i = 0; i < 37; i++)
    {
        if (teclas[i].letra == ' ') // Desenhando especificamente a tecla de espaço
        {
            glPushMatrix();
            glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
            glTranslatef(teclas[i].x, teclas[i].y, teclas[i].z + (teclas[i].pressionada ? -0.05 : 0.0));
            glPushMatrix();
            glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
            glScalef(1.9, 0.2, 0.05);
            glutSolidCube(1.0);
            glPopMatrix();
            // Haste da tecla (cilindro de baixo)
            glPushMatrix();
            glUniform3f(objectColorLoc, 0.34f, 0.34f, 0.34f);
            glTranslatef(teclas[i].x, 0.0, teclas[i].z - 0.45 + (teclas[i].pressionada ? -0.05 : 0.0));
            gluCylinder(planet, 0.02, 0.02, 0.3, 100, 40);
            glPopMatrix();
            glPopMatrix();
        }
        else
        {
            DesenhaTecla(teclas[i].x, teclas[i].y, teclas[i].z, teclas[i].pressionada); // Teclas normais do teclado
            DesenharTexto3D(teclas[i].x - 0.05,
                            teclas[i].y + 0.08,
                            teclas[i].z + 0.055,
                            std::string(1, teclas[i].letra).c_str(),
                            false,
                            teclas[i].pressionada,
                            teclas[i].r,
                            teclas[i].g,
                            teclas[i].b); // Texto usado no teclado
        }
    }

    // Triangulo do meio
    glPushMatrix();
    glLineWidth(2.0);
    glRotatef(75, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 1.55, -1.45);
    glUniform3f(objectColorLoc, 0.5, 0.5, 0.5);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.15, -0.15); // Vértice 1
    glVertex2f(0.15, -0.15);  // Vértice 2
    glVertex2f(0.0, 0.15);    // Vértice 3
    glEnd();
    glPopMatrix();

    // Barra de metal do lado do triangulo
    glPushMatrix();
    glRotatef(-5, 1.0, 0.0, 0.0);
    glUniform3f(objectColorLoc, 0.4, 0.4, 0.4);
    glTranslatef(0.0, 1.69, 0.97);
    glScalef(3.7, 0.05, 0.4);
    glutSolidCube(1);
    glPopMatrix();

    // Retangulo do fundo azul
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glTranslatef(0.0, 2.8, 0.3);
    glScalef(3.7, 0.4, 0.7);
    glutSolidCube(1);
    glPopMatrix();

    // Retangulo de preenchimento azul de dentro e topo
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.2, 0.3, 0.7);
    glTranslatef(0.0, 2.2, 0.3);
    glScalef(3.2, 1.06, 0.7);
    glutSolidCube(1);
    glPopMatrix();

    // Haste de rolagem
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glUniform3f(objectColorLoc, 0.6, 0.6, 0.6);
    glTranslatef(-0.65, 2.4, -1.855);
    glScalef(1.1, 1.1, 0.7);
    gluCylinder(planet, 0.05, 0.05, 5.3, 100, 40);
    glPopMatrix();

    // Carretel 1 Esquerdo
    glPushMatrix();
    glTranslatef(-1.0, 0.9, 0.6);
    glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
    glScalef(1.8, 1.8, 0.05);
    glutSolidSphere(0.21, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.0, 0.9, 0.15);
    glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
    glScalef(1.8, 1.8, 0.2);
    glutSolidSphere(0.21, 30, 30);
    glPopMatrix();
    // Cilindro preto
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.1f, 0.1f, 0.1f);
    glTranslatef(-1.0, 0.9, 0.35);
    gluCylinder(planet, 0.2, 0.2, 0.25, 100, 40);
    glPopMatrix();
    // Cilindro vermelho
    glPushMatrix();
    glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
    glTranslatef(-1.0, 0.9, 0.1);
    gluCylinder(planet, 0.2, 0.2, 0.25, 100, 40);
    glPopMatrix();

    // Carretel 2 Direito
    glPushMatrix();
    glRotatef(0, 1, 0, 0);
    glTranslatef(1.0, 0.9, 0.6);
    glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
    glScalef(1.8, 1.8, 0.05);
    glutSolidSphere(0.21, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glRotatef(0, 1, 0, 0);
    glTranslatef(1.0, 0.9, 0.15);
    glUniform3f(objectColorLoc, 0.2, 0.2, 0.2);
    glScalef(1.8, 1.8, 0.05);
    glutSolidSphere(0.21, 30, 30);
    glPopMatrix();
    // Cilindro preto
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.1f, 0.1f, 0.1f);
    glTranslatef(1.0, 0.9, 0.35);
    gluCylinder(planet, 0.2, 0.2, 0.25, 100, 40);
    glPopMatrix();
    // Cilindro vermelho
    glPushMatrix();
    glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
    glTranslatef(1.0, 0.9, 0.1);
    gluCylinder(planet, 0.2, 0.2, 0.25, 100, 40);
    glPopMatrix();

    // Cilindro subindo
    glPushMatrix();
    if (vermelho)
    {

        glTranslatef(0.0, 0.0, (teclando ? 0.1 : -0.12));
    }
    else
    {
        glTranslatef(0.0, 0.0, (teclando ? 0.0 : -0.16));
    }
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4f, 0.4f, 0.4f);
    glTranslatef(0.0, 1.7, 0.0);
    gluCylinder(planet, 0.02, 0.02, 1.0, 100, 40);
    glPopMatrix();

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4, 0.4, 0.4);
    glTranslatef(0.0, 1.7, 1.0);
    glScalef(0.2, 0.06, 0.01);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4, 0.4, 0.4);
    glTranslatef(0.1, 1.7, 1.0);
    glScalef(0.02, 0.06, 0.07);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4, 0.4, 0.4);
    glTranslatef(-0.1, 1.7, 1.0);
    glScalef(0.02, 0.06, 0.07);
    glutSolidCube(1);
    glPopMatrix();

    glPopMatrix();

    // Cilindro 1 estático esquerdo
    glPushMatrix();
    glTranslatef(-0.6, -0.18, -0.3);
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4f, 0.4f, 0.4f);
    glTranslatef(0.0, 1.7, 0.0);
    gluCylinder(planet, 0.02, 0.02, 1.0, 100, 40);
    glPopMatrix();

    // Topo
    glPushMatrix();

    glRotatef(90, 0, 0, 1);
    glTranslatef(1.69, -1.7, 0.0);

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4, 0.4, 0.4);
    glTranslatef(0.0, 1.7, 1.0);
    glScalef(0.07, 0.06, 0.01);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4, 0.4, 0.4);
    glTranslatef(0.03, 1.7, 1.0);
    glScalef(0.02, 0.06, 0.07);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.4, 0.4, 0.4);
    glTranslatef(-0.03, 1.7, 1.0);
    glScalef(0.02, 0.06, 0.07);
    glutSolidCube(1);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    // Cilindro 2 estático direito
    glPushMatrix();
    glTranslatef(0.6, -0.18, -0.3);
    glPushMatrix();
    glUniform3f(objectColorLoc, 0.5f, 0.5f, 0.5f);
    glTranslatef(0.0, 1.7, 0.0);
    gluCylinder(planet, 0.02, 0.02, 1.0, 100, 40);
    glPopMatrix();

    // Topo
    glPushMatrix();

    glRotatef(90, 0, 0, 1);
    glTranslatef(1.69, -1.7, 0.0);

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.5, 0.5, 0.5);
    glTranslatef(0.0, 1.7, 1.0);
    glScalef(0.07, 0.06, 0.01);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.5, 0.5, 0.5);
    glTranslatef(0.03, 1.7, 1.0);
    glScalef(0.02, 0.06, 0.07);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glUniform3f(objectColorLoc, 0.5, 0.5, 0.5);
    glTranslatef(-0.03, 1.7, 1.0);
    glScalef(0.02, 0.06, 0.07);
    glutSolidCube(1);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    DesenhaFita();
    DesenhaFita2();

    // Desenhando os carimbos
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glRotatef(90, 0, 0, 1);
    glTranslatef(0.2f, 0.5f, 0.0f);

    for (int i = 0; i < 36; i++)
    {
        // Ponto central
        float centroX = 0.0f;
        float centroY = 0.0f;
        float centroZ = 0.0f;

        // Vetor do carimbo para o centro
        float dx = centroX - carimbos[i].x;
        float dy = centroY - carimbos[i].y;
        float dz = centroZ - carimbos[i].z;

        float anguloY = 0.0;
        float distanciaHorizontal = 0.0;
        float anguloX = 0.0;

        // Calcula os ângulos de rotação
        if ((i >= 0 && i <= 4 && teclas[i].pressionada) || (i >= 32 && i < 36 && teclas[i].pressionada)) // Teclas dos cantos tem rotações mais significativas
        {
            anguloY = atan2(dz, dx) * 150.0f / M_PI; // Rotação no eixo Y
            distanciaHorizontal = sqrt(dx * dx + dz * dz);
            anguloX = atan2(dy, distanciaHorizontal) * 160.0f / M_PI; // Rotação no eixo X
        }
        else if (((i >= 5 && i <= 9) || i == 31 || i == 30 || i == 29) && teclas[i].pressionada)
        {
            anguloY = atan2(dz, dx) * 170.0f / M_PI;
            distanciaHorizontal = sqrt(dx * dx + dz * dz);
            anguloX = atan2(dy, distanciaHorizontal) * 160.0f / M_PI;
        }
        else
        {
            anguloY = atan2(dz, dx) * 180.0f / M_PI;
            distanciaHorizontal = sqrt(dx * dx + dz * dz);
            anguloX = atan2(dy, distanciaHorizontal) * 180.0f / M_PI;
        }

        // Calcula o fator parabólico
        float fatorParabolico = 3.2f - (fabs(i - 17.5f) / 17.5f); // 1 no meio, 0 nos extremos
        bool isExtremo = (i >= 0 && i < 8) || (i >= 29 && i <= 36);
        float escalaAproximacao = teclas[i].pressionada ? 0.2f * (isExtremo ? 1.5f : fatorParabolico) : 0.0f;

        float ajusteX = dx * escalaAproximacao;
        float ajusteY = dy * escalaAproximacao;
        float ajusteZ = dz * escalaAproximacao;

        if (teclas[i].pressionada)
        {
            ajusteY -= 0.2f; // Incremento a altura ao pressionar para ajustar
        }

        float inclinacao = teclas[i].pressionada ? -40.0f : 40.0f; // Inclinação do carimbo

        // Desenha o carimbo
        glPushMatrix();
        glTranslatef(carimbos[i].x - ajusteX, carimbos[i].y - ajusteY, carimbos[i].z + ajusteZ);
        glRotatef(anguloY, 0.0f, 1.0f, 0.0f);
        glRotatef(anguloX, 1.0f, 0.0f, 0.0f);
        glRotatef(-inclinacao, 0.0f, 0.0f, 1.0f);

        glUniform3f(objectColorLoc, 0.45f, 0.45f, 0.45f);
        glPushMatrix(); // Desenha a haste do carimbo
        glScalef(0.1f, 9.0f, 0.1f);
        glutSolidCube(0.1f);
        glPopMatrix();

        // Desenha a cabeça do carimbo
        glPushMatrix();
        glRotatef(90, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.5f, 0.0f);
        glScalef(0.2f, 0.6f, 0.1f);
        glutSolidSphere(0.2f, 10, 10);
        glPopMatrix();
        glPopMatrix();
    }

    glPopMatrix();

    Mover(); // Parte movel ao digitar

    glFlush();
    glutSwapBuffers();
}

// Função para tratar eventos de teclado
void Teclado(unsigned char key, int x, int y)
{
    if (deslocamentoX > 2.6) // Condição caso a folha tenha chegado no final
    {
        return;
    }
    aperto = false;
    teclando = false;
    for (int i = 0; i < 37; i++)
    {
        if (teclas[i].letra == key || tolower(teclas[i].letra) == key)
        {
            if (key != ' ')
            {
                teclando = true;
            }
            teclas[i].pressionada = true;

            textoNaFolha.push_back({static_cast<char>(key), -1.3, 1.33, 0.9, true,
                                    (vermelho ? 1.0 : 0.0),
                                    (vermelho ? 0.0 : 0.0),
                                    (vermelho ? 0.0 : 0.0)});

            if (key == 'm' || key == 'M') // Ajusta o deslocamento para algumas letras
            {
                deslocamentoX += 0.1;
                for (auto &letra : textoNaFolha)
                {
                    letra.x -= 0.1f;
                }
            }
            else if (key == 'l' || key == 'L' || key == 'i' || key == 'I')
            {
                deslocamentoX += 0.04;
                for (auto &letra : textoNaFolha)
                {
                    letra.x -= 0.04f;
                }
            }
            else
            {
                deslocamentoX += 0.07;
                for (auto &letra : textoNaFolha)
                {
                    letra.x -= 0.07f;
                }
            }
            glutPostRedisplay();
            return;
        }
    }
}

void TecladoSolto(unsigned char key, int x, int y) // Função para manipular o pressionamento das teclas
{
    teclando = false;
    for (int i = 0; i < 37; i++)
    {
        if (teclas[i].letra == key || tolower(teclas[i].letra) == key)
        {
            teclas[i].pressionada = false;
            glutPostRedisplay();
            return;
        }
    }
}

void AtualizarPosicaoFolha(float incrementoAltura) // Função para calcular o movimento das letras na folha em relação a inclinação
{
    const float inclinacao = 160.0f * M_PI / 180.0f;

    float incrementoY = incrementoAltura * sin(inclinacao);
    float incrementoZ = -incrementoAltura * cos(inclinacao);

    alturaFolha += incrementoZ;

    // Ajusta a posição das letras
    for (auto &letra : textoNaFolha)
    {
        letra.y += incrementoY;
        letra.z += incrementoZ;
    }

    glutPostRedisplay();
}

void TeclasEspeciaisUp(int key, int x, int y)
{
    if (key == GLUT_KEY_F1 && deslocamentoX > 0.0)
    {
        aperto = false;
        glutPostRedisplay();
    }
}

// Função para tratar teclas especiais
void TeclasEspeciais(int key, int x, int y)
{

    if (key == GLUT_KEY_F4)
    {
        if (vermelho)
        {
            vermelho = false;
        }
        else
        {
            vermelho = true;
        }
    }

    if (key == GLUT_KEY_F1 && deslocamentoX > 0.0)
    {
        aperto = true;
        deslocamentoX -= 0.05f; // Movimento de volta
        for (auto &letra : textoNaFolha)
        {
            letra.x += 0.05f;
        }
        glutPostRedisplay();
        return;
    }

    if (key == GLUT_KEY_F2 && alturaFolha < 3.0)
    { // Enrolar a folha
        AtualizarPosicaoFolha(0.05f);
        return;
    }
    if (key == GLUT_KEY_F3 && alturaFolha > 0.3)
    { // Desenrolar a folha
        AtualizarPosicaoFolha(-0.05f);
        return;
    }

    switch (key) // Ajusta a câmera
    {
    case GLUT_KEY_LEFT:
        angY -= 5.0;
        break;
    case GLUT_KEY_RIGHT:
        angY += 5.0;
        break;
    case GLUT_KEY_UP:
        angX -= 5.0;
        break;
    case GLUT_KEY_DOWN:
        angX += 5.0;
        break;
    }
    glutPostRedisplay();
}

void printInfoLog(GLuint obj)
{
    int log_size = 0;
    int bytes_written = 0;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &log_size);
    if (!log_size)
        return;
    char *infoLog = new char[log_size];
    glGetProgramInfoLog(obj, log_size, &bytes_written, infoLog);
    std::cerr << infoLog << std::endl;
    delete[] infoLog;
}

bool read_n_compile_shader(const char *filename, GLuint &hdlr, GLenum shaderType)
{
    std::ifstream is(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!is.is_open())
    {
        std::cerr << "Unable to open file " << filename << std::endl;
        return false;
    }
    long size = is.tellg();
    char *buffer = new char[size + 1];
    is.seekg(0, std::ios::beg);
    is.read(buffer, size);
    is.close();
    buffer[size] = 0;

    hdlr = glCreateShader(shaderType);
    glShaderSource(hdlr, 1, (const GLchar **)&buffer, NULL);
    glCompileShader(hdlr);
    std::cerr << "info log for " << filename << std::endl;
    printInfoLog(hdlr);
    delete[] buffer;
    return true;
}

void setShaders(GLuint &prog_hdlr, const char *vsfile, const char *fsfile)
{
    GLuint vert_hdlr, frag_hdlr;
    read_n_compile_shader(vsfile, vert_hdlr, GL_VERTEX_SHADER);
    read_n_compile_shader(fsfile, frag_hdlr, GL_FRAGMENT_SHADER);

    prog_hdlr = glCreateProgram();
    glAttachShader(prog_hdlr, frag_hdlr);
    glAttachShader(prog_hdlr, vert_hdlr);

    glLinkProgram(prog_hdlr);
    std::cerr << "info log for the linked program" << std::endl;
    printInfoLog(prog_hdlr);
}

// Inicializa vetor de teclas com os dados da lista de teclado
void InicializaTeclas()
{
    double offsetX = -1.35, offsetY = -0.16;

    int index = 0;

    for (int i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < std::strlen(teclado[i]); j++)
        {
            if (i == 2)
            {
                offsetX = -1.2;
            }
            else if (i == 3)
            {
                offsetX = -1.0;
            }
            teclas[index++] = {teclado[i][j], offsetX + j * 0.3, offsetY - i * 0.3, 0.4 - i * 0.1, false, 1.0, 1.0, 1.0};
        }
    }
    teclas[index++] = {' ', 0.0, offsetY - 1.2, 0.1, false};
}

void Inicializa(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0, 1.0, 1.0, 150.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glewInit();
    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
        std::cout << "Ready for GLSL - vertex, fragment, and geometry units" << std::endl;
    else
    {
        std::cout << "No GLSL support" << std::endl;
        exit(1);
    }
    setShaders(prog_hdlr, "shaders/vert_shader.glsl", "shaders/frag_shader.glsl");
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    glUseProgram(prog_hdlr);
    GLint lightPosLoc = glGetUniformLocation(prog_hdlr, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(prog_hdlr, "viewPos");
    GLint lightColorLoc = glGetUniformLocation(prog_hdlr, "lightColor");
    objectColorLoc = glGetUniformLocation(prog_hdlr, "objectColor");

    glUniform3f(lightPosLoc, 0.5f, 20.5f, 10.0f); // Posição da luz
    glUniform3f(viewPosLoc, 0.0f, -40.0f, 0.0f);  // Posição da câmera
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // Cor da luz
    glUniform3f(objectColorLoc, 1.0f, 0.8f, 0.3f);

    glDepthFunc(GL_LESS);
    planet = gluNewQuadric();
    glEnable(GL_MULTISAMPLE);
    InicializaTeclas();
    InicializaCarimbos();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(100, 100);
    float maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Maquina de Escrever em 3D");
    glutDisplayFunc(Desenha);
    glutKeyboardFunc(Teclado);
    glutKeyboardUpFunc(TecladoSolto);
    glutSpecialFunc(TeclasEspeciais);
    glutSpecialUpFunc(TeclasEspeciaisUp);
    Inicializa();
    glutMainLoop();
    return 0;
}