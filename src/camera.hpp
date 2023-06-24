#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum camera_movement { FORWARD, BACKWARD, LEFT, RIGHT };

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
 public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 world_up;

  float yaw;
  float pitch;

  float movement_speed;
  float mouse_sensitivity;
  float zoom;

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH)
      : front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movement_speed(SPEED),
        mouse_sensitivity(SENSITIVITY),
        zoom(ZOOM) {
    this->position = position;
    this->world_up = up;
    this->yaw = yaw;
    this->pitch = pitch;

    update_camera_vectors();
  }

  Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y,
         float up_z, float yaw, float pitch)
      : front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movement_speed(SPEED),
        mouse_sensitivity(SENSITIVITY),
        zoom(ZOOM) {
    this->position = glm::vec3(pos_x, pos_y, pos_z);
    this->world_up = glm::vec3(up_x, up_y, up_z);
    this->yaw = yaw;
    this->pitch = pitch;

    update_camera_vectors();
  }

  glm::mat4 get_view_matrix() {
    return glm::lookAt(position, position + front, up);
  }

  void process_keyboard(camera_movement direction, float delta_time) {
    float velocity = movement_speed * delta_time;

    if (direction == FORWARD) {
      position += front * velocity;
    }

    if (direction == BACKWARD) {
      position -= front * velocity;
    }

    if (direction == LEFT) {
      position -= right * velocity;
    }

    if (direction == RIGHT) {
      position += right * velocity;
    }
  }

  void process_mouse_movement(float x_offset, float y_offset,
                              bool constrain_pitch = true) {
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if (constrain_pitch) {
      if (pitch > 89.0f) {
        pitch = 89.0f;
      }

      if (pitch < -89.0f) {
        pitch = -89.0f;
      }
    }

    update_camera_vectors();
  }

  void process_mouse_scroll(float y_offset) {
    zoom -= (float)y_offset;

    if (zoom < 1.0f) {
      zoom = 1.0f;
    }

    if (zoom > 45.0f) {
      zoom = 45.0f;
    }
  }

 private:
  void update_camera_vectors() {
    glm::vec3 new_front;
    new_front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    new_front.y = std::sin(glm::radians(pitch));
    new_front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    front = glm::normalize(new_front);
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
  }
};
