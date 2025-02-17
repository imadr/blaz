#version 430 core

precision highp float;

layout(location = 0) in vec3 v_view_position;
layout(location = 1) in vec3 v_world_position;
layout(location = 2) in vec3 v_world_normal;
layout(location = 3) in vec3 v_position;

layout(location = 0) out float o_color;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

layout(binding = 1) uniform sampler2D u_sampler_gbuffer_position;

uint next_random(inout uint rng_state) {
    rng_state = rng_state * 747796405u + 2891336453u;
    uint result = ((rng_state >> ((rng_state >> 28) + 4u)) ^ rng_state) * 277803737u;
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

#define SSAO_NUM_SAMPLES 16
#define SSAO_BIAS 0.2
#define SSAO_RADIUS 0.6
#define SSAO_INTENSITY 2.0

void main() {
    uint rng_state = uint(gl_FragCoord.x) * 984894 + uint(gl_FragCoord.y) * 184147;

    vec3 normal = normalize(v_world_normal);

    vec4 view_space = (u_view_mat * vec4(v_world_position, 1.0));

    float occlusion = 0.0;
    for (int i = 0; i < SSAO_NUM_SAMPLES; i++) {
        vec3 random_direction = random_unit_vector_on_hemisphere(rng_state, normal);

        vec3 world_space_rand = v_world_position + random_direction * SSAO_RADIUS;

        vec4 view_space_rand = (u_view_mat * vec4(world_space_rand, 1.0));
        vec4 clip_space_rand = u_projection_mat * view_space_rand;
        vec3 screen_space_rand = clip_space_rand.xyz / clip_space_rand.w;
        screen_space_rand.xyz = screen_space_rand.xyz * 0.5 + 0.5;

        float sample_depth = texture(u_sampler_gbuffer_position, screen_space_rand.xy).z;

        float range_check = smoothstep(0.0, 1.0, SSAO_RADIUS / abs(view_space.z - sample_depth));
        occlusion += (sample_depth >= view_space_rand.z + SSAO_BIAS ? 1.0 : 0.0) * range_check;
    }
    occlusion /= SSAO_NUM_SAMPLES;
    o_color = clamp(1.0 - occlusion * SSAO_INTENSITY, 0.0, 1.0);
}