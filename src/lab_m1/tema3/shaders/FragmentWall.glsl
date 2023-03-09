#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform int lightNumber;
uniform vec3 pointLight_position[9];
uniform vec3 eye_position;

uniform vec3 pointLight_color[9];
uniform int material_shininess;
uniform vec3 pointLight_intensity;

// TODO(student): Declare any other uniforms


// Output
layout(location = 0) out vec4 out_color;

vec3 computeSpecular(vec3 ks, vec3 intensity, vec3 V, vec3 R, int shiny) {
    vec3 result;
    float max_value = pow(max(dot(V, R), 0), shiny);

    result.x = ks.x * intensity.x;
    result.y = ks.y * intensity.y;
    result.z = ks.z * intensity.z;

    result = result * max_value;

    return result;
}

vec3 computeAllSpecular() {
    vec3 final;
    for (int i = 0; i < lightNumber; i++) {
        float atenuare = 0.0f;

        if (distance(pointLight_position[i], world_position) < 1.5f) {
            atenuare = pow(1.5f - distance(pointLight_position[i], world_position), 2);
        }

        vec3 L = normalize(pointLight_position[i] - world_position);
        vec3 V = normalize(eye_position - world_position);
        vec3 R = normalize(reflect(-L, world_normal));
        vec3 specular_tmp = vec3(0);

        if (dot(L, normalize(world_normal)) > 0) {
            specular_tmp = computeSpecular(pointLight_color[i], pointLight_intensity, V, R, material_shininess);
        }

        final += specular_tmp * atenuare;
    }

    return final;
}

vec3 computeDifuse(vec3 kd, vec3 intensity, vec3 L, vec3 normal) {
    float max_value = max(dot(normal, L), 0);
    vec3 result;

    result.x = kd.x * intensity.x;
    result.y = kd.y * intensity.y;
    result.z = kd.z * intensity.z;
    result = result * max_value;

    return result;
}

vec3 computeAllDifuse() {
    vec3 final;
    for (int i = 0; i < lightNumber; i++) {
        float atenuare = 0.0f;
        
        if (distance(pointLight_position[i], world_position) < 1.5f) {
            atenuare = pow(1.5f - distance(pointLight_position[i], world_position), 2);
        }
        
        vec3 difuse_tmp = computeDifuse(pointLight_color[i], pointLight_intensity, normalize(pointLight_position[i] - world_position), normalize(world_normal));

        final += difuse_tmp * atenuare;
    }

    return final;
}

void main()
{
    // TODO(student): Define ambient, diffuse and specular light components

    vec3 lumina = vec3(0);
    lumina = computeAllDifuse() + computeAllSpecular();

    out_color = vec4(lumina, 1);
}
