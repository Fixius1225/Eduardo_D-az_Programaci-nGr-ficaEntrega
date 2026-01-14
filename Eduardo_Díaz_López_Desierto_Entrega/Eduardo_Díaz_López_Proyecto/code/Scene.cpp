// Scene.cpp
//Código completo de Escena para crear los objetos y shaders
//Eduardo Díaz López
#include "Scene.hpp"
#include <SDL3/SDL.h> 
#include <glm.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>  
#include <sstream>  

namespace udit
{
    using namespace std;

    //Shaders
    const string Scene::vertex_shader_lit = "#version 330\n layout (location = 0) in vec3 aPos; layout (location = 1) in vec3 aNormal; layout (location = 2) in vec2 aTexCoords; out vec3 FragPos; out vec3 Normal; out vec2 TexCoords; uniform mat4 model; uniform mat4 view; uniform mat4 projection; void main(){ FragPos = vec3(model * vec4(aPos, 1.0)); Normal = mat3(model) * aNormal; TexCoords = aTexCoords; gl_Position = projection * view * vec4(FragPos, 1.0); }";
    const string Scene::fragment_shader_lit = "#version 330\n out vec4 FragColor; in vec3 FragPos; in vec3 Normal; in vec2 TexCoords; uniform vec3 lightPos; uniform vec3 viewPos; uniform vec3 objectColor; uniform float alphaTransparency; uniform sampler2D texture1; uniform bool useTexture; void main(){ vec3 norm = normalize(Normal); vec3 lightDir = normalize(lightPos - FragPos); float diff = max(dot(norm, lightDir), 0.0); vec3 diffuse = diff * vec3(0.9); vec3 viewDir = normalize(viewPos - FragPos); vec3 reflectDir = reflect(-lightDir, norm); float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16); vec3 specular = spec * vec3(0.5); vec4 texColor = useTexture ? texture(texture1, TexCoords) : vec4(1.0); if(texColor.a < 0.1) discard; vec3 result = (vec3(0.4) + diffuse + specular) * objectColor * texColor.rgb; FragColor = vec4(result, alphaTransparency); }";
    const string Scene::post_vs = "#version 330\n layout(location=0) in vec2 p; layout(location=1) in vec2 u; out vec2 uv; void main(){gl_Position=vec4(p,0,1); uv=u;}";
    const string Scene::post_fs = "#version 330\n out vec4 C; in vec2 uv; uniform sampler2D t; uniform float time; float noise(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); } void main(){ vec3 c = texture(t, uv).rgb; vec3 sepia = vec3(dot(c, vec3(0.393, 0.769, 0.189)), dot(c, vec3(0.349, 0.686, 0.168)), dot(c, vec3(0.272, 0.534, 0.131))); c = mix(c, sepia, 0.35); float grain = noise(uv * time) * 0.05; c += grain; float d = distance(uv, vec2(0.5)); c *= smoothstep(0.9, 0.4, d); C = vec4(c, 1.0); }";

    GLuint Scene::compile_shaders(const string& vs, const string& fs) {
        GLuint v = glCreateShader(GL_VERTEX_SHADER); GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
        const char* v_c = vs.c_str(); const char* f_c = fs.c_str();
        glShaderSource(v, 1, &v_c, NULL); glShaderSource(f, 1, &f_c, NULL);
        glCompileShader(v); glCompileShader(f);
        GLuint p = glCreateProgram(); glAttachShader(p, v); glAttachShader(p, f);
        glLinkProgram(p); glDeleteShader(v); glDeleteShader(f);
        return p;
    }

