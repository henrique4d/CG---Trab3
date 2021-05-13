#include <GL/gl.h>
#include <GL/glut.h>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <sstream>

struct vec3{
    double x, y, z;
};
struct vec2{
    double x, y;
};

char mode = 'm';
int nave = 0;

int height = 0;
int vel = 0;

//point of view
int pov[3][2][3] = {{{60,50,0},{-500,600,0}},{{35,227,0},{-500,600,0}} ,{{245,90,0},{-500,600,0}}};
//foco 
int view[3][2][3] = {{{500,100,0},{100,0,0}}, {{500,227,0},{100,0,0}}, {{500,90,0},{100,0,0}}};
int cur_pov = 1;    //começa fora do objeto
int cur_view = 1;

int angle = 0;
bool right = false; 
bool left = false;

GLfloat andar[2] = {0,0};

//iluminação 
GLfloat posLuz[] = {0.0,20000.0,0.0,1.0};
GLfloat ambiente[] = {0.8,0.8,0.8,1.0};
GLfloat difus[] = {0.5,0.5,0.5,1.0};
GLfloat especular[] = {0.5,0.5,0.5,1.0};


//vetores dos dirigiveis
std::vector<std::vector<vec3>> d_v(3);
std::vector<std::vector<vec2>> d_t(3);
std::vector<std::vector<vec3>> d_n(3);

//vetores chao
std::vector<vec3> chao_v;
std::vector<vec2> chao_t;
std::vector<vec3> chao_n;

//vetores das arvores
std::vector<vec3> a1_v;
std::vector<vec2> a1_t;
std::vector<vec3> a1_n;

std::vector<vec3> a2_v;
std::vector<vec2> a2_t;
std::vector<vec3> a2_n;

std::vector<vec3> a3_v;
std::vector<vec2> a3_t;
std::vector<vec3> a3_n;

//vetores das estruturas
std::vector<vec3> c1_v;
std::vector<vec2> c1_t;
std::vector<vec3> c1_n;

std::vector<vec3> c2_v;
std::vector<vec2> c2_t;
std::vector<vec3> c2_n;

//vetores dos animais
std::vector<vec3> animal1_v;
std::vector<vec2> animal1_t;
std::vector<vec3> animal1_n;

std::vector<vec3> animal2_v;
std::vector<vec2> animal2_t;
std::vector<vec3> animal2_n;

std::vector<vec3> animal3_v;
std::vector<vec2> animal3_t;
std::vector<vec3> animal3_n;

//montanhas
std::vector<vec3> m1_v;
std::vector<vec2> m1_t;
std::vector<vec3> m1_n;

std::vector<vec3> m2_v;
std::vector<vec2> m2_t;
std::vector<vec3> m2_n;

//planets
std::vector<vec3> p1_v;
std::vector<vec2> p1_t;
std::vector<vec3> p1_n;

std::vector<vec3> p2_v;
std::vector<vec2> p2_t;
std::vector<vec3> p2_n;

/*--------------------------------------------------------------------------------------------------*/

