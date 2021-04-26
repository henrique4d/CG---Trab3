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

//cor de cada face do cubo
GLfloat colors[6][3] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0}};

bool parser()
{
    //vetor que guarda as posições que vamos usar de fato
    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_texcoord;
    std::vector<glm::vec3> out_normals;

    //vetor temporario para ler o arquivo
    std::vector<glm::vec3> vertex_positions;
    std::vector<glm::vec2> vertex_texcoord;
    std::vector<glm::vec3> vertex_normals;

    //vetor que le os indices no "F"
    std::vector<GLint> vertex_position_indice;
    std::vector<GLint> vertex_texcoord_indice;
    std::vector<GLint> vertex_normal_indice;

    std::stringstream ss;
    std::ifstream in_file("test.obj");  //nome do arquivo, aq vamos ter q mudar ne
    std::string line = "";              //lê a linha do arquivo
    std::string prefix = "";            //lê o prefixo
    glm::vec3 temp_vec3;
    glm::vec2 temp_vec2;
    GLint temp_glint = 0;

    //verifica se o arquivo foi aberto corretamente
    if (!in_file.is_open())
    { 
        throw "Erro ao abrir o arquivo";
    }

    //lê o arquivo
    while (std::getline(in_file, line))
    {
        ss.clear();
        ss.str(line);

        ss >> prefix;

        if (prefix == "v")  //se for um vertice
        {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
        }

        else if (prefix == "vt")    //esse daq eu n lembro praq que serve, mas acho que é a textura
        {
            ss >> temp_vec2.x >> temp_vec2.y;
            vertex_texcoord.push_back(temp_vec2);
        }

        else if (prefix == "vn")    //se for a normal
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
                if (counter == 0)       //se for 0 coloca o indice no vetor de indices do vertice
                    vertex_position_indice.push_back(temp_glint);

                else if (counter == 1)  //se for 1 coloca o indice no vetor de indices da textura
                    vertex_texcoord_indice.push_back(temp_glint);

                else if (counter == 2)  //se for 2 coloca o indice no vetor de indices da normal
                    vertex_normal_indice.push_back(temp_glint);

                if (ss.peek() == '/')   //ignora o / e incrementa o counter
                {
                    ++counter;
                    ss.ignore(1, '/');
                }
                else if (ss.peek() == ' ')  // ignora o espaço e incrementa o counter
                {
                    ++counter;
                    ss.ignore(1, ' ');
                }

                if (counter > 2)    //volra para zero para poder fazer a representação do "x/y/z"
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

    //aqui desenha o arquivo. Pensei em colocar os vetores finais como globais (te mandei um audio explicando)
    int j = 0;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < out_vertices.size(); i++)
    {
        j++;
        if (j > 5)
            j = 0;
        glColor3fv(colors[j]);  //coloquei isso daq so pra ficar mais legal de ver o desenho, se fosse tudo da mesma cor ia ficar ruim de visualizar
        glVertex3f(out_vertices[i].x, out_vertices[i].y, out_vertices[i].z);
    }
    glEnd();

    return true;
}

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0); //cor para limpeza do buffer
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(-20.0, 20.0, -20.0, 20.0, 30.0, 500.0); //proje��o perspectiva
                                                      //proje��o perspectiva

    glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpa a janela
    glLoadIdentity();
    gluLookAt(0, 180, 250,       // define posicao do observador
              10.0, 150.0, 40.0, // ponto de interesse (foco)
              0.0, 1.0, 0.0);    // vetor de "view up"

    parser();
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
    glEnable(GL_DEPTH_TEST);  //habilita remo��o de superf�cies ocultas
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // remove a parte de tras do cubo
    glutMainLoop();
    return 0;
}