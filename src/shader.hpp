#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

class Shader {
public:
  unsigned int ID;

  Shader(const char* vertex_shader_path, const char* fragment_shader_path) {
    std::string vertex_shader_code_str;
    std::string fragment_shader_code_str;
    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;

    vertex_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
      vertex_shader_file.open(vertex_shader_path);
      fragment_shader_file.open(fragment_shader_path);

      std::stringstream vertex_shader_stream, fragment_shader_stream;

      vertex_shader_stream << vertex_shader_file.rdbuf();
      fragment_shader_stream << fragment_shader_file.rdbuf();

      vertex_shader_file.close();
      fragment_shader_file.close();

      vertex_shader_code_str = vertex_shader_stream.str();
      fragment_shader_code_str = fragment_shader_stream.str();
    }

    catch (std::ifstream::failure& e) {
      std::cerr << "ERROR::SHADER::FILE_READ_UNSUCCESSFUL\n" << e.what() << "\n\n";
    }

    const char* vertex_shader_code = vertex_shader_code_str.c_str();
    const char* fragment_shader_code = fragment_shader_code_str.c_str();

    unsigned int vertex_shader, fragment_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    check_error(vertex_shader, "VERTEX");

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    check_error(fragment_shader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);
    check_error(ID, "PROGRAM");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }

  void use() {
    glUseProgram(ID);
  }

  void set_uniform_bool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }

  void set_uniform_int(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }

  void set_uniform_float(const std::string& name, bool value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }

  void delete_program() {
    glDeleteProgram(ID);
  }

private:
  void check_error(unsigned int shader, std::string type) {
    int success;
    char info_log[1024];

    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, info_log);
        std::cerr << "ERROR::" << type << "_SHADER_COMPILATION_ERROR\n" << info_log << "\n\n";
      }
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);

      if (!success) {
        glGetProgramInfoLog(shader, 1024, NULL, info_log);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << info_log << "\n\n";
      }
    }
  }
};
