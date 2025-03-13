#version 430 core

precision highp float;

layout(location = 0) in vec3 v_view_position;
layout(location = 1) in vec3 v_world_position;
layout(location = 2) in vec3 v_world_normal;
layout(location = 3) in vec3 v_position;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 0) uniform u_mat {
    mat4 u_model_mat;
    mat4 u_view_mat;
    mat4 u_projection_mat;
};

layout(std140, binding = 1) uniform u_info {
    vec2 u_resolution;
    float u_reset_accumulation;
};

layout(std140, binding = 2) uniform u_time {
    uint u_frame_number;
};

layout(binding = 1) uniform sampler2D u_sampler_gbuffer_position;
layout(binding = 2) uniform sampler2D u_sampler_old_ao;

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

const float PI = 3.14159265359;


#define SSAO 1
#define HBAO 2

#define AO_TYPE 2

#define SSAO_NUM_SAMPLES 8
#define SSAO_BIAS 0.2
#define SSAO_RADIUS 1.0

#define HBAO_NUMBER_OF_DIRECTIONS 2
#define HBAO_NUMBER_OF_STEPS 2
#define HBAO_STEP_SIZE 0.05
#define HBAO_MAXIMUM_OCCLUSION_RADIUS 2


#define AO_INTENSITY 2.0

void main() {
    uint rng_state = uint(gl_FragCoord.x) * 984894 + uint(gl_FragCoord.y) * u_frame_number * 184147;

    vec3 world_normal = normalize(v_world_normal);
    vec3 view_normal = normalize((u_view_mat * vec4(v_world_normal, 0.0)).xyz);
    vec3 view_position = (u_view_mat * vec4(v_world_position, 1.0)).xyz;
    vec4 clip_position = u_projection_mat * vec4(view_position, 1.0);
    vec2 screen_position = (clip_position.xy / clip_position.w) * 0.5 + 0.5;

    float occlusion = 0.0;

#if AO_TYPE == SSAO

    for (int i = 0; i < SSAO_NUM_SAMPLES; i++) {
        vec3 random_direction = random_unit_vector_on_hemisphere(rng_state, world_normal);

        vec3 world_position_rand = v_world_position + random_direction * SSAO_RADIUS;

        vec4 view_position_rand = (u_view_mat * vec4(world_position_rand, 1.0));
        vec4 clip_position_rand = u_projection_mat * view_position_rand;
        vec3 screen_position_rand = clip_position_rand.xyz / clip_position_rand.w;
        screen_position_rand.xyz = screen_position_rand.xyz * 0.5 + 0.5;

        float sample_depth = texture(u_sampler_gbuffer_position, screen_position_rand.xy).z;

        float range_check = smoothstep(0.0, 1.0, SSAO_RADIUS / abs(view_position.z - sample_depth));
        occlusion += (sample_depth >= view_position_rand.z + SSAO_BIAS ? 1.0 : 0.0) * range_check;
    }
    occlusion /= SSAO_NUM_SAMPLES;

#elif AO_TYPE == HBAO

    float random_angle = random(rng_state) * 2.0 * 3.14159;
    for (int i = 0; i < HBAO_NUMBER_OF_DIRECTIONS; i++) {
        float angle = (float(i) / float(HBAO_NUMBER_OF_DIRECTIONS)) * 2.0 * 3.14159 + random_angle;
        vec2 direction = vec2(cos(angle), sin(angle));

        float maximum_sine_elevation_angle = 0.0;

        for (int j = 1; j <= HBAO_NUMBER_OF_STEPS; j++) {
            vec2 offset = direction * HBAO_STEP_SIZE * float(j) * random(rng_state);
            vec2 sample_uv_coordinates = clamp(screen_position + offset, 0.0, 1.0);

            vec3 sample_view_position = texture(u_sampler_gbuffer_position, sample_uv_coordinates).xyz;

            vec3 sample_to_position_vector = sample_view_position - view_position;
            float sample_to_position_distance = length(sample_to_position_vector);
            if (dot(sample_to_position_vector, view_normal) > 0.0 && sample_to_position_distance < HBAO_MAXIMUM_OCCLUSION_RADIUS) {
                float sine_elevation_angle = dot(sample_to_position_vector, view_normal) / sample_to_position_distance;
                if (sine_elevation_angle > maximum_sine_elevation_angle) {
                    maximum_sine_elevation_angle = sine_elevation_angle;
                }
            }
        }

        occlusion += maximum_sine_elevation_angle * maximum_sine_elevation_angle;
    }

    occlusion /= float(HBAO_NUMBER_OF_DIRECTIONS);

#endif

    vec2 uv = gl_FragCoord.xy / u_resolution;
    vec4 old_ao = texture(u_sampler_old_ao, uv);
    if (u_reset_accumulation == 1.0) {
        old_ao = vec4(0.0);
    }
    vec4 new_ao = vec4(clamp(1.0 - occlusion * AO_INTENSITY, 0.0, 1.0), 0, 0, 1.0);
    o_color = new_ao + old_ao;
}