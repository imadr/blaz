#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D u_image_compute_output;

mat3 lookat_matrix(vec3 origin, vec3 target, float roll) {
    vec3 rr = vec3(sin(roll), cos(roll), 0.0);
    vec3 ww = normalize(target - origin);
    vec3 uu = normalize(cross(ww, rr));
    vec3 vv = normalize(cross(uu, ww));
    return mat3(uu, vv, ww);
}

bool ray_sphere_intersection(vec3 ray_origin, vec3 ray_direction, vec3 sphere_position,
                             float sphere_radius, out vec3 intersection) {
    vec3 dir = ray_origin - sphere_position;
    float a = dot(ray_direction, ray_direction);
    float b = 2 * dot(ray_direction, dir);
    float c = dot(dir, dir) - sphere_radius * sphere_radius;
    float delta = b * b - 4 * a * c;
    if (delta == 0) {
        intersection = ray_origin + (-0.5 * b / a) * ray_direction;
        return true;
    } else if (delta > 0) {
        float x1 = -b - sqrt(delta) / (2 * a);
        float x2 = b - sqrt(delta) / (2 * a);
        if (x1 < x2) {
            intersection = intersection = ray_origin + x1 * ray_direction;
        } else {
            intersection = intersection = ray_origin + x2 * ray_direction;
        }
        return true;

    } else {
        return false;
    }
}

void main() {
    ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 resolution = imageSize(u_image_compute_output);
    vec2 uv = texel_coord / resolution.xy - 0.5;
    uv.x *= resolution.x / resolution.y;

    // vec3 camera_position = vec3(0, 0, 8);
    // vec3 camera_target = vec3(0, 0, 0);
    // mat3 matrix = lookat_matrix(camera_position, camera_target, 0.0);
    // vec3 view = matrix * normalize(vec3(uv, 1.0));

    // vec3 ray_origin = camera_position;
    // vec3 ray_direction = view;

    // vec3 sphere_position = vec3(0.0, 0.0, 0.0);
    // float sphere_radius = 1.0;

    // vec3 intersection = vec3(0.0);

    // vec3 value = vec3(0.0);
    // if (ray_sphere_intersection(ray_origin, ray_direction, sphere_position, sphere_radius,
    //                             intersection)) {
    //     value = vec3(1.0);
    // }

    // imageStore(u_image_compute_output, texel_coord, vec4(value, 1.0));
    // imageStore(u_image_compute_output, texel_coord, vec4(uv, 0.0, 1.0));
    imageStore(u_image_compute_output, texel_coord, vec4(1.0, 0.0, 1.0, 1.0));
}