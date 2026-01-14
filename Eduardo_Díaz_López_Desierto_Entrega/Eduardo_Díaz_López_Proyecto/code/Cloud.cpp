// Cloud.cpp
//C´´odigo para crear una nube en 3D
//Eduardo Díaz López
#include "Cloud.hpp"
#include <vector>

namespace udit
{
    
    void add_cloud_box(std::vector<float>& vertices, float x, float y, float z, float sx, float sy, float sz) {
        float hx = sx / 2.0f; float hy = sy / 2.0f; float hz = sz / 2.0f;
        
        // Generador rápido de cubo
        float v[] = {
           -hx,-hy,hz,0,0,1,0,0, hx,-hy,hz,0,0,1,1,0, hx,hy,hz,0,0,1,1,1, hx,hy,hz,0,0,1,1,1, -hx,hy,hz,0,0,1,0,1, -hx,-hy,hz,0,0,1,0,0, // Front
           -hx,-hy,-hz,0,0,-1,0,0, -hx,hy,-hz,0,0,-1,0,1, hx,hy,-hz,0,0,-1,1,1, hx,hy,-hz,0,0,-1,1,1, hx,-hy,-hz,0,0,-1,1,0, -hx,-hy,-hz,0,0,-1,0,0, // Back
           -hx,hy,hz,-1,0,0,1,0, -hx,hy,-hz,-1,0,0,1,1, -hx,-hy,-hz,-1,0,0,0,1, -hx,-hy,-hz,-1,0,0,0,1, -hx,-hy,hz,-1,0,0,0,0, -hx,hy,hz,-1,0,0,1,0, // Left
            hx,hy,hz,1,0,0,1,0, hx,-hy,-hz,1,0,0,0,1, hx,hy,-hz,1,0,0,1,1, hx,-hy,-hz,1,0,0,0,1, hx,hy,hz,1,0,0,1,0, hx,-hy,hz,1,0,0,0,0, // Right
           -hx,hy,-hz,0,1,0,0,1, -hx,hy,hz,0,1,0,0,0, hx,hy,hz,0,1,0,1,0, hx,hy,hz,0,1,0,1,0, hx,hy,-hz,0,1,0,1,1, -hx,hy,-hz,0,1,0,0,1, // Top
           -hx,-hy,-hz,0,-1,0,0,1, hx,-hy,-hz,0,-1,0,1,1, hx,-hy,hz,0,-1,0,1,0, hx,-hy,hz,0,-1,0,1,0, -hx,-hy,hz,0,-1,0,0,0, -hx,-hy,-hz,0,-1,0,0,1 // Bottom
        };
        for (int i = 0; i < 36; i++) {
            vertices.push_back(v[i * 8] + x); vertices.push_back(v[i * 8 + 1] + y); vertices.push_back(v[i * 8 + 2] + z);
            vertices.push_back(v[i * 8 + 3]); vertices.push_back(v[i * 8 + 4]); vertices.push_back(v[i * 8 + 5]);
            vertices.push_back(v[i * 8 + 6]); vertices.push_back(v[i * 8 + 7]);
        }
    }

    Cloud::Cloud()
    {
        std::vector<float> data;

        //Conjunto de nubes 
        // Centro
        add_cloud_box(data, 0.0f, 0.0f, 0.0f, 1.5f, 1.0f, 1.0f);
        // Bulto izquierda
        add_cloud_box(data, -1.0f, -0.2f, 0.0f, 1.0f, 0.8f, 0.9f);
        // Bulto derecha
        add_cloud_box(data, 1.0f, 0.1f, 0.2f, 1.1f, 0.9f, 0.8f);
        // Bulto arriba
        add_cloud_box(data, 0.2f, 0.6f, -0.1f, 1.2f, 0.8f, 0.9f);

        glGenVertexArrays(1, &vao); glGenBuffers(1, &vbo);
        glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }

    Cloud::~Cloud() { glDeleteVertexArrays(1, &vao); glDeleteBuffers(1, &vbo); }

    void Cloud::render(const glm::mat4& m, GLint loc, GLint use, GLint col) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
        glUniform1i(use, false);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36 * 4); //4 bloques
        glBindVertexArray(0);
    }
}