void parser(std::vector<vec3> &out_vertices, std::vector<vec2> &out_texcoord, std::vector<vec3> &out_normals, std::string file_name){
    if(out_vertices.size() != 0) return;    //se ja leu, retorna.

    //vetor temporario para ler o arquivo
    std::vector<vec3> vertex_positions;
    std::vector<vec2> vertex_texcoord;
    std::vector<vec3> vertex_normals;

    //vetor que le os indices no "F"
    std::vector<GLint> vertex_position_indice;
    std::vector<GLint> vertex_texcoord_indice;
    std::vector<GLint> vertex_normal_indice;

    std::stringstream ss;
    std::ifstream in_file(file_name); //nome do arquivo, aq vamos ter q mudar ne
    std::string line = "";             //lê a linha do arquivo
    std::string prefix = "";           //lê o prefixo
    vec3 temp_vec3;
    vec2 temp_vec2;
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

        if (prefix == "v"){ //se for um vertice
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
        }

        else if (prefix == "vt"){ //esse daq eu n lembro praq que serve, mas acho que é a textura
            ss >> temp_vec2.x >> temp_vec2.y;
            vertex_texcoord.push_back(temp_vec2);
        }

        else if (prefix == "vn"){ //se for a normal
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_normals.push_back(temp_vec3);
        }

        else if (prefix == "f"){ //se for a face
            int counter = 0;
            while (ss >> temp_glint) {
                // o prefixo F no arquivo obj le algo do tipo "x/y/z", x representa o indice do vertex_position, ou seja, representa qual vertice vai desenhar primeiro
                //o y representa a textura e o x a normal daquela face
                if (counter == 0) //se for 0 coloca o indice no vetor de indices do vertice
                    vertex_position_indice.push_back(temp_glint);

                else if (counter == 1) //se for 1 coloca o indice no vetor de indices da textura
                    vertex_texcoord_indice.push_back(temp_glint);

                else if (counter == 2) //se for 2 coloca o indice no vetor de indices da normal
                    vertex_normal_indice.push_back(temp_glint);

                if (ss.peek() == '/'){ //ignora o / e incrementa o counter
                    ++counter;
                    ss.ignore(1, '/');
                }
                else if (ss.peek() == ' '){ // ignora o espaço e incrementa o counter
                    ++counter;
                    ss.ignore(1, ' ');
                }

                if (counter > 2) counter = 0; //volra para zero para poder fazer a representação do "x/y/z"
            }
        }
    }

    //depois de ler as faces, fica guardado apenas os indices que vamos usar
    //temos q tratar e colocar no vetor final os valores exatos que vamos uasr, tanto dos vertices, das texturas e das normais
    for (int i = 0; i < vertex_position_indice.size(); i++) {
        int vertexIndex = vertex_position_indice[i];

        vec3 vertex = vertex_positions[vertexIndex - 1];
        out_vertices.push_back(vertex);
    }

    for (int i = 0; i < vertex_texcoord_indice.size(); i++) {
        int vertexIndex = vertex_texcoord_indice[i];

        vec2 vertex = vertex_texcoord[vertexIndex - 1];
        out_texcoord.push_back(vertex);
    }

    for (int i = 0; i < vertex_normal_indice.size(); i++){
        int vertexIndex = vertex_normal_indice[i];

        vec3 vertex = vertex_normals[vertexIndex - 1];
        out_normals.push_back(vertex);
    }
}

void texto(const std::string &texto, int x, int y, double tamanhoX, double tamanhoY) { //função que escreve algo na tela
    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(tamanhoX, tamanhoY, 1);
    
    for(int i=0; i<texto.length(); i++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, texto[i]);
    }

    glPopMatrix();

    if(mode == 'g'){    //escreve altura e velocidade na tela qnd esta pov: out
        std::string cur_vel = std::to_string(vel);
        std::string aux = "altura:";
        std::string cur_alt;

        if(nave == 2)  cur_alt = std::to_string(height + 100000);
        else cur_alt = std::to_string(height);

        glPushMatrix();

        glTranslatef(x + 130, y, 0);
        glScalef(tamanhoX, tamanhoY, 1);
        
        for(int i=0; i<cur_vel.length(); i++) {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, cur_vel[i]);
        }

        glTranslatef(-1800, 0, 0);
        for(int i=0; i<aux.length(); i++) {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, aux[i]);
        }

        glTranslatef(25, 0, 0);
        for(int i=0; i<cur_alt.length(); i++) {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, cur_alt[i]);
        }

        glPopMatrix();
    }
}

void quadrado(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) { //desenha quadrados de acordo com os parâmetros
    glBegin (GL_QUADS);
        glVertex2f (x1 , y1);
        glVertex2f (x2 , y1);
        glVertex2f (x2 , y2);
        glVertex2f (x1 , y2);
    glEnd();
}

