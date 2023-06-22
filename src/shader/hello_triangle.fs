#version 330 core

in vec3 my_color;

out vec4 frag_color;

void main() {
  frag_color = vec4(my_color, 1.0);
}
