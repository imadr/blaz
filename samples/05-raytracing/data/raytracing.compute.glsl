#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D u_image_render;
layout(rgba32f, binding = 1) uniform image2D u_image_old_render;

layout(std140, binding = 2) uniform u_time {
    uint u_frame_number;
};

layout(std140, binding = 3) uniform u_info {
    vec3 u_camera_position;
    vec3 u_camera_target;
    float u_reset_accumulation;
};

mat3 lookat_matrix(vec3 origin, vec3 target, float roll) {
    vec3 rr = vec3(sin(roll), cos(roll), 0.0);
    vec3 ww = normalize(target - origin);
    vec3 uu = normalize(cross(ww, rr));
    vec3 vv = normalize(cross(uu, ww));
    return mat3(uu, vv, ww);
}

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 albedo;
    bool metal;
    float roughness;
};

struct Hit {
    bool did_hit;
    float t;
    vec3 point;
    vec3 normal;
    uint material_id;
};

struct Hittable {
    vec3 position;

    uint material_id;

    uint primitive_id;
    float sphere_radius;
    vec3 plane_normal;
};

vec3 at(Ray r, float t) {
    return r.origin + r.direction * t;
}

void ray_sphere_intersection(Ray ray, float ray_min, float ray_max, Hittable sphere, out Hit hit) {
    hit.did_hit = false;

    vec3 oc = sphere.position - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float b = -2.0 * dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.sphere_radius * sphere.sphere_radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return;
    }

    float sqrt_discriminant = sqrt(discriminant);
    float root = (-b - sqrt_discriminant) / (2 * a);
    if (root <= ray_min || root >= ray_max) {
        root = (-b + sqrt_discriminant) / (2 * a);
        if (root <= ray_min || root >= ray_max) {
            return;
        }
    }

    hit.did_hit = true;
    hit.t = root;
    hit.point = at(ray, hit.t);
    hit.normal = (hit.point - sphere.position) / sphere.sphere_radius;
}

void ray_plane_intersection(Ray ray, float ray_min, float ray_max, Hittable plane, out Hit hit) {
    hit.did_hit = false;
    float denom = dot(ray.direction, plane.plane_normal);
    if (abs(denom) > 0.0001) {
        float t = dot(plane.position - ray.origin, plane.plane_normal) / denom;
        if (t >= ray_min && t <= ray_max) {
            hit.t = t;
            hit.point = at(ray, hit.t);
            hit.normal = plane.plane_normal;
            hit.did_hit = true;
            return;
        }
    }
}

#define RAY_MIN 0.0001
#define RAY_MAX 999999
#define SAMPLE_PER_RAY 10
#define MAX_BOUNCES 10

float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

vec2 hash(vec2 p) {
    return vec2(hash(p.x + p.y * 57.0), hash(p.y + p.x * 57.0));
}

// [0, 1]
float random(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453123 * hash(u_frame_number));
}

vec3 random_unit_vector(vec2 uv) {
    float rand1 = random(uv);
    float rand2 = random(uv + 1.0);

    float theta = rand1 * 2.0 * 3.14159265358979323846;
    float phi = acos(2.0 * rand2 - 1.0);

    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    return normalize(vec3(x, y, z));
}

vec3 random_unit_vector_on_hemisphere(vec2 uv, vec3 normal) {
    vec3 v = random_unit_vector(uv);
    if (dot(v, normal) > 0.0) {
        return v;
    } else {
        return -v;
    }
}

bool near_zero(vec3 v) {
    float s = 1e-8;
    return (abs(v.x) < s) && (abs(v.y) < s) && (abs(v.z) < s);
}

#define PRIMITIVE_SPHERE 0
#define PRIMITIVE_PLANE 1

const uint num_materials = 4;
const Material materials[num_materials] = Material[](
    Material(vec3(0.5, 0.5, 0.5), false, 0.0), Material(vec3(1.0, 0.0, 0.0), false, 0.0),
    Material(vec3(0.0, 1.0, 0.0), false, 0.0), Material(vec3(0.0, 0.0, 1.0), false, 0.0));
