#version 430 core

precision highp float;

in vec3 v_world_position;
in vec4 v_light_space_position;
in vec3 v_normal;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 1) uniform u_light {
    vec3 u_light_position;
    mat4 u_light_view_mat;
    mat4 u_light_projection_mat;
};

layout(binding = 2) uniform sampler2D u_sampler_shadowmap;

void main() {
    vec3 light_direction = normalize(u_light_position - v_world_position);

    // float shadow = calculate_shadow(v_light_space_position, v_normal, light_direction,
    // u_sampler_shadowmap);

    // vec3 color = vec3(dot(v_normal, light_direction));
    // o_color = vec4(color * (1.0 - shadow), 1.0);

    vec3 projected_light_coords = v_light_space_position.xyz / v_light_space_position.w;
    // if (projected_light_coords.x < -1.0 || projected_light_coords.x > 1.0 ||
    //     projected_light_coords.y < -1.0 || projected_light_coords.y > 1.0 ||
    //     projected_light_coords.z < 0.0 || projected_light_coords.z > 1.0) {
    //     o_color = vec4(1.0, 0.0, 1.0, 1.0);
    //     // return;
    // }

    projected_light_coords = projected_light_coords * 0.5 + 0.5;

    vec2 texel_size = 1.0 / textureSize(u_sampler_shadowmap, 0);
//     float slope = abs(dot(normalize(v_normal), light_direction));
//     float bias = 0.00005 * slope + 0.00002;

//     float blocker_sum = 0.0;
//     int num_blockers = 0;

//     float light_size = 30.0;

//     float search_width = light_size;

//     for (int i = 0; i <= 32; i++) {
//         vec2 offset = POISSON32[i] * search_width * texel_size;
//         float shadowmap_sample =
//             texture(u_sampler_shadowmap, projected_light_coords.xy + offset).r;

//         if (projected_light_coords.z > shadowmap_sample) {
//             blocker_sum += shadowmap_sample;
//             num_blockers++;
//         }
//     }


//     float avg_blocker_depth = blocker_sum / num_blockers;

//     float penumbra_size = light_size*(projected_light_coords.z-avg_blocker_depth)/avg_blocker_depth;
// penumbra_size *= 0.1;
//     float shadow = 0.0;


//     for (int i = 0; i <= 32; i++) {
//         vec2 offset = POISSON32[i] * penumbra_size;
//         float shadowmap_sample =
//             texture(u_sampler_shadowmap, projected_light_coords.xy + offset).r;
//         if (projected_light_coords.z > shadowmap_sample + bias) {
//             shadow += 1.0;
//         }
//     }
//     shadow /= 32;

//     // o_color = vec4(vec3(1.0 - shadow), 1.0);

//     vec3 color = vec3(dot(v_normal, light_direction));
//     o_color = vec4(color * (1.0 - shadow), 1.0);

    vec3 color = vec3(dot(v_normal, light_direction));

    float shadow = 0.0;
// if(false){
//     float shadowmap_sample =
//             texture(u_sampler_shadowmap, projected_light_coords.xy).r;

//     if(shadowmap_sample > projected_light_coords.z - 0.00001){
//         shadow = 1.0;

//     }
//     else{
//         shadow = 0.0;
//     }
// }
    float bias = 0.00001;
    float kernel_scale = 10.0;
    int sample_size = 1;


    float average_blockers_depth = 0.0;
    float num_blockers = 0.0;
    for (int i = -sample_size; i <= sample_size; i++) {
        for (int j = -sample_size; j <= sample_size; j++) {
            vec2 offset = vec2(i, j) * texel_size * kernel_scale;
            float shadowmap_sample =
                texture(u_sampler_shadowmap, projected_light_coords.xy + offset).r;
            if (shadowmap_sample < projected_light_coords.z - bias) {
                average_blockers_depth += shadowmap_sample;
                num_blockers += 1.0;
            }
        }
    }

    average_blockers_depth /= num_blockers;
    o_color = vec4(vec3(texture(u_sampler_shadowmap, projected_light_coords.xy).r/5.0), 1.0);


    // o_color = vec4(vec3(average_blockers_depth/3.0), 1.0);

    // for (int i = -sample_size; i <= sample_size; i++) {
    //     for (int j = -sample_size; j <= sample_size; j++) {
    //         vec2 offset = vec2(i, j) * texel_size * kernel_scale;
    //         float shadowmap_sample =
    //             texture(u_sampler_shadowmap, projected_light_coords.xy + offset).r;
    //         if (shadowmap_sample > projected_light_coords.z - bias) {
    //             shadow += 1.0;
    //         }
    //     }
    // }
    // shadow /= (sample_size+sample_size+1)*(sample_size+sample_size+1);

    // o_color = vec4(vec3(color * shadow), 1.0);
}