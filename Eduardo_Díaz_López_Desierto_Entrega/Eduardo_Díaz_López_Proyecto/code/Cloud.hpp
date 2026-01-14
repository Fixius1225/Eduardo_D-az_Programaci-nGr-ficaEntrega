// Cloud.hpp código para crear una nube en 3D.
#ifndef CLOUD_HEADER
#define CLOUD_HEADER

#include <glad/gl.h>
#include <glm.hpp>

namespace udit
{
    class Cloud
    {
    private:
        GLuint vao;
        GLuint vbo;

    public:
        Cloud();
        ~Cloud();
                
        void render(const glm::mat4& model_view_matrix, GLint model_view_loc, GLint use_tex_loc, GLint color_loc);
    };
}
#endif