    Scene::Scene(int width, int height)
        : width(width), height(height), camera(float(width), float(height)),
        //Ruta para el heightmap y sus parametros de elevacion y extension
        terrain("../../../shared/assets/height-map.png", 50.f, 50.f, 4.0f)
    {
        program_id = compile_shaders(vertex_shader_lit, fragment_shader_lit);
        post_program_id = compile_shaders(post_vs, post_fs);

        model_view_matrix_id = glGetUniformLocation(program_id, "model");
        projection_matrix_id = glGetUniformLocation(program_id, "projection");
        use_texture_id = glGetUniformLocation(program_id, "useTexture");
        color_uniform_id = glGetUniformLocation(program_id, "objectColor");

        //ruta para la textura de madera para la caja
        wood_box = std::make_unique<Box>("../../../shared/assets/wood.png");

        cloud = std::make_unique<Cloud>();
        cactus = std::make_unique<Cactus>();
        rocks = std::make_unique<Rock>(15);

        //ruta para la escena, es decir el TXT
        load_scene_from_file("../../../shared/assets/scene.txt");

        init_postprocess();
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    Scene::~Scene() { glDeleteProgram(program_id); glDeleteProgram(post_program_id); glDeleteFramebuffers(1, &fbo_id); glDeleteTextures(1, &fbo_texture_id); }

    //lector de archivos
    //En caso de no leer el txt printa un mensaje critico
    void Scene::load_scene_from_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "ERROR CRITICO: No se pudo abrir scene.txt en: " << path << std::endl;
            std::cerr << "Asegurate de que la carpeta 'assets' esta en el lugar correcto (junto al proyecto o un nivel arriba)." << std::endl;
            //por si falla el archivo
            boxPosition = glm::vec2(2.0f, -2.0f);
            return;
        }

        std::cout << "Cargando escena desde: " << path << std::endl;
        std::string line;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue; 

            std::stringstream ss(line);
            std::string type;
            ss >> type;

