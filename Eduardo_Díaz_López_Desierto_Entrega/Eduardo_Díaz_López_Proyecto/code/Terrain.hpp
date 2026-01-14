// Terrain.hpp Código para crear un terreno para la escen, 
//Se le aplica un haigthmap para crear dunas y montañas sobre la arena
//Eduardo Díaz López
#ifndef TERRAIN_HEADER
#define TERRAIN_HEADER
#include <glad/gl.h>
#include <vector>
#include <string>

namespace udit {
    class Terrain {
    private:
        GLuint vao, vbo;
        int vertex_count;

        //calcular altura
        std::vector<float> heights; //altura de cada vértice
        int width_samples, depth_samples; //Resolución de la imagen
        float world_width, world_depth, max_height;

    public:
        Terrain(const std::string& heightmap_path, float width, float depth, float height_scale);
        ~Terrain();
        void render();
        float get_height(float x, float z);
    };
}
#endif