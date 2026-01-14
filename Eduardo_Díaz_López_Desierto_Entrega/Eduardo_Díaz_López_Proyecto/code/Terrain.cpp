// Terrain.cpp
//Código para construir terreno con Height map, una textura en la carpeta de Shared
#include "Terrain.hpp"
#include <iostream>
#include <SOIL2.h>
#include <cmath>

namespace udit {
    Terrain::Terrain(const std::string& heightmap_path, float width, float depth, float height_scale)
        : world_width(width), world_depth(depth), max_height(height_scale)
    {
        std::vector<float> data;
        int channels;

        //imagen
        unsigned char* img_data = SOIL_load_image(heightmap_path.c_str(), &width_samples, &depth_samples, &channels, SOIL_LOAD_RGB);

        if (!img_data) {
            std::cerr << "ERROR: No heightmap: " << heightmap_path << std::endl;
            width_samples = 2; depth_samples = 2;
            img_data = new unsigned char[12] {0};
        }

        //redimensionar
        heights.resize(width_samples * depth_samples);

        //guardar alturas
        auto get_pixel_h = [&](int x, int z) {
            if (x < 0) x = 0; if (x >= width_samples) x = width_samples - 1;
            if (z < 0) z = 0; if (z >= depth_samples) z = depth_samples - 1;
            int index = (z * width_samples + x) * 3;
            float h = (img_data[index] / 255.0f);

            // Guardamos la altura
            heights[z * width_samples + x] = h * height_scale;

            return h * height_scale;
            };

        for (int z = 0; z < depth_samples - 1; ++z) {
            for (int x = 0; x < width_samples - 1; ++x) {
                float x0 = (float)x / width_samples * width - width / 2.0f;
                float z0 = (float)z / depth_samples * depth - depth / 2.0f;
                float x1 = (float)(x + 1) / width_samples * width - width / 2.0f;
                float z1 = (float)(z + 1) / depth_samples * depth - depth / 2.0f;

                float y00 = get_pixel_h(x, z);
                float y10 = get_pixel_h(x + 1, z);
                float y01 = get_pixel_h(x, z + 1);
                float y11 = get_pixel_h(x + 1, z + 1);
                // Triangulo 1
                data.insert(data.end(), { x0, y00, z0,  0,1,0,  0,0 });
                data.insert(data.end(), { x1, y10, z0,  0,1,0,  1,0 });
                data.insert(data.end(), { x0, y01, z1,  0,1,0,  0,1 });
                // Triangulo 2
                data.insert(data.end(), { x1, y10, z0,  0,1,0,  1,0 });
                data.insert(data.end(), { x1, y11, z1,  0,1,0,  1,1 });
                data.insert(data.end(), { x0, y01, z1,  0,1,0,  0,1 });
            }
        }

        if (img_data && width_samples > 2) SOIL_free_image_data(img_data);

        vertex_count = data.size() / 8;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    }

    Terrain::~Terrain() { glDeleteVertexArrays(1, &vao); glDeleteBuffers(1, &vbo); }

    void Terrain::render() {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        glBindVertexArray(0);
    }

    // Calcula la altura del terreno
    float Terrain::get_height(float world_x, float world_z) {
        
        float img_x = (world_x + world_width / 2.0f) / world_width * width_samples;
        float img_z = (world_z + world_depth / 2.0f) / world_depth * depth_samples;
        if (img_x < 0 || img_x >= width_samples - 1 || img_z < 0 || img_z >= depth_samples - 1) {
            return 0.0f;
        }
        int x = (int)img_x;
        int z = (int)img_z;

        return heights[z * width_samples + x];
    }
}