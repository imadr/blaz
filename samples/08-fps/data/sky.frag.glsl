#version 430 core

layout(location = 0) out vec4 o_color;

layout(location = 0) in vec3 v_world_position;
layout(location = 1) in vec3 v_view_position;

layout(std140, binding = 1) uniform u_camera {
    vec3 u_camera_position;
    vec3 u_skydome_position;
};

layout(std140, binding = 2) uniform u_light {
    vec3 u_light_position;
    mat4 u_light_view_mat;
    mat4 u_light_projection_mat;
    vec3 u_light_color;
};

#define MAX_VALUE 999999999

vec2 ray_sphere_intersection(vec3 sphere_centre, float sphere_radius, vec3 ray_origin,
                             vec3 ray_direction) {
    vec3 offset = ray_origin - sphere_centre;
    float a = 1;
    float b = 2 * dot(offset, ray_direction);
    float c = dot(offset, offset) - sphere_radius * sphere_radius;
    float d = b * b - 4 * a * c;

    if (d > 0) {
        float s = sqrt(d);
        float dist_to_sphere_near = max(0, (-b - s) / (2 * a));
        float dist_to_sphere_far = (-b + s) / (2 * a);

        if (dist_to_sphere_far >= 0) {
            return vec2(dist_to_sphere_near, dist_to_sphere_far - dist_to_sphere_near);
        }
    }
    return vec2(MAX_VALUE, 0);
}

float atmosphere_density_at_point(vec3 sample_point, vec3 planet_center, float planet_radius,
                                  float atmosphere_radius) {
    float height_above_surface = length(sample_point - planet_center) - planet_radius;
    float height_scaled = height_above_surface / (atmosphere_radius - planet_radius);
    const float density_falloff = 2.0;
    float local_density = exp(-height_scaled * density_falloff) * (1 - height_scaled);
    return local_density;
}

float optical_depth(vec3 ray_origin, vec3 ray_direction, float ray_length, vec3 planet_center,
                    float planet_radius, float atmosphere_radius) {
    vec3 density_sample_point = ray_origin;
    const int num_optical_depth_samples = 10;
    float step_size = ray_length / (num_optical_depth_samples);
    float optical_depth = 0.0;
    for (int i = 0; i < num_optical_depth_samples; i++) {
        float local_density = atmosphere_density_at_point(density_sample_point, planet_center,
                                                          planet_radius, atmosphere_radius);
        optical_depth += local_density * step_size;
        density_sample_point += ray_direction * step_size;
    }
    return optical_depth;
}

vec3 calculate_light(vec3 ray_origin, vec3 ray_direction, float ray_length, vec3 planet_center,
                     float planet_radius, vec3 direction_to_sun, float atmosphere_radius) {
    vec3 inscatter_point = ray_origin;

    const int num_inscattering_samples = 10;
    float step_size = ray_length / (num_inscattering_samples - 1);

    vec3 total_inscattered_light = vec3(0);

    const float scattering_strength = 200000;
    const vec3 scattering_coefficients = vec3(5.802e-6, 1.35e-5, 3.31e-5) * scattering_strength;

    for (int i = 0; i < num_inscattering_samples; i++) {
        float sun_ray_length = ray_sphere_intersection(planet_center, atmosphere_radius,
                                                       inscatter_point, direction_to_sun)
                                   .y;

        float sun_ray_optical_depth =
            optical_depth(inscatter_point, direction_to_sun, sun_ray_length, planet_center,
                          planet_radius, atmosphere_radius);
        float view_ray_optical_depth =
            optical_depth(inscatter_point, -ray_direction, step_size * i, planet_center,
                          planet_radius, atmosphere_radius);

        vec3 transmittance =
            exp(-(sun_ray_optical_depth + view_ray_optical_depth) * scattering_coefficients);
        float local_density = atmosphere_density_at_point(inscatter_point, planet_center,
                                                          planet_radius, atmosphere_radius);

        total_inscattered_light +=
            local_density * transmittance * step_size * scattering_coefficients;
        inscatter_point += ray_direction * step_size;
    }

    return total_inscattered_light;
}

void main() {
    vec3 direction_to_sun = normalize(u_light_position);

    float atmosphere_radius = 1.0;
    float planet_radius = 0.7;

    vec3 ray_origin = vec3(0, 0.701, 0);
    vec3 ray_direction = normalize(v_world_position);

    float sun_size = 0.07;
    float sun_brightness = 5.0;
    float sun_intensity =
        max(0.0, 1.0 - length(ray_direction - direction_to_sun) / sun_size) * sun_brightness;

    vec2 hit_info =
        ray_sphere_intersection(u_skydome_position, atmosphere_radius, ray_origin, ray_direction);
    if (hit_info.y > 0) {
        vec3 point_in_atmosphere = ray_origin + ray_direction * (hit_info.x);
        vec3 light =
            calculate_light(point_in_atmosphere, ray_direction, hit_info.y, u_skydome_position,
                            planet_radius, direction_to_sun, atmosphere_radius);

        light += sun_intensity;
        o_color = vec4(vec3(light), 1.0);
    }
}
