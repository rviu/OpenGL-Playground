#version 330 core

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

out vec4 frag_color;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Dir_Light {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Point_Light {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Spot_Light {
  vec3 position;
  vec3 direction;
  float cut_off;
  float outer_cut_off;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#define N_POINT_LIGHTS 4

uniform vec3 view_pos;
uniform Dir_Light dir_light;
uniform Point_Light point_lights[N_POINT_LIGHTS];
uniform Spot_Light spot_light;
uniform Material material;

vec3 calc_dir_light(Dir_Light light, vec3 normal, vec3 view_dir);
vec3 calc_point_light(Point_Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 calc_spot_light(Spot_Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main() {
  vec3 norm = normalize(normal);
  vec3 view_dir = normalize(view_pos - frag_pos);

  vec3 result = calc_dir_light(dir_light, norm, view_dir);

  for (int i = 0; i < N_POINT_LIGHTS; i++) {
    result += calc_point_light(point_lights[i], norm, frag_pos, view_dir);
  }

  result += calc_spot_light(spot_light, norm, frag_pos, view_dir);

  frag_color = vec4(result, 1.0);
}

vec3 calc_dir_light(Dir_Light light, vec3 normal, vec3 view_dir) {
  vec3 light_dir = normalize(-light.direction);

  float diff = max(dot(normal, light_dir), 0.0);

  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

  return (ambient + diffuse + specular);
}

vec3 calc_point_light(Point_Light light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
  vec3 light_dir = normalize(light.position - frag_pos);

  float diff = max(dot(normal, light_dir), 0.0);

  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

  float distance = length(light.position - frag_pos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

vec3 calc_spot_light(Spot_Light light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
  vec3 light_dir = normalize(light.position - frag_pos);

  float diff = max(dot(normal, light_dir), 0.0);

  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

  float distance = length(light.position - frag_pos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  float theta = dot(light_dir, normalize(-light.direction));
  float epsilon = light.cut_off - light.outer_cut_off;
  float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;

  return (ambient + diffuse + specular);
}
