#include <GL/gl.h>
#include <GL/glut.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/vec3.hpp>                  // glm::vec3
#include <glm/vec4.hpp>                  // glm::vec4
#include <glm/mat4x4.hpp>                // glm::mat4
#include <glm/ext/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp>  // glm::pi
int velA = 0;
//vetores do aviao
std::vector<glm::vec3> aviao_v;
std::vector<glm::vec2> aviao_t;
std::vector<glm::vec3> aviao_n;

//vetores chao
std::vector<glm::vec3> chao_v;
std::vector<glm::vec2> chao_t;
std::vector<glm::vec3> chao_n;

//vetores das arvores
std::vector<glm::vec3> a1_v;
std::vector<glm::vec2> a1_t;
std::vector<glm::vec3> a1_n;

std::vector<glm::vec3> a2_v;
std::vector<glm::vec2> a2_t;
std::vector<glm::vec3> a2_n;

std::vector<glm::vec3> a3_v;
std::vector<glm::vec2> a3_t;
std::vector<glm::vec3> a3_n;

//vetores das estruturas
std::vector<glm::vec3> c1_v;
std::vector<glm::vec2> c1_t;
std::vector<glm::vec3> c1_n;

std::vector<glm::vec3> c2_v;
std::vector<glm::vec2> c2_t;
std::vector<glm::vec3> c2_n;

//vetores dos animais
std::vector<glm::vec3> animal1_v;
std::vector<glm::vec2> animal1_t;
std::vector<glm::vec3> animal1_n;

std::vector<glm::vec3> animal2_v;
std::vector<glm::vec2> animal2_t;
std::vector<glm::vec3> animal2_n;

std::vector<glm::vec3> animal3_v;
std::vector<glm::vec2> animal3_t;
std::vector<glm::vec3> animal3_n;

//montanhas
std::vector<glm::vec3> m1_v;
std::vector<glm::vec2> m1_t;
std::vector<glm::vec3> m1_n;

std::vector<glm::vec3> m2_v;
std::vector<glm::vec2> m2_t;
std::vector<glm::vec3> m2_n;


/*--------------------------------------------------------------------------------------------------*/

void parser(std::vector<glm::vec3> &out_vertices, std::vector<glm::vec2> &out_texcoord, std::vector<glm::vec3> &out_normals, std::string file_name)
{
    if(out_vertices.size() != 0) return;
    //vetor que guarda as posições que vamos usar de fato

    //vetor temporario para ler o arquivo
    std::vector<glm::vec3> vertex_positions;
    std::vector<glm::vec2> vertex_texcoord;
    std::vector<glm::vec3> vertex_normals;

    //vetor que le os indices no "F"
    std::vector<GLint> vertex_position_indice;
    std::vector<GLint> vertex_texcoord_indice;
    std::vector<GLint> vertex_normal_indice;

    std::stringstream ss;
    std::ifstream in_file(file_name); //nome do arquivo, aq vamos ter q mudar ne
    std::string line = "";             //lê a linha do arquivo
    std::string prefix = "";           //lê o prefixo
    glm::vec3 temp_vec3;
    glm::vec2 temp_vec2;
    GLint temp_glint = 0;

    //verifica se o arquivo foi aberto corretamente
    if (!in_file.is_open())
        throw "Erro ao abrir o arquivo";

    //lê o arquivo
    while (std::getline(in_file, line))
    {
        ss.clear();
        ss.str(line);

        ss >> prefix;

        if (prefix == "v") //se for um vertice
        {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
        }

        else if (prefix == "vt") //esse daq eu n lembro praq que serve, mas acho que é a textura
        {
            ss >> temp_vec2.x >> temp_vec2.y;
            vertex_texcoord.push_back(temp_vec2);
        }

        else if (prefix == "vn") //se for a normal
        {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_normals.push_back(temp_vec3);
        }

        else if (prefix == "f") //se for a face
        {
            int counter = 0;
            while (ss >> temp_glint)
            {
                // o prefixo F no arquivo obj le algo do tipo "x/y/z", x representa o indice do vertex_position, ou seja, representa qual vertice vai desenhar primeiro
                //o y representa a textura e o x a normal daquela face
                if (counter == 0) //se for 0 coloca o indice no vetor de indices do vertice
                    vertex_position_indice.push_back(temp_glint);

                else if (counter == 1) //se for 1 coloca o indice no vetor de indices da textura
                    vertex_texcoord_indice.push_back(temp_glint);

                else if (counter == 2) //se for 2 coloca o indice no vetor de indices da normal
                    vertex_normal_indice.push_back(temp_glint);

                if (ss.peek() == '/') //ignora o / e incrementa o counter
                {
                    ++counter;
                    ss.ignore(1, '/');
                }
                else if (ss.peek() == ' ') // ignora o espaço e incrementa o counter
                {
                    ++counter;
                    ss.ignore(1, ' ');
                }

                if (counter > 2) //volra para zero para poder fazer a representação do "x/y/z"
                    counter = 0;
            }
        }
    }

    //depois de ler as faces, fica guardado apenas os indices que vamos usar
    //temos q tratar e colocar no vetor final os valores exatos que vamos uasr, tanto dos vertices, das texturas e das normais
    for (int i = 0; i < vertex_position_indice.size(); i++)
    {
        int vertexIndex = vertex_position_indice[i];

        glm::vec3 vertex = vertex_positions[vertexIndex - 1];
        out_vertices.push_back(vertex);
    }

    for (int i = 0; i < vertex_texcoord_indice.size(); i++)
    {
        int vertexIndex = vertex_texcoord_indice[i];

        glm::vec2 vertex = vertex_texcoord[vertexIndex - 1];
        out_texcoord.push_back(vertex);
    }

    for (int i = 0; i < vertex_normal_indice.size(); i++)
    {
        int vertexIndex = vertex_normal_indice[i];

        glm::vec3 vertex = vertex_normals[vertexIndex - 1];
        out_normals.push_back(vertex);
    }
}

