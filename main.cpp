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


int height = 0;
int vel = 0;
int pov[2][3] = {{0,50,-75},{0,300,200}};
int cur_pov = 1;

int view[2][3] = {{0,100,-500},{0,100,0}};
int cur_view = 1;

int angle = 0;
bool right = false; 
bool left = false;

int andar = 0;



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

//parede
std::vector<glm::vec3> p1_v;
std::vector<glm::vec2> p1_t;
std::vector<glm::vec3> p1_n;

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

    glColor3f(0.5,0.5,0.5);
        
    desenhaOBJ(aviao_v, aviao_t, aviao_n);

    glutPostRedisplay();
}

void drawParede(){
    parser(p1_v, p1_t, p1_n, "parede/grade.obj");

    glColor3f(0.5,0.5,0.5);
 
    desenhaOBJ(p1_v, p1_t, p1_n);
}

void drawGramSky(){
    parser(chao_v, chao_t, chao_n, "vegetacao/grama.obj");

    glColor3f(0.13,0.55,0.13);
    desenhaOBJ(chao_v, chao_t, chao_n);

    glPushMatrix();
        glColor3f(0.12,0.699,0.756);
        glTranslated(1,2000,1);
        desenhaOBJ(chao_v, chao_t, chao_n);
    glPopMatrix();
}

void drawTree(){
    parser(a1_v, a1_t, a1_n, ("vegetacao/arvore1.obj"));
    parser(a2_v, a2_t, a2_n, ("vegetacao/arvore2.obj"));
    parser(a3_v, a3_t, a3_n, ("vegetacao/arvore3.obj"));

    glColor3f(0.96,0.2,0.9);
    
    glPushMatrix();
        glTranslated(60,50,120);
        desenhaOBJ(a1_v, a1_t, a1_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,40,0);
        desenhaOBJ(a2_v, a2_t, a2_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(20,150,0);
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
        glTranslated(3000,150,3000);
        desenhaOBJ(a3_v, a3_t, a3_n);
        glTranslated(120,0,0);
        desenhaOBJ(a3_v, a3_t, a3_n);
    glPopMatrix();

    glColor3f(0.43,0.1,0.75);
    glPushMatrix();
        glTranslated(-4000,460,-3000);
        desenhaOBJ(c1_v, c1_t, c1_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(-3000,0,-2000);
        drawTree();
    glPopMatrix();

    glPushMatrix();
        glTranslated(-3000,150,-3000);
        desenhaOBJ(a3_v, a3_t, a3_n);
        glTranslated(-120,0,0);
        desenhaOBJ(a3_v, a3_t, a3_n);
    glPopMatrix();
    //desenhaOBJ(c2_v, c2_t, c2_n);
}

void drawAnimals(){
    parser(animal1_v, animal1_t, animal1_n, ("animais/charizard.obj"));
    parser(animal2_v, animal2_t, animal2_n, ("animais/digimon.obj"));
    parser(animal3_v, animal3_t, animal3_n, ("animais/umbreon.obj"));

    for(int i = 0; i < 3; i ++){
    glPushMatrix();
        glTranslated(100 + i*1000,42,140 + i*500);
        glColor3f(1,0.64,0);
        desenhaOBJ(animal1_v, animal1_t, animal1_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(100 + i*700,100,0 + i*2000);
        glColor3f(1,0.79,0.64);
        desenhaOBJ(animal2_v, animal2_t, animal2_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0 + i*500,16,0 + i*300);
        glColor3f(0.67,0.84,0.9);
        desenhaOBJ(animal3_v, animal3_t, animal3_n);
    glPopMatrix();
    }

    for(int i = 1; i < 3; i ++){
    glPushMatrix();
        glTranslated(100 + i*-1000,42,140 + i*-500);
        glColor3f(1,0.64,0);
        desenhaOBJ(animal1_v, animal1_t, animal1_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(100 + i*-700,100,0 + i*-2000);
        glColor3f(1,0.79,0.64);
        desenhaOBJ(animal2_v, animal2_t, animal2_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0 + i*-500,16,0 + i*-300);
        glColor3f(0.67,0.84,0.9);
        desenhaOBJ(animal3_v, animal3_t, animal3_n);
    glPopMatrix();
    }
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

void draw(void){
    andar += vel;
    glPushMatrix(); //movimenta o avião
        glTranslated(500,-height,andar);
        glRotated(angle,0,1,0);
        drawGramSky();
        drawCastle();
        drawAnimals();
        //drawParede();

        drawM();

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

    glPopMatrix();

    glutPostRedisplay();
}

void SpecialKeys(int key, int x, int y) {
    
    if(key == GLUT_KEY_UP) {    //sobe
            height += 15;
        }
    if(key == GLUT_KEY_DOWN) {  //desce
            height -= 15;
        }
    if(key == GLUT_KEY_RIGHT) {  //desce
            if(right == true){
                if(angle < 360) angle ++;
                else angle = 0;
            }
            else{
                right = true;
                left = false;
                //angle = 0;
            }
        }
    if(key == GLUT_KEY_LEFT) {  //desce
            if(left == true){
                if(angle > 0) angle --;
                else angle = 360;
            }
            else{
                right = false;
                left = true;
                //angle = 0;
            }
        }
}

void HandleKeyboard(unsigned char key, int x, int y) {
    switch (key){
        case 'W':   //acelera pra frente
        case 'w':
            vel += 1;
            break;
        
        case 'S':   //acelera para tras
        case 's':
            vel -= 1;
            break;
        
        case 'F':   //pov fora
        case 'f':
            cur_pov = 1;
            cur_view = 1;
            break;

        case 'I':   //pov inside
        case 'i':
            cur_pov = 0;
            cur_view = 0;
            break;
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
    gluLookAt(pov[cur_pov][0], pov[cur_pov][1], pov[cur_pov][2],       // define posicao do observador (x -> esquerda roda, y -> cima embaixo, z ->tras frente)
              view[cur_view][0], view[cur_view][1], view[cur_view][2], // ponto de interesse (foco)
              0.0, 1.0, 0.0);    // vetor de "view up"

    
    draw();

    

    drawAviao();
    
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
    glutIdleFunc(draw);
    glutKeyboardFunc(HandleKeyboard);
    glutSpecialFunc(SpecialKeys);
    glEnable(GL_DEPTH_TEST);  //habilita remo��o de superf�cies ocultas
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // remove a parte de tras do cubo
    glutMainLoop();
    return 0;
}
