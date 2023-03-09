#version 330

// Input
in vec3 world_position;
in vec3 world_normal;
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;

uniform vec3 discoCenter;
uniform bool drawText;

// Uniforms for light properties
uniform int lightNumber;
uniform vec3 pointLight_position[9];
uniform vec3 eye_position;

uniform vec3 pointLight_color[9];
uniform int material_shininess;
uniform vec3 pointLight_intensity;
uniform int objectType;
uniform bool drawGrid;
float pointRadius;

uniform bool drawSpot;
uniform vec3 spotPositions[4];
uniform vec3 spotDirections[4];
uniform vec3 spotColor[4];
uniform vec3 intensitySpot;
// TODO(student): Declare any other uniforms
uniform float rotation_time;


// Output
layout(location = 0) out vec4 out_color;

mat4 translation(float diff) {
    return mat4(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                diff, 1, 1, 1);
}

vec3 computeSpecular(vec3 ks, vec3 intensity, vec3 V, vec3 R, int shiny) {
    vec3 result;
    float max_value = pow(max(dot(V, R), 0), shiny);

    result.x = ks.x * intensity.x;
    result.y = ks.y * intensity.y;
    result.z = ks.z * intensity.z;

    result = result * max_value;

    return result;
}

vec3 computeAllPointSpecular() {
    vec3 final;
    for (int i = 0; i < lightNumber; i++) {
        float atenuare = 0.0f;

        if (distance(pointLight_position[i], world_position) < pointRadius) {
            atenuare = pow(pointRadius - distance(pointLight_position[i], world_position), 2);
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

vec3 computeAllPointDifuse() {
    vec3 final = vec3(0);
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

vec3 computeAllSpot() {
    vec3 final = vec3(0);

    for (int i = 0; i < 4; i++) {
        vec3 new_normal = normalize(world_normal);
    
        vec3 L = normalize(spotPositions[i] - world_position);

        vec3 V = normalize(eye_position - world_position);

        vec3 R = normalize(reflect(-L, world_normal));

        vec3 diffuse_light = computeDifuse(spotColor[i], intensitySpot, L, new_normal);
        vec3 specular_light = vec3(0);

        if (dot(L, new_normal) > 0)
        {
            specular_light = computeSpecular(spotColor[i], intensitySpot, V, R, material_shininess);
        }

        float cut_off = radians(15.0);
        float spot_light = dot(-L, spotDirections[i]);
        float spot_light_limit = cos(cut_off);

        float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
        float light_att_factor = pow(linear_att, 2);

        float atenuare1 = pow(3.f - distance(spotPositions[i], world_position), 2);
        float atenuare2 = pow(4.f - distance(spotPositions[i], world_position), 2);


        if (spot_light > spot_light_limit) {
            final += (light_att_factor + atenuare1) * diffuse_light + (light_att_factor + atenuare2) * specular_light;
        }
    }

    return final;
}

void main()
{
    if (objectType == 1) {
        pointRadius = 1.5f;
    } else {
        pointRadius = 2;
    }

    out_color = vec4(0);

    vec3 lumina = vec3(0);
    
    if (drawGrid) {
        lumina += computeAllPointDifuse() + computeAllPointSpecular();
    }

    if (drawSpot) {
        lumina += computeAllSpot();
    }

    vec3 discoDir = world_position - discoCenter;
    vec2 coord;
    coord.x = (1.0 / (2 *3.14159)) * atan(discoDir.x, discoDir.z);
    coord.y = (1.0 / 3.14159) * acos(discoDir.y / length(discoDir));
    
    vec2 rotated_coord = vec2(translation(rotation_time) * vec4(coord, 1, 1));
    
    vec3 text_color1 = texture(texture_1, rotated_coord).xyz;
    vec3 text_color2 = texture(texture_2, rotated_coord).xyz;

    vec3 L = normalize(discoCenter - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 R = normalize(reflect(-L, world_normal));

    float atenuareDisco = 0.0f;
        
    if (distance(discoCenter, world_position) < 7.5f) {
        atenuareDisco = pow(7.5f - distance(discoCenter, world_position), 2);
    }

    vec3 difuseDisco = atenuareDisco * computeDifuse(text_color1, intensitySpot, L, world_normal);

    float atenuareDisco2 = 0.0f;
        
    if (distance(discoCenter, world_position) < 2.5f) {
        atenuareDisco2 = pow(2.5f - distance(discoCenter, world_position), 2);
    }

    vec3 specularDisco = vec3(0);

    if (dot(L, normalize(world_normal)) > 0) {
        specularDisco = atenuareDisco2 * computeSpecular(discoCenter, pointLight_intensity, V, R, material_shininess);
    }

    vec3 final_color_text = difuseDisco + specularDisco;

    if (drawText)
        out_color = vec4(mix(final_color_text, lumina, 0.75), 1);
    else
        out_color = vec4(lumina, 1);
}
