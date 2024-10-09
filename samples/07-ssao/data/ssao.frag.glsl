#version 430 core

precision highp float;

layout(location = 0) in vec4 v_clip_position;
layout(location = 1) in vec3 v_world_position;
layout(location = 2) in vec3 v_world_normal;
layout(location = 3) in vec3 v_world_tangent;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

layout(binding = 1) uniform sampler2D u_sampler_depth;

uint next_random(inout uint rng_state) {
    rng_state = rng_state * 747796405 + 2891336453;
    uint result = ((rng_state >> ((rng_state >> 28) + 4)) ^ rng_state) * 277803737;
    result = (result >> 22) ^ result;
    return result;
}

float random(inout uint rng_state) {
    return next_random(rng_state) / 4294967295.0;
}

vec3 random_unit_vector(inout uint rng_state) {
    float rand1 = random(rng_state);
    float rand2 = random(rng_state);

    float theta = rand1 * 2.0 * 3.14159265358979323846;
    float phi = acos(2.0 * rand2 - 1.0);

    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    return normalize(vec3(x, y, z));
}

vec3 random_unit_vector_on_hemisphere(inout uint rng_state, vec3 normal) {
    vec3 v = random_unit_vector(rng_state);
    if (dot(v, normal) > 0.0) {
        return v;
    } else {
        return -v;
    }
}

float linearize_depth(float depth) {
    float z_near = 0.1;
    float z_far = 2000.0;
    return (2.0 * z_near) / (z_far + z_near - depth * (z_far - z_near));
}

#define NUM_SAMPLES 64
#define SSAO_BIAS 0.0001
#define SSAO_RADIUS 0.05

void main() {
    uint rng_state = uint(gl_FragCoord.x) * 984894 + uint(gl_FragCoord.y) * 184147;

    vec3 normal = normalize(v_world_normal);

    vec3 clip_position = v_clip_position.xyz / v_clip_position.w;
    clip_position.xyz = clip_position.xyz * 0.5 + 0.5;


    float occlusion = 0.0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        vec3 random_direction = random_unit_vector_on_hemisphere(rng_state, normal);

        vec3 world_space_rand = v_world_position + random_direction * SSAO_RADIUS;

        vec4 view_space_rand = (u_view_mat * vec4(world_space_rand, 1.0));
        vec4 clip_space_rand = u_projection_mat * view_space_rand;
        clip_space_rand.xyz /= clip_space_rand.w;
        clip_space_rand.xyz = clip_space_rand.xyz * 0.5 + 0.5;

        float sample_depth = texture(u_sampler_depth, clip_space_rand.xy).x;


float range_check = (abs(clip_position.z - sample_depth) < 0.003) ? 1.0 : 0.0;
        o_color = vec4(vec3(range_check), 1.0);
return;
        occlusion += (clip_space_rand.z > sample_depth + SSAO_BIAS ? 1.0 : 0.0);
    }
    occlusion /= NUM_SAMPLES;

    o_color = vec4(vec3(1.0-occlusion), 1.0);
}