#define MATERIAL_GROUND 0
#define MATERIAL_1 1
#define MATERIAL_2 2
#define MATERIAL_3 3

const uint num_hittables = 4;
const Hittable hittables[num_hittables] = Hittable[](
    Hittable(vec3(0.0, 0.0, 1.0), MATERIAL_1, PRIMITIVE_SPHERE, 0.5, vec3(0.0)),
    Hittable(vec3(1.05, 0.0, 1.0), MATERIAL_2, PRIMITIVE_SPHERE, 0.5, vec3(0.0)),
    Hittable(vec3(-1.05, 0.0, 1.0), MATERIAL_3, PRIMITIVE_SPHERE, 0.5, vec3(0.0)),
    Hittable(vec3(0.0, -0.5, 0.0), MATERIAL_GROUND, PRIMITIVE_PLANE, 0, vec3(0.0, 1.0, 0.0)));

void main() {
    ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 resolution = imageSize(u_image_render);
    vec2 uv = texel_coord / resolution.xy - 0.5;
    uv.x *= resolution.x / resolution.y;
    float fov = 2.0;
    uv *= fov;

    mat3 matrix = lookat_matrix(u_camera_position, u_camera_target, 0.0);

    vec3 final_color = vec3(0.0);
    for (int i = 0; i < SAMPLE_PER_RAY; i++) {
        vec2 offset = vec2(random(uv + vec2(i)), random(uv + random(uv) + vec2(i))) - vec2(0.5);
        offset /= resolution.xy;

        vec3 view = matrix * normalize(vec3(uv + offset, 1.0));
        Ray ray = Ray(u_camera_position, view);

        vec3 color = vec3(1.0);
        for (int j = 0; j < MAX_BOUNCES; j++) {
            Hit hit = Hit(false, RAY_MAX, vec3(0.0), vec3(0.0), 0);

            for (int k = 0; k < num_hittables; k++) {
                Hit tmp_hit = Hit(false, 0.0, vec3(0.0), vec3(0.0), 0);

                if (hittables[k].primitive_id == PRIMITIVE_SPHERE) {
                    ray_sphere_intersection(ray, RAY_MIN, hit.t, hittables[k], tmp_hit);
                } else if (hittables[k].primitive_id == PRIMITIVE_PLANE) {
                    ray_plane_intersection(ray, RAY_MIN, hit.t, hittables[k], tmp_hit);
                }

                if (tmp_hit.did_hit) {
                    tmp_hit.material_id = hittables[k].material_id;
                    hit = tmp_hit;
                }
            }

            if (hit.did_hit) {
                Material hit_material = materials[hit.material_id];
                if (hit_material.metal) {
                    vec3 reflected_direction = normalize(reflect(ray.direction, hit.normal)) +
                                               hit_material.roughness * random_unit_vector(uv);
                    ray = Ray(hit.point, reflected_direction);

                    if (dot(ray.direction, hit.normal) > 0) {
                        color *= hit_material.albedo;
                    }
                } else {
                    vec3 scatter_direction = hit.normal + random_unit_vector(uv);

                    if (near_zero(scatter_direction)) {
                        scatter_direction = hit.normal;
                    }

                    ray = Ray(hit.point, scatter_direction);
                    color *= hit_material.albedo;
                }
            } else {
                color *= vec3(0.5, 0.7, 1.0);
                break;
            }
        }

        final_color += color;
    }

    final_color /= SAMPLE_PER_RAY;
    final_color = pow(final_color, vec3(1.0 / 2.2));

    vec4 old_render = imageLoad(u_image_old_render, ivec2(texel_coord));
    if (u_reset_accumulation == 1.0) {
        old_render = vec4(0.0);
    }
    imageStore(u_image_render, texel_coord, vec4(final_color, 1.0) + old_render);
}