void desenhaOBJ(const std::vector<glm::vec3> &out_vertices, const std::vector<glm::vec2> &out_texcoord, const std::vector<glm::vec3> &out_normals){
        //aqui desenha o arquivo. Pensei em colocar os vetores finais como globais (te mandei um audio explicando)
    int j = 0;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < out_vertices.size(); i++)
    {
        j++;
        if (j > 5)
            j = 0;
         //coloquei isso daq so pra ficar mais legal de ver o desenho, se fosse tudo da mesma cor ia ficar ruim de visualizar
        glVertex3f(out_vertices[i].x, out_vertices[i].y, out_vertices[i].z);
    }
    glEnd();
}

void drawAviao(){
    parser(aviao_v, aviao_t, aviao_n, "aviao2.obj");

    glColor3f(0,1,0);
    glPushMatrix();
        glTranslated(0,0,-velA);
        desenhaOBJ(aviao_v, aviao_t, aviao_n);
    glPopMatrix();

    glutPostRedisplay();
}

void drawGramSky(){
    parser(chao_v, chao_t, chao_n, "vegetacao/grama.obj");

    glColor3f(0.13,0.55,0.13);
    desenhaOBJ(chao_v, chao_t, chao_n);

    glPushMatrix();
        glColor3f(0,0,1);
        glTranslated(1,2000,1);
        desenhaOBJ(chao_v, chao_t, chao_n);
    glPopMatrix();
}

