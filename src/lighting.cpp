#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);
unsigned int load_texture(char const* path);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
bool first_mouse = true;
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;

float delta_time = 0.0f;
float last_frame_time = 0.0f;

glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting", NULL, NULL);

  if (!window) {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();

    return -1;
  }

  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialise GLAD\n";

    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  Shader object_shader("src/shader/lighting_object.vs", "src/shader/lighting_object.fs");
  Shader light_source_shader("src/shader/lighting_source.vs", "src/shader/lighting_source.fs");

  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };

  glm::vec3 cube_positions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };

  glm::vec3 point_light_positions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
  };

  unsigned int VBO, object_VAO;
  glGenVertexArrays(1, &object_VAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(object_VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  unsigned int light_source_VAO;
  glGenVertexArrays(1, &light_source_VAO);
  glBindVertexArray(light_source_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  unsigned int diffuse_map = load_texture("data/container2.png");
  unsigned int specular_map = load_texture("data/container2_specular.png");

  object_shader.use();
  object_shader.set_uniform_int("material.diffuse", 0);
  object_shader.set_uniform_int("material.specular", 1);

  while (!glfwWindowShouldClose(window)) {
    float current_frame_time = static_cast<float>(glfwGetTime());
    delta_time = current_frame_time - last_frame_time;
    last_frame_time = current_frame_time;

    process_input(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    object_shader.use();
    object_shader.set_uniform_vec3("view_pos", camera.position);
    object_shader.set_uniform_float("material.shininess", 32.0f);

    // Directional Light
    object_shader.set_uniform_vec3("dir_light.direction", -0.2f, -1.0f, -0.3f);
    object_shader.set_uniform_vec3("dir_light.ambient", 0.05f, 0.05f, 0.05f);
    object_shader.set_uniform_vec3("dir_light.diffuse", 0.4f, 0.4f, 0.4f);
    object_shader.set_uniform_vec3("dir_light.specular", 0.5f, 0.5f, 0.5f);

    // Point Lights
    object_shader.set_uniform_vec3("point_lights[0].position", point_light_positions[0]);
    object_shader.set_uniform_vec3("point_lights[0].ambient", 0.05f, 0.05f, 0.05f);
    object_shader.set_uniform_vec3("point_lights[0].diffuse", 0.8f, 0.8f, 0.8f);
    object_shader.set_uniform_vec3("point_lights[0].specular", 1.0f, 1.0f, 1.0f);
    object_shader.set_uniform_float("point_lights[0].constant", 1.0f);
    object_shader.set_uniform_float("point_lights[0].linear", 0.09f);
    object_shader.set_uniform_float("point_lights[0].quadratic", 0.032f);

    object_shader.set_uniform_vec3("point_lights[1].position", point_light_positions[1]);
    object_shader.set_uniform_vec3("point_lights[1].ambient", 0.05f, 0.05f, 0.05f);
    object_shader.set_uniform_vec3("point_lights[1].diffuse", 0.8f, 0.8f, 0.8f);
    object_shader.set_uniform_vec3("point_lights[1].specular", 1.0f, 1.0f, 1.0f);
    object_shader.set_uniform_float("point_lights[1].constant", 1.0f);
    object_shader.set_uniform_float("point_lights[1].linear", 0.09f);
    object_shader.set_uniform_float("point_lights[1].quadratic", 0.032f);

    object_shader.set_uniform_vec3("point_lights[2].position", point_light_positions[2]);
    object_shader.set_uniform_vec3("point_lights[2].ambient", 0.05f, 0.05f, 0.05f);
    object_shader.set_uniform_vec3("point_lights[2].diffuse", 0.8f, 0.8f, 0.8f);
    object_shader.set_uniform_vec3("point_lights[2].specular", 1.0f, 1.0f, 1.0f);
    object_shader.set_uniform_float("point_lights[2].constant", 1.0f);
    object_shader.set_uniform_float("point_lights[2].linear", 0.09f);
    object_shader.set_uniform_float("point_lights[2].quadratic", 0.032f);

    object_shader.set_uniform_vec3("point_lights[3].position", point_light_positions[3]);
    object_shader.set_uniform_vec3("point_lights[3].ambient", 0.05f, 0.05f, 0.05f);
    object_shader.set_uniform_vec3("point_lights[3].diffuse", 0.8f, 0.8f, 0.8f);
    object_shader.set_uniform_vec3("point_lights[3].specular", 1.0f, 1.0f, 1.0f);
    object_shader.set_uniform_float("point_lights[3].constant", 1.0f);
    object_shader.set_uniform_float("point_lights[3].linear", 0.09f);
    object_shader.set_uniform_float("point_lights[3].quadratic", 0.032f);

    // Spot Light
    object_shader.set_uniform_vec3("spot_light.position", camera.position);
    object_shader.set_uniform_vec3("spot_light.direction", camera.front);
    object_shader.set_uniform_vec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
    object_shader.set_uniform_vec3("spot_light.diffuse", 1.0f, 1.0f, 1.0f);
    object_shader.set_uniform_vec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
    object_shader.set_uniform_float("spot_light.constant", 1.0f);
    object_shader.set_uniform_float("spot_light.linear", 0.09f);
    object_shader.set_uniform_float("spot_light.quadratic", 0.032f);
    object_shader.set_uniform_float("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
    object_shader.set_uniform_float("spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));

    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.get_view_matrix();
    object_shader.set_uniform_mat4("projection", projection);
    object_shader.set_uniform_mat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    object_shader.set_uniform_mat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_map);

    glBindVertexArray(object_VAO);

    for (unsigned int i = 0; i < 10; i++) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cube_positions[i]);
      float angle = 20.0f * i;
      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      object_shader.set_uniform_mat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    light_source_shader.use();
    light_source_shader.set_uniform_mat4("projection", projection);
    light_source_shader.set_uniform_mat4("view", view);

    glBindVertexArray(light_source_VAO);

    for (unsigned int i = 0; i < 4; i++) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, point_light_positions[i]);
      model = glm::scale(model, glm::vec3(0.2f));

      light_source_shader.set_uniform_mat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &object_VAO);
  glDeleteVertexArrays(1, &light_source_VAO);
  glDeleteBuffers(1, &VBO);
  object_shader.delete_program();
  light_source_shader.delete_program();

  glfwTerminate();

  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glfwSetWindowAspectRatio(window, SCR_WIDTH, SCR_HEIGHT);
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x_pos_in, double y_pos_in) {
  float x_pos = static_cast<float>(x_pos_in);
  float y_pos = static_cast<float>(y_pos_in);

  if (first_mouse) {
    last_x = x_pos;
    last_y = y_pos;
    first_mouse = false;
  }

  float x_offset = x_pos - last_x;
  float y_offset = last_y - y_pos;
  last_x = x_pos;
  last_y = y_pos;

  camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
  camera.process_mouse_scroll(static_cast<float>(y_offset));
}

void process_input(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.process_keyboard(FORWARD, delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.process_keyboard(BACKWARD, delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.process_keyboard(LEFT, delta_time);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.process_keyboard(RIGHT, delta_time);
  }
}

unsigned int load_texture(char const* path) {
  unsigned int texture_ID;
  glGenTextures(1, &texture_ID);

  int width, height, n_components;
  unsigned char* data = stbi_load(path, &width, &height, &n_components, 0);

  if (data) {
    GLenum format;

    if (n_components == 1) {
      format = GL_RED;
    } else if (n_components == 3) {
      format = GL_RGB;
    } else if (n_components == 4) {
      format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cerr << "Texture failed to load at path: " << path << "\n";
    stbi_image_free(data);
  }

  return texture_ID;
}
