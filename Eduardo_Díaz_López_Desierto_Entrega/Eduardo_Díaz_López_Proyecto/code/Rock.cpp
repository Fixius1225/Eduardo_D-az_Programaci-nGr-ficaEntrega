// Rock.cpp
//Script para crear roca deforme
//Eduardo Díaz López
#include "Rock.hpp"
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

namespace udit
{
    Rock::Rock(int amount)
    {
        std::vector<float> data;

        //Geometria irregular
        int stacks = 3; 
        int slices = 5; 
        float radius = 0.6f;

        for (int i = 0; i < stacks; ++i) {
            float V = i / (float)stacks;
            float phi = V * M_PI;
            float nextV = (i + 1) / (float)stacks;
            float nextPhi = nextV * M_PI;

            for (int j = 0; j < slices; ++j) {
                float U = j / (float)slices;
                float theta = U * (M_PI * 2);
                float nextU = (j + 1) / (float)slices;
                float nextTheta = nextU * (M_PI * 2);

                //Calcular vértices
                auto push_vert = [&](float t, float p, float u, float v) {
                    //Ruido Aleatorio
                    float r = radius;
                    float x = r * sin(p) * cos(t);
                    float y = r * cos(p);
                    float z = r * sin(p) * sin(t);
                    //Posicion
                    data.push_back(x); data.push_back(y); data.push_back(z);
                    data.push_back(x); data.push_back(y); data.push_back(z);
                    // UV
                    data.push_back(u); data.push_back(v);
                    };

                // Triangulo 1
                push_vert(theta, phi, U, V);
                push_vert(theta, nextPhi, U, nextV);
                push_vert(nextTheta, phi, nextU, V);
                // Triangulo 2
                push_vert(nextTheta, phi, nextU, V);
                push_vert(theta, nextPhi, U, nextV);
                push_vert(nextTheta, nextPhi, nextU, nextV);
            }
        }

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

    Rock::~Rock() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void Rock::render(const glm::mat4& m, GLint loc, GLint use, GLint col)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
        glUniform1i(use, false);
        glBindVertexArray(vao);
        //Dibujamos
        int verts = 3 * 5 * 6;// 6 vertices
        glDrawArrays(GL_TRIANGLES, 0, verts);
        glBindVertexArray(0);
    }
}