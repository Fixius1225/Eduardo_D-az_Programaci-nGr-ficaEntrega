// Texture_Cube.hpp Código para crear un cubo con textura
#ifndef TEXTURE_CUBE_HEADER
#define TEXTURE_CUBE_HEADER

#include <string>
#include <glad/gl.h>

namespace udit
{
    class Texture_Cube
    {
    private:
        GLuint texture_id;
        bool   texture_is_loaded;

    public:
        Texture_Cube(const std::string& texture_base_path);
        ~Texture_Cube();

        //evitar errores de memoria
        Texture_Cube(const Texture_Cube&) = delete;
        Texture_Cube& operator = (const Texture_Cube&) = delete;

        bool is_ok() const { return texture_is_loaded; }

        bool bind() const
        {
            if (texture_is_loaded)
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
                return true;
            }
            return false;
        }
    };
}
#endif