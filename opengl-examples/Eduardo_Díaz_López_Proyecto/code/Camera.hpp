// Camera.hpp
// Camara con movimiento  W A S D y pulsar y arrastrar para mover la cámara
#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace udit {

    class Camera {
    private:
        // posiciones de la cámara
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;

        // Ángulos
        float pitch;
        float yaw;

        // Configuración de movimiento
        float movement_speed;
        float mouse_sensitivity;

        // lente
        float fov;
        float ratio;
        float near_z;
        float far_z;

    public:
       //constructor de la cámara
        Camera(float width, float height)
            : position(glm::vec3(0.0f, 2.0f, 10.0f)), //Posición inicial 
            world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
            yaw(-90.0f),
            pitch(0.0f),
            movement_speed(0.2f), //Velocidad
            mouse_sensitivity(0.1f),
            fov(45.0f),
            near_z(0.1f),
            far_z(1000.0f)
        {
            ratio = width / height;
            update_vectors();
        }

        //Matrices para el Shader
        glm::mat4 get_view_matrix() const {
            return glm::lookAt(position, position + front, up);
        }

        glm::mat4 get_projection_matrix() const {
            return glm::perspective(glm::radians(fov), ratio, near_z, far_z);
        }

        //Control de cámara
        void rotate(float xoffset, float yoffset) {
            xoffset *= mouse_sensitivity;
            yoffset *= mouse_sensitivity;

            yaw += xoffset;
            pitch -= yoffset;

            // Restricciones
            if (pitch > 89.0f)  pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            update_vectors();
        }

        //Movimiento
        void move_forward() {
            position += movement_speed * front;
        }

        void move_backward() {
            position -= movement_speed * front;
        }

        void move_left() {
            position -= glm::normalize(glm::cross(front, up)) * movement_speed;
        }

        void move_right() {
            position += glm::normalize(glm::cross(front, up)) * movement_speed;
        }

        //Mover verticalmente (opcional, útil para modo espectador)
        void move_up() {
            position += movement_speed * world_up;
        }

        void move_down() {
            position -= movement_speed * world_up;
        }

        
        glm::vec3 get_position() const { return position; }
        glm::vec3 get_front()    const { return front; }

        void set_ratio(float new_ratio) {
            ratio = new_ratio;
        }

    private:
        void update_vectors() {
            glm::vec3 new_front;
            new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            new_front.y = sin(glm::radians(pitch));
            new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

            front = glm::normalize(new_front);
            right = glm::normalize(glm::cross(front, world_up));
            up = glm::normalize(glm::cross(right, front));
        }
    };

}

#endif