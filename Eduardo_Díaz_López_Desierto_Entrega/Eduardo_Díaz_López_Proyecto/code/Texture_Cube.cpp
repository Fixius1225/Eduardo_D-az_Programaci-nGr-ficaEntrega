// Texture_Cube.cpp
//Eduardo Díaz López
// Extraido del ejemplo delprofesor.
// angel.rodriguez@udit.es

#include "Texture_Cube.hpp"
#include <vector>
#include <iostream>

namespace udit
{
    Texture_Cube::Texture_Cube(const std::string& texture_base_path)
    {
        texture_is_loaded = false;

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        static const GLenum texture_target[] =
        {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        };


        unsigned char sky_pixel[4] = { 135, 206, 235, 255 };

        for (int i = 0; i < 6; i++)
        {

            glTexImage2D(
                texture_target[i],
                0,
                GL_RGBA,
                1, 
                1, 
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                sky_pixel
            );
        }

       
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        texture_is_loaded = true;
        std::cout << "Skybox generado por codigo (Color Solido) para evitar errores de archivo." << std::endl;
    }

    Texture_Cube::~Texture_Cube()
    {
        if (texture_is_loaded)
        {
            glDeleteTextures(1, &texture_id);
        }
    }
}