void drawTree(){
    parser(a1_v, a1_t, a1_n, ("vegetacao/arvore1.obj"));
    parser(a2_v, a2_t, a2_n, ("vegetacao/arvore2.obj"));
    parser(a3_v, a3_t, a3_n, ("vegetacao/arvore3.obj"));

    glColor3f(1,1,1);
    
    glPushMatrix();
        glTranslated(60,50,120);
        desenhaOBJ(a1_v, a1_t, a1_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,40,0);
        desenhaOBJ(a2_v, a2_t, a2_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(20,240,0);
        desenhaOBJ(a3_v, a3_t, a3_n);
    glPopMatrix();
}

void drawCastle(){
    parser(c1_v, c1_t, c1_n, ("castelos/castle1.obj"));
    //parser(c2_v, c2_t, c2_n, ("castelos/a.obj"));


    glColor3f(1,0,0);
    glPushMatrix();
        glTranslated(4000,460,3000);
        desenhaOBJ(c1_v, c1_t, c1_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(3000,0,2000);
        drawTree();
    glPopMatrix();

    glPushMatrix();
        glTranslated(3000,240,3000);
        desenhaOBJ(a3_v, a3_t, a3_n);
        glTranslated(120,0,0);
        desenhaOBJ(a3_v, a3_t, a3_n);
    glPopMatrix();
    //desenhaOBJ(c2_v, c2_t, c2_n);
}

void drawAnimals(){
    parser(animal1_v, animal1_t, animal1_n, ("animais/charizard.obj"));
    parser(animal2_v, animal2_t, animal2_n, ("animais/digimon.obj"));
    parser(animal3_v, animal3_t, animal3_n, ("animais/umbreon.obj"));

    glColor3f(1,1,0);
    desenhaOBJ(animal1_v, animal1_t, animal1_n);
    desenhaOBJ(animal2_v, animal2_t, animal2_n);
    desenhaOBJ(animal3_v, animal3_t, animal3_n);
}

void drawM(){
    parser(m1_v, m1_t, m1_n, ("montanha/montanha1.obj"));
    parser(m2_v, m2_t, m2_n, ("montanha/montanha2.obj"));
    
    glColor3f(0,0,0);
    glPushMatrix();
    glTranslated(4500,600,-4200);
    desenhaOBJ(m1_v, m1_t, m1_n);
    glPopMatrix();

    glPushMatrix();
    glTranslated(4500,600,-2200);
    desenhaOBJ(m1_v, m1_t, m1_n);
    glPopMatrix();
}

void SpecialKeys(int key, int x, int y) {
    if(key == GLUT_KEY_UP) {
            velA ++;
            std::cout << velA << "\n";
        }
}

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0); //cor para limpeza do buffer
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-40.0, 40.0, -40.0, 40.0, 30.0, 12000.0); //proje��o perspectiva
                                                      //proje��o perspectiva

    glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpa a janela
    glLoadIdentity();
    gluLookAt(0, 300, 200 - velA,       // define posicao do observador (x -> esquerda roda, y -> cima embaixo, z ->tras frente)
              0, 0, -velA, // ponto de interesse (foco)
              0.0, 1.0, 0.0);    // vetor de "view up"


    drawGramSky();
    drawAviao();

    drawCastle();
    drawM();
    glColor3f(0,1,0);
    glTranslated(-5600,0,-3300);

    glBegin(GL_LINES);
        glColor3f(1,0,0); //coloquei isso daq so pra ficar mais legal de ver o desenho, se fosse tudo da mesma cor ia ficar ruim de visualizar
        glVertex3f(-500,0,0);
        glVertex3f(500,0,0);
        
        glColor3f(0,0,1); //coloquei isso daq so pra ficar mais legal de ver o desenho, se fosse tudo da mesma cor ia ficar ruim de visualizar
        glVertex3f(0,-500,0);
        glVertex3f(0,500,0);

        glColor3f(0,1,0); //coloquei isso daq so pra ficar mais legal de ver o desenho, se fosse tudo da mesma cor ia ficar ruim de visualizar
        glVertex3f(0,0,-500);
        glVertex3f(0,0,500);
    glEnd();
    
    glFlush();
}

int main(int argc, char **argv)
{
    srand(0);
    glutInit(&argc, argv);                                    //inicializa a glut
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); //tipo de buffer/cores/profundidade

    glutInitWindowSize(1000, 1000);                  //dimens�es da janela
    glutInitWindowPosition(200, 200);                //posicao da janela
    glutCreateWindow("Visualizacao 3D - Exemplo 1"); //cria a janela
    init();
    glutDisplayFunc(display); //determina fun��o callback de desenho
    
    glutSpecialFunc(SpecialKeys);
    glEnable(GL_DEPTH_TEST);  //habilita remo��o de superf�cies ocultas
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // remove a parte de tras do cubo
    glutMainLoop();
    return 0;
}