void menu() {   
    //titulo
    glColor3f(1,1,1);
    texto("Mundinho",-115,400,0.4,0.4);

    //ajuda
    glColor3f(1,1,1);
    texto("Help",120,250,0.4,0.4);

    texto("^",-130,135,0.4,0.4); texto(" - sobe",-110,145,0.2,0.2);
    texto("V",-128,45,0.4,0.4); texto(" - desce",-110,45,0.2,0.2);
    texto(">",140,135,0.4,0.4); texto(" - gira para direita",160,145,0.2,0.2);
    texto("<",140,45,0.4,0.4); texto(" - gira para esquerda",160,45,0.2,0.2);

    texto("W",-130,-35,0.4,0.4); texto(" - acelera",-110,-25,0.2,0.2);
    texto("S",140,-35,0.4,0.4); texto(" - desacelera",160,-25,0.2,0.2);

    texto("F",-130,-115,0.4,0.4); texto(" - POV: out",-110,-105,0.2,0.2);
    texto("I",150,-115,0.4,0.4); texto(" - POV: inside",160,-105,0.2,0.2);

    texto("H",20,-195,0.4,0.4); texto(" - volta para o menu",40,-185,0.2,0.2); texto("e reseta o jogo",115,-215,0.2,0.2);
    texto("esc",20,-280,0.4,0.4);  texto(" - fecha o jogo",85,-275,0.2,0.2);
    
    //comandos
    glColor3f(1,1,1); 
    texto("Aperte1",-380,280,0.3,0.3);
    texto("Aperte2",-380,20,0.3,0.3);
    texto("Aperte3",-380,-240,0.3,0.3);

    glBegin(GL_LINE_LOOP);
        glVertex2f(-140,320);
        glVertex2f(475,320);
        glVertex2f(475,-340);
        glVertex2f(-140,-340);
    glEnd();

    //naves
    glColor3f(0,0,0); 
    texto("Aviao",-375,200,0.4,0.4);
    texto("~",-300,225,0.2,0.2);
    glColor3f(0.5,0.5,0.5);
    quadrado(-400,180,-220,260);

    glColor3f(0,0,0); 
    texto("Harpy",-380,-60,0.4,0.4);
    glColor3f(0.5,0.5,0.5);
    quadrado(-400,-80,-220,0);

    glColor3f(0,0,0); 
    texto("millenium",-365,-290,0.2,0.2); texto("falcon",-345,-330,0.2,0.2);
    glColor3f(0.5,0.5,0.5);
    quadrado(-400,-340,-220,-260);

}

void desenhaOBJ(const std::vector<vec3> &out_vertices, const std::vector<vec2> &out_texcoord, const std::vector<vec3> &out_normals){
    //desenha o obj
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < out_vertices.size(); i++){  //lê o vetor de vertices e desenha
        glNormal3f(out_normals[i].x,out_normals[i].y,out_normals[i].z);
        glVertex3f(out_vertices[i].x, out_vertices[i].y, out_vertices[i].z);
    }
    glEnd();
}

