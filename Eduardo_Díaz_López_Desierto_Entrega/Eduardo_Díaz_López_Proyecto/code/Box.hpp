// Box.hpp para crear una caja de madera.
//Código por Eduardo Dïaz López
#ifndef BOX_HEADER
#define BOX_HEADER

#include <string>
#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{
    class Box
    {
    private:
        GLuint vao;
        GLuint vbo;
        GLuint texture_id;

    public:
        Box(const std::string& texture_path);
        ~Box();

        //Modelo completo
        void render(const glm::mat4& model_view_matrix, GLint model_view_loc, GLint use_tex_loc, GLint color_loc);
    };
}
#endif