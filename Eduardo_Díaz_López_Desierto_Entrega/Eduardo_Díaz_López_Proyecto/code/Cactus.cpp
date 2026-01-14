// Cactus.cpp
//Código para el cactus
//Eduardo Díaz López

#include "Cactus.hpp"
#include <vector>

namespace udit
{
    // scale=Tamaño
    void add_box(std::vector<float>& vertices, float x, float y, float z, float sx, float sy, float sz) {
        float hx = sx / 2.0f; float hy = sy / 2.0f; float hz = sz / 2.0f;

        // Vértices base de un cubo
        float v[] = {
            // Frente
            -hx, -hy,  hz,  0,0,1, 0,0,   hx, -hy,  hz,  0,0,1, 1,0,   hx,  hy,  hz,  0,0,1, 1,1,
             hx,  hy,  hz,  0,0,1, 1,1,  -hx,  hy,  hz,  0,0,1, 0,1,  -hx, -hy,  hz,  0,0,1, 0,0,
             //Atrás
             -hx, -hy, -hz,  0,0,-1, 0,0,  -hx,  hy, -hz,  0,0,-1, 0,1,   hx,  hy, -hz,  0,0,-1, 1,1,
              hx,  hy, -hz,  0,0,-1, 1,1,   hx, -hy, -hz,  0,0,-1, 1,0,  -hx, -hy, -hz,  0,0,-1, 0,0,
              //Izquierda
              -hx,  hy,  hz, -1,0,0, 1,0,  -hx,  hy, -hz, -1,0,0, 1,1,  -hx, -hy, -hz, -1,0,0, 0,1,
              -hx, -hy, -hz, -1,0,0, 0,1,  -hx, -hy,  hz, -1,0,0, 0,0,  -hx,  hy,  hz, -1,0,0, 1,0,
              //Derecha
               hx,  hy,  hz,  1,0,0, 1,0,   hx, -hy, -hz,  1,0,0, 0,1,   hx,  hy, -hz,  1,0,0, 1,1,
               hx, -hy, -hz,  1,0,0, 0,1,   hx,  hy,  hz,  1,0,0, 1,0,   hx, -hy,  hz,  1,0,0, 0,0,
               //Arriba
               -hx,  hy, -hz,  0,1,0, 0,1,  -hx,  hy,  hz,  0,1,0, 0,0,   hx,  hy,  hz,  0,1,0, 1,0,
                hx,  hy,  hz,  0,1,0, 1,0,   hx,  hy, -hz,  0,1,0, 1,1,  -hx,  hy, -hz,  0,1,0, 0,1,
                //Abajo
                -hx, -hy, -hz,  0,-1,0, 0,1,   hx, -hy, -hz,  0,-1,0, 1,1,   hx, -hy,  hz,  0,-1,0, 1,0,
                 hx, -hy,  hz,  0,-1,0, 1,0,  -hx, -hy,  hz,  0,-1,0, 0,0,  -hx, -hy, -hz,  0,-1,0, 0,1
        };

        // Añadir vértices desplazados
        for (int i = 0; i < 36; i++) {
            vertices.push_back(v[i * 8] + x);       // Pos X 
            vertices.push_back(v[i * 8 + 1] + y);   // Pos Y 
            vertices.push_back(v[i * 8 + 2] + z);   // Pos Z 
            vertices.push_back(v[i * 8 + 3]);       // Normal X
            vertices.push_back(v[i * 8 + 4]);       // Normal Y
            vertices.push_back(v[i * 8 + 5]);       // Normal Z
            vertices.push_back(v[i * 8 + 6]);       // U
            vertices.push_back(v[i * 8 + 7]);       // V
        }
    }

    Cactus::Cactus()
    {
        std::vector<float> data;

        // Construimos el Cactus compuesto por 3 partes

        //Tronco Central 
        add_box(data, 0.0f, 1.5f, 0.0f, 0.5f, 3.0f, 0.5f);

        //Brazo Izquierdo
        add_box(data, -0.6f, 2.0f, 0.0f, 0.35f, 1.0f, 0.3f);
        // Conector horizontal pequeño
        add_box(data, -0.3f, 1.8f, 0.0f, 0.4f, 0.3f, 0.3f);
        //Brazo Derecho
        add_box(data, 0.6f, 1.0f, 0.0f, 0.35f, 1.0f, 0.3f);
        //Conector
        add_box(data, 0.3f, 0.8f, 0.0f, 0.4f, 0.3f, 0.3f);

        // Subir datos
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }

    Cactus::~Cactus() { glDeleteVertexArrays(1, &vao); glDeleteBuffers(1, &vbo); }

    void Cactus::render(const glm::mat4& m, GLint loc, GLint use_tex, GLint col) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
        glUniform1i(use_tex, false);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36 * 5); //5 cubos en total
        glBindVertexArray(0);
    }
}