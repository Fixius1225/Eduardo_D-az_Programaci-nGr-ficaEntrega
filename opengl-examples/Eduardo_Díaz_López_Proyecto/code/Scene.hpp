// Scene.hpp Código para crear una escena para renderizar.
//Eduardo Dïaz López
#ifndef SCENE_HEADER
#define SCENE_HEADER

#include <glad/gl.h>
#include <string>
#include <memory> 
#include <vector> 
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Box.hpp"
#include "Cloud.hpp"
#include "Cactus.hpp"
#include "Rock.hpp"

namespace udit
{
    //Guarda datos del cactus
    struct CactusData {
        float x, z;
        float rot;
        float scaleH;
    };

    class Scene
    {
    private:
        // Shaders
        static const std::string vertex_shader_lit;
        static const std::string fragment_shader_lit;
        static const std::string post_vs;
        static const std::string post_fs;

        // OpenGL
        GLuint program_id;
        GLuint post_program_id;
        GLuint fbo_id, fbo_texture_id, rbo_id;
        GLuint quad_vao, quad_vbo;

        // Uniforms
        GLint model_view_matrix_id;
        GLint projection_matrix_id;
        GLint use_texture_id;
        GLint color_uniform_id;

        // Elementos
        Camera      camera;
        Terrain     terrain;
        std::unique_ptr<Box>    wood_box;
        std::unique_ptr<Cloud>  cloud;
        std::unique_ptr<Cactus> cactus;
        std::unique_ptr<Rock>   rocks;

        //datos dinamicos
        std::vector<glm::vec2> rockPositions;  //posiciones de rocas
        std::vector<CactusData> cactusList;    //datos de cactus
        glm::vec2 boxPosition;                 //posición de la caja

        int width;
        int height;

    public:
        Scene(int width, int height);
        ~Scene();

        void update();
        void render();
        void resize(int width, int height);
        void handle_input(const bool* keys);
        void on_drag(float xrel, float yrel);

    private:
        void init_postprocess();
        // Nueva función para leer el archivo
        void load_scene_from_file(const std::string& path);
        GLuint compile_shaders(const std::string& vs, const std::string& fs);
    };
}
#endif