void drawNave(){    //desenha nave
    GLfloat mat_ambient[ ] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_diffuse[ ] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[ ] = { 0, 0, 0, 1.0 };
    GLfloat mat_shininess[ ] = { 0, 0, 0, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    

    if(nave == 0) glColor3f(0.5,0.5,0.5);   //cada nave tem 1 cor
    if(nave == 1) glColor3f(0.76,0.23,0.13);
    if(nave == 2) glColor3f(1,0.7,0.75);
        
    desenhaOBJ(d_v[nave], d_t[nave], d_n[nave]);

    glutPostRedisplay();
}

void drawGramSky(){ //desenha o chao, o céu e as paredes
    GLfloat mat_ambient[ ] = { 1, 1, 1, 1.0 };
    GLfloat mat_diffuse[ ] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_specular[ ] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_shininess[ ] = { 1.0, 0, 0, 1.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
    glColor3f(0.13,0.55,0.13);
    desenhaOBJ(chao_v, chao_t, chao_n);

    glPushMatrix(); //céu
        glColor3f(0.12,0.699,0.756);
        glTranslated(1,2000,1);
        desenhaOBJ(chao_v, chao_t, chao_n);
    glPopMatrix();

    //paredes
    glPushMatrix();
        glColor3f(0.88,1,1);
        glTranslated(6000,0,0);
        glRotated(90,0,0,1);
        desenhaOBJ(chao_v, chao_t, chao_n);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.88,1,1);
        glTranslated(-6000,0,0);
        glRotated(90,0,0,1);
        desenhaOBJ(chao_v, chao_t, chao_n);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.88,1,1);
        glTranslated(0,0,-6000);
        glRotated(90,1,0,0);
        desenhaOBJ(chao_v, chao_t, chao_n);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.88,1,1);
        glTranslated(0,0,6000);
        glRotated(90,1,0,0);
        desenhaOBJ(chao_v, chao_t, chao_n);
    glPopMatrix();
}

