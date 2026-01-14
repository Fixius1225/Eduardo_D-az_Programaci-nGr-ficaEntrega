// Rock.hpp Código para crear rocas que luego se multilicaran por el entorno
#ifndef ROCK_HEADER
#define ROCK_HEADER

#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{
    class Rock
    {
    private:
        GLuint vao;
        GLuint vbo;

    public:
        Rock(int amount);
        ~Rock();

        
        void render(const glm::mat4& model_view_matrix, GLint model_view_loc, GLint use_tex_loc, GLint color_loc);
    };
}
#endif