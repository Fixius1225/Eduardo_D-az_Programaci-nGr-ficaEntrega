// Cactus.hpp Código para crear un cactus
//Código por eduardo Díaz López
#ifndef CACTUS_HEADER
#define CACTUS_HEADER

#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{
    class Cactus
    {
    private:
        GLuint vao;
        GLuint vbo;

    public:
        Cactus(); //Construye cactus
        ~Cactus();

        void render(const glm::mat4& model_view_matrix, GLint model_view_loc, GLint use_tex_loc, GLint color_loc);
    };
}
#endif