void drawTree(){    //desenha as árvores
    GLfloat mat_ambient[ ] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_diffuse[ ] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[ ] = { 0, 0, 0, 1.0 };
    GLfloat mat_shininess[ ] = { 0, 0, 0, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


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

void drawCastle(){  //desenha os castelos
    GLfloat mat_ambient[ ] = { 1, 1, 1, 1.0 };
    GLfloat mat_diffuse[ ] = { 1, 1, 1, 1.0 };
    GLfloat mat_specular[ ] = { 1, 1, 1, 1.0 };
    GLfloat mat_shininess[ ] = { 1.0, 0, 0, 1.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);


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
}

void drawAnimals(){ //desenha os pokemons e os digimons
    GLfloat mat_ambient[ ] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_diffuse[ ] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[ ] = { 0, 0, 0, 1.0 };
    GLfloat mat_shininess[ ] = { 0, 0, 0, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


    for(int i = 0; i < 3; i ++){
    glPushMatrix();
        glTranslated(100 + i*1000,42,140 + i*500);
        glColor3f(1,0.64,0);
        desenhaOBJ(animal1_v, animal1_t, animal1_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(200 + i*700,100,0 + i*2000);
        glColor3f(1,0.79,0.64);
        desenhaOBJ(animal2_v, animal2_t, animal2_n);
    glPopMatrix();

    glPushMatrix();
        glTranslated(300 + i*500,16,0 + i*300);
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

void drawM(){   //desenha as montanhas
    GLfloat mat_ambient[ ] = { 1, 1, 1, 1.0 };
    GLfloat mat_diffuse[ ] = { 1, 1, 1, 1.0 };
    GLfloat mat_specular[ ] = { 1, 1, 1, 1.0 };
    GLfloat mat_shininess[ ] = { 1.0, 0, 0, 1.0 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
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

void move(){    //incrementa a posição do mundo em relação ao avião
    double c = 3.14159265359/180;
    andar[0] += vel*cos(c*angle);
    andar[1] += vel*sin(c*angle);

    posLuz[0] -= vel*cos(c*angle);
    posLuz[2] -= vel*sin(c*angle);

    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);

    

    glPushMatrix(); //escreve a velocidade e a altura na tela quando esta no pov: out
        if(nave == 2) glColor3f(1,0,0);
        else glColor3f(1,1,1);
        glRotated(270,0,1,0);
        glRotated(270,1,0,0);

        glTranslated(100,-430,0);
        
        texto("velocidade:",0,0,0.2,0.2);
    glPopMatrix();
}

void draw(void){    //desenha o mapa
    move();

    glPushMatrix();             //movimenta o mundo em relação ao avião
        glRotated(angle,0,1,0); //roda o mundo

        glTranslated(-andar[0],-height,-andar[1]);
        
        
        drawGramSky();  //desenha o mapa
        drawCastle();
        drawAnimals();
        drawM();
    glPopMatrix();

    glutPostRedisplay();
}

void draw1(void){   //desenha o mapa da millenium falcon
    move();

    glPushMatrix(); //movimenta o mundo em relação ao avião
        glRotated(angle,0,1,0);                     //roda o mundo
        glTranslated(-andar[0],-height,-andar[1]);  //transalada o mundo

        glPushMatrix();
            for(int i = 0; i < 6; i ++){            //desenha varios planetas
                glRotated(i*72, i%2, i, 0);
                if(i%2 == 0) glTranslated(1500 + 7000*i,0 + 3000*i, 2000 + 3500*i);
                else glTranslated(1500 - 14000*i,0 - 1200*i, 2000 - 4500*i);
                glColor3f(i%2 + 0.3,i%2*2 + 0.2,i%2*3 + 0.4);
                desenhaOBJ(p1_v, p1_t, p1_n);
            }
        glPopMatrix();

        glPushMatrix(); //sol
            glColor3f(1,1,0);
            glTranslated(20000,0,0);
            desenhaOBJ(p2_v, p2_t, p2_n);
        glPopMatrix();

        glPushMatrix(); //desenha varios planos
            glScalef(0.3,0.3,0.3);
            for(int i = 0; i < 6; i ++){
                glRotated(i*72, i%2, i, 0);
                if(i%2 == 0) glTranslated(-1500 + 7000*i,0 + 3000*i, -2000 + 3500*i);
                else glTranslated(-1500 - 14000*i,0 - 1200*i, -2000 - 4500*i);
                glColor3f(i%2 + 0.3,i%2*2 + 0.2,i%2*3 + 0.4);
                
                desenhaOBJ(p2_v, p2_t, p2_n);
            }
        glPopMatrix();
    glPopMatrix();

    glutPostRedisplay();
}

void SpecialKeys(int key, int x, int y) {
    if(mode == 'g'){
        if(key == GLUT_KEY_UP) {    //sobe
                height += 15;
                posLuz[1] -= 15;
            }
        if(key == GLUT_KEY_DOWN) {  //desce
                height -= 15;
                posLuz[1] += 15;
            }
        if(key == GLUT_KEY_RIGHT) {  //gira para a direita
                if(right == true){
                    if(angle < 360) angle ++;
                    else angle = 0;
                }
                else{
                    right = true;
                    left = false;
                }
            }
        if(key == GLUT_KEY_LEFT) {  //gira para esquerda
                if(left == true){
                    if(angle > 0) angle --;
                    else angle = 360;
                }
                else{
                    right = false;
                    left = true;
                }
            }
    }
}

void changeGame(){
    if(mode == 'm'){    //mudda do 3d para o 2d
        glClearColor(0.0, 0.0, 0.0, 0.0);
        
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-500, 500, -500, 500);
        glShadeModel (GL_SMOOTH);
        glMatrixMode(GL_MODELVIEW);
    }
    else if(mode == 'g'){   //muda do 2d para 3d
        glClearColor(0.0, 0.0, 0.0, 0.0);
/////////////////////////////////////////////////////////////////////////
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

        //glLightfv(GL_LIGHT0, GL_SPECULAR, especular );
        glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, difus);
        glLightfv(GL_LIGHT0, GL_SPECULAR, especular);

        glEnable(GL_LIGHT0);
        /*glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);*/
//////////////////////////////////////////////////////////////////////////
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-40.0, 40.0, -40.0, 40.0, 50.0, 1200000.0); //proje��o perspectiva
        glMatrixMode(GL_MODELVIEW);
    }
}

void reset(){   //reseta o jogo
    angle = 0;
    right = false; 
    left = false;
    height = 0;
    vel = 0;
    andar[0] = 0; andar[1] = 0;
}

void HandleKeyboard(unsigned char key, int x, int y) {
    if(mode == 'g'){    //no jogo
        switch (key){
            case 'W':   //acelera pra frente
            case 'w':
                vel += 1;
                break;
            
            case 'S':   //acelera para tras
            case 's':
                vel -= 1;
                break;
            
            case 'F':   //pov out. POV = point of view
            case 'f':
                cur_pov = 1;
                cur_view = 1;
                break;

            case 'I':   //pov inside
            case 'i':
                cur_pov = 0;
                cur_view = 0;
                break;
            case 'h':
            case 'H':
                mode = 'm';

                changeGame();
                break;
/////////////////////////////////////////////////////////////////
            case 'l':
            case 'L': 
                glEnable(GL_LIGHTING);
                break;
            case 'a' :
                glDisable(GL_LIGHTING);
                break;
            case 'q' : 
                glShadeModel(GL_FLAT);;
                break;
            case 'g' :
                glShadeModel(GL_SMOOTH);
                break;
            case 27: // “esc” on keyboard
                exit(0);
                break;
            default: // “a” on keyboard
                break;
            //////////////////////////////////////////
        }
    }

    if(mode == 'm'){    //no menu
        switch (key){
            case '1':   //seleciona o aviao
                mode = 'g';
                nave = 0;
                reset();
                changeGame();

                break;
            case '2':   //seleciona a harpia
                mode = 'g';
                nave = 1;
                reset();
                changeGame();

                break;
            case '3':   //seleciona a millenium falcon
                mode = 'g';
                nave = 2;
                reset();
                changeGame();

                break;
            case 27:    //fecha o jogo
                exit(0);
                break;
        }
    }
}

void init(void){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    mode = 'm';
    changeGame();
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(mode == 'm'){            //menu
        glLoadIdentity();
        menu();
    }
    else if(mode == 'g'){       //game
        glLoadIdentity();
        gluLookAt(pov[nave][cur_pov][0], pov[nave][cur_pov][1], pov[nave][cur_pov][2],       // define posicao do observador (x -> esquerda roda, y -> cima embaixo, z ->tras frente)
                  view[nave][cur_view][0], view[nave][cur_view][1], view[nave][cur_view][2], // ponto de interesse (foco)
                  0.0, 1.0, 0.0);// vetor de "view up"
        
        if(nave != 2) draw();   //mapa 1
        else draw1();           //mapa millenium falcon

        drawNave();
        
    glBegin(GL_LINES);
        glColor3f(0,0,1); //coloquei isso daq so pra ficar mais legal de ver o desenho, se fosse tudo da mesma cor ia ficar ruim de visualizar
        glVertex3f(0,0,0);
        glVertex4fv(posLuz);
    glEnd();
    
    }
    //glutSolidSphere(1.0, 50, 16);
    glFlush();
    glutSwapBuffers();
}

void readOBJ(){//desenhos
    parser(d_v[0], d_t[0], d_n[0], "aviao2.obj");
    parser(d_v[1], d_t[1], d_n[1], "harpy.obj");
    parser(d_v[2], d_t[2], d_n[2], "falcon.obj");

    parser(animal1_v, animal1_t, animal1_n, ("charizard.obj"));
    parser(animal2_v, animal2_t, animal2_n, ("gumdramon.obj"));
    parser(animal3_v, animal3_t, animal3_n, ("umbreon.obj"));

    parser(m1_v, m1_t, m1_n, ("montanha1.obj"));

    parser(c1_v, c1_t, c1_n, ("castle1.obj"));

    parser(a1_v, a1_t, a1_n, ("arvore1.obj"));
    parser(a2_v, a2_t, a2_n, ("arvore2.obj"));
    parser(a3_v, a3_t, a3_n, ("arvore3.obj"));

    parser(chao_v, chao_t, chao_n, "grama.obj");

    parser(p1_v, p1_t, p1_n, "planet2.obj");
    parser(p2_v, p2_t, p2_n, "planet3.obj");
}

int main(int argc, char **argv){
    readOBJ();

    srand(0);
    glutInit(&argc, argv);                                    //inicializa a glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //tipo de buffer/cores/profundidade

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
