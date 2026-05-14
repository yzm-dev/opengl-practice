#version 410 core

out vec4 frag_color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 view_pos;

in vec3 frag_pos;
in vec3 frag_normal;

void main() {
    // blinn-phong lighting model

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 norm = normalize(frag_normal);
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (distance * distance);
    vec3 diffuse = material.diffuse * (light.diffuse * attenuation) * diff;

    // specular
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float spec = pow(max(dot(norm, halfway_dir), 0.0), material.shininess);
    vec3 specular = material.specular * (light.specular * attenuation) * spec;

    frag_color = vec4(ambient + diffuse + specular, 1.0);
}