            if (type == "ROCK") {
                float x, z;
                ss >> x >> z;
                rockPositions.push_back(glm::vec2(x, z));
            }
            else if (type == "CACTUS") {
                CactusData c;
                ss >> c.x >> c.z >> c.rot >> c.scaleH;
                cactusList.push_back(c);
            }
            else if (type == "BOX") {
                ss >> boxPosition.x >> boxPosition.y;
            }
        }
        file.close();
    }

    void Scene::init_postprocess() {
        glGenFramebuffers(1, &fbo_id); glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glGenTextures(1, &fbo_texture_id); glBindTexture(GL_TEXTURE_2D, fbo_texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture_id, 0);
        glGenRenderbuffers(1, &rbo_id); glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        float q[] = { -1,1,0,1, -1,-1,0,0, 1,-1,1,0, -1,1,0,1, 1,-1,1,0, 1,1,1,1 };
        glGenVertexArrays(1, &quad_vao); glGenBuffers(1, &quad_vbo); glBindVertexArray(quad_vao); glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(q), q, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * 4, 0); glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * 4, (void*)8); glEnableVertexAttribArray(1);
    }

    void Scene::update() {}

    void Scene::render() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glViewport(0, 0, width, height);
        glClearColor(0.6f, 0.8f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program_id);
        glUniform3f(glGetUniformLocation(program_id, "lightPos"), 5.0f, 15.0f, 5.0f);
        glUniform3f(glGetUniformLocation(program_id, "viewPos"), 0.0f, 2.0f, 5.0f);

        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 proj = camera.get_projection_matrix();
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, &proj[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program_id, "view"), 1, GL_FALSE, &view[0][0]);
        glUniform1f(glGetUniformLocation(program_id, "alphaTransparency"), 1.0f);

        //terreno
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, &model[0][0]);
        glUniform1i(use_texture_id, false);
        glUniform3f(color_uniform_id, 0.7f, 0.6f, 0.4f);
        terrain.render();

        //caja
        if (wood_box) {
            float boxX = boxPosition.x;
            float boxZ = boxPosition.y;
            float h = terrain.get_height(boxX, boxZ);
            float hover = sin(SDL_GetTicks() / 500.0f) * 0.2f + 1.5f;

            glm::mat4 boxModel = glm::mat4(1.0f);
            boxModel[3] = glm::vec4(boxX, h + hover, boxZ, 1.0f);

            float a = SDL_GetTicks() / 1000.f;
            glm::mat4 r(1.0f); r[0][0] = cos(a); r[0][2] = -sin(a); r[2][0] = sin(a); r[2][2] = cos(a);
            boxModel = boxModel * r;

            glUniform3f(color_uniform_id, 1.0f, 1.0f, 1.0f);
            wood_box->render(boxModel, model_view_matrix_id, use_texture_id, color_uniform_id);
        }

        //cactus
        if (cactus) {
            glUniform3f(color_uniform_id, 0.1f, 0.6f, 0.1f);

            //para saber cuantos cactus hay
            for (size_t i = 0; i < cactusList.size(); i++) {
                float cX = cactusList[i].x;
                float cZ = cactusList[i].z;
                float h = terrain.get_height(cX, cZ);

                glm::mat4 m = glm::mat4(1.0f);
                m[3] = glm::vec4(cX, h, cZ, 1.0f);

                float rad = glm::radians(cactusList[i].rot);
                glm::mat4 rot(1.0f);
                rot[0][0] = cos(rad); rot[0][2] = -sin(rad); rot[2][0] = sin(rad); rot[2][2] = cos(rad);
                m = m * rot;

                m[1][1] = cactusList[i].scaleH;

                cactus->render(m, model_view_matrix_id, use_texture_id, color_uniform_id);
            }
        }

        //rocas
        if (rocks) {
            glUniform3f(color_uniform_id, 0.4f, 0.4f, 0.4f);
            for (size_t i = 0; i < rockPositions.size(); i++) {
                float rX = rockPositions[i].x;
                float rZ = rockPositions[i].y;
                float h = terrain.get_height(rX, rZ);

                glm::mat4 r = glm::mat4(1.0f);
                r[3] = glm::vec4(rX, h + 0.1f, rZ, 1.0f);

                float scale = 0.4f + (i % 3) * 0.1f;
                r[0][0] = scale; r[1][1] = scale * 0.8f; r[2][2] = scale;

                rocks->render(r, model_view_matrix_id, use_texture_id, color_uniform_id);
            }
        }

        //nube contransparencia
        if (cloud) {
            glEnable(GL_BLEND);
            glUniform1f(glGetUniformLocation(program_id, "alphaTransparency"), 0.7f);
            glDepthMask(GL_FALSE);

            glm::mat4 c = glm::mat4(1.0f);
            c[3] = glm::vec4(sin(SDL_GetTicks() / 3000.f) * 8.0f, 15.0f, -15.0f, 1.0f);
            c[0][0] = 6.0f; c[1][1] = 4.0f; c[2][2] = 5.0f;

            glUniform3f(color_uniform_id, 0.95f, 0.95f, 1.0f);
            cloud->render(c, model_view_matrix_id, use_texture_id, color_uniform_id);

            glDepthMask(GL_TRUE); glDisable(GL_BLEND);
            glUniform1f(glGetUniformLocation(program_id, "alphaTransparency"), 1.0f);
        }

        //postprocesado de la escena, con granulado y viñeta
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT); glDisable(GL_DEPTH_TEST);
        glUseProgram(post_program_id);

        glUniform1f(glGetUniformLocation(post_program_id, "time"), SDL_GetTicks() / 1000.0f);

        glBindTexture(GL_TEXTURE_2D, fbo_texture_id);
        glBindVertexArray(quad_vao); glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
    }
    void Scene::resize(int w, int h) { width = w; height = h; camera.set_ratio((float)w / h); glViewport(0, 0, w, h); glBindTexture(GL_TEXTURE_2D, fbo_texture_id); glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); glBindRenderbuffer(GL_RENDERBUFFER, rbo_id); glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h); }
    void Scene::handle_input(const bool* k) { if (k[SDL_SCANCODE_W]) camera.move_forward(); if (k[SDL_SCANCODE_S]) camera.move_backward(); if (k[SDL_SCANCODE_A]) camera.move_left(); if (k[SDL_SCANCODE_D]) camera.move_right(); if (k[SDL_SCANCODE_Q]) camera.move_up(); if (k[SDL_SCANCODE_E]) camera.move_down(); }
    void Scene::on_drag(float x, float y) { camera.rotate(x, y); }
}