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

#define LAMBERTIAN 0
#define METALLIC 1
#define DIELECTRIC 2

struct Material {
    vec3 albedo;
    uint type;
    float roughness;
    vec3 emissive;
};

struct Hit {
    bool did_hit;
    float t;
    vec3 point;
    vec3 normal;
    uint material_id;
};

struct Triangle {
    vec3 v1;
    vec3 v2;
    vec3 v3;
};

struct Hittable {
    vec3 position;

    uint material_id;

    uint primitive_id;
    float sphere_radius;
    vec3 triangle_normal;
    Triangle triangle;
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

void ray_triangle_intersection(Ray ray, float ray_min, float ray_max, Hittable triangle,
                               out Hit hit) {
    hit.did_hit = false;

    vec3 v0 = triangle.triangle.v1 + triangle.position;
    vec3 v1 = triangle.triangle.v2 + triangle.position;
    vec3 v2 = triangle.triangle.v3 + triangle.position;

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);

    if (a < 1e-8) {
        return;
    }

    float f = 1.0 / a;
    vec3 s = ray.origin - v0;
    float u = f * dot(s, h);

    if (u < 0.0 || u > 1.0) {
        return;
    }

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);

    if (v < 0.0 || u + v > 1.0) {
        return;
    }

    float t = f * dot(edge2, q);
    if (t < ray_min || t > ray_max) {
        return;
    }

    hit.did_hit = true;
    hit.t = t;
    hit.point = ray.origin + t * ray.direction;
    hit.normal = triangle.triangle_normal;
}

#define RAY_MIN 0.0001
#define RAY_MAX 999999
#define SAMPLE_PER_RAY 1
#define MAX_BOUNCES 50

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

bool near_zero(vec3 v) {
    float s = 1e-8;
    return (abs(v.x) < s) && (abs(v.y) < s) && (abs(v.z) < s);
}

#define PRIMITIVE_SPHERE 0
#define PRIMITIVE_TRIANGLE 1

const uint num_materials = 6;
const Material materials[num_materials] = Material[](
    Material(vec3(0.5), LAMBERTIAN, 0, vec3(0)), Material(vec3(1, 0, 0), LAMBERTIAN, 0, vec3(0)),
    Material(vec3(1), METALLIC, 1.0, vec3(0)), Material(vec3(1), METALLIC, 0.5, vec3(0)),
    Material(vec3(1), METALLIC, 0, vec3(0)), Material(vec3(1), DIELECTRIC, 0, vec3(0)));
#define MATERIAL_WHITE 0
#define MATERIAL_LAMBERTIAN 1
#define MATERIAL_MIRROR1 2
#define MATERIAL_MIRROR2 3
#define MATERIAL_MIRROR3 4
#define MATERIAL_DIELECTRIC 5

const Triangle default_triangle = Triangle(vec3(0), vec3(0), vec3(0));

const uint num_hittables = 7;
const Hittable hittables[num_hittables] =
    Hittable[](Hittable(vec3(0, 0, 0), MATERIAL_WHITE, PRIMITIVE_TRIANGLE, 0, vec3(0, 1, 0),
                        Triangle(vec3(10, 0, 10), vec3(10, 0, -10), vec3(-10, 0, -10))),
               Hittable(vec3(0, 0, 0), MATERIAL_WHITE, PRIMITIVE_TRIANGLE, 0, vec3(0, 1, 0),
                        Triangle(vec3(-10.0, 0, -10), vec3(-10, 0, 10), vec3(10, 0, 10))),

               Hittable(vec3(2 * 1.2 - 3, 0.5, 0), MATERIAL_LAMBERTIAN, PRIMITIVE_SPHERE, 0.5,
                        vec3(0), default_triangle),
               Hittable(vec3(1 * 1.2 - 3, 0.5, 0), MATERIAL_MIRROR1, PRIMITIVE_SPHERE, 0.5, vec3(0),
                        default_triangle),
               Hittable(vec3(0 * 1.2 - 3, 0.5, 0), MATERIAL_MIRROR2, PRIMITIVE_SPHERE, 0.5, vec3(0),
                        default_triangle),
               Hittable(vec3(3 * 1.2 - 3, 0.5, 0), MATERIAL_MIRROR3, PRIMITIVE_SPHERE, 0.5, vec3(0),
                        default_triangle),
               Hittable(vec3(4 * 1.2 - 3, 0.5, 0), MATERIAL_DIELECTRIC, PRIMITIVE_SPHERE, 0.5,
                        vec3(0), default_triangle));

vec3 sky(vec3 dir) {
    vec3 sky_gradient =
        mix(vec3(1.0, 1.0, 1.0), vec3(0.08, 0.37, 0.73), pow(smoothstep(-0.3, 0.4, dir.y), 0.55));
    float sun = pow(max(0.0, dot(dir, normalize(vec3(1.0, 2.0, 3.0)))), 600.0) * 3000.0;
    return sky_gradient + sun;
}

float reflectance(float cosine, float refraction_index) {
    float r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

void main() {
    ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 resolution = imageSize(u_image_render);
    vec2 uv = texel_coord / vec2(resolution.xy) - 0.5;
    uv.x *= float(resolution.x) / float(resolution.y);
    float fov = 2.0;
    uv *= fov;

    mat3 matrix = lookat_matrix(u_camera_position, u_camera_target, 0.0);

    uint rng_state = texel_coord.y * resolution.x + texel_coord.x + u_frame_number * 1281565;

    vec3 final_color = vec3(0.0);
    for (int i = 0; i < SAMPLE_PER_RAY; i++) {
        vec2 offset = vec2(random(rng_state), random(rng_state)) - vec2(0.5);
        offset /= resolution.xy;

        vec3 view = matrix * normalize(vec3(uv + offset, 1.0));
        Ray ray = Ray(u_camera_position, view);

        vec3 incoming_light = vec3(0);
        vec3 ray_color = vec3(1);
        for (int j = 0; j < MAX_BOUNCES; j++) {
            Hit hit = Hit(false, RAY_MAX, vec3(0.0), vec3(0.0), 0);

            for (int k = 0; k < num_hittables; k++) {
                Hit tmp_hit = Hit(false, 0.0, vec3(0.0), vec3(0.0), 0);

                if (hittables[k].primitive_id == PRIMITIVE_SPHERE) {
                    ray_sphere_intersection(ray, RAY_MIN, hit.t, hittables[k], tmp_hit);
                } else if (hittables[k].primitive_id == PRIMITIVE_TRIANGLE) {
                    ray_triangle_intersection(ray, RAY_MIN, hit.t, hittables[k], tmp_hit);
                }

                if (tmp_hit.did_hit) {
                    tmp_hit.material_id = hittables[k].material_id;
                    hit = tmp_hit;
                }
            }

            if (hit.did_hit) {
                Material hit_material = materials[hit.material_id];
                vec3 bounce_direction;

                if (hit_material.type == LAMBERTIAN) {
                    bounce_direction = normalize(hit.normal + random_unit_vector(rng_state));

                } else if (hit_material.type == METALLIC) {
                    bounce_direction =
                        normalize(reflect(ray.direction, hit.normal) +
                                  hit_material.roughness * random_unit_vector(rng_state));

                    if (dot(bounce_direction, hit.normal) <= 0) {
                        break;
                    }
                } else if (hit_material.type == DIELECTRIC) {
                    vec3 normalized_direction = normalize(ray.direction);
                    float refraction_index = 0.5;
                    if (dot(ray.direction, hit.normal) > 0.0) {
                        refraction_index = 1 / refraction_index;
                        hit.normal = -hit.normal;
                    }

                    float cos_theta = min(dot(-normalized_direction, hit.normal), 1.0);
                    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

                    bool cannot_refract = refraction_index * sin_theta > 1.0;
                    vec3 direction;

                    if (cannot_refract ||
                        reflectance(cos_theta, refraction_index) > random(rng_state)) {
                        bounce_direction = reflect(normalized_direction, hit.normal);
                    } else {
                        bounce_direction =
                            refract(normalized_direction, hit.normal, refraction_index);
                    }
                }

                ray = Ray(hit.point, bounce_direction);
                ray_color *= hit_material.albedo;
                incoming_light += hit_material.emissive * ray_color;
            } else {
                incoming_light += sky(ray.direction) * ray_color;
                break;
            }
        }

        final_color += incoming_light;
    }

    final_color /= SAMPLE_PER_RAY;
    final_color = pow(final_color, vec3(1.0 / 2.2));

    vec4 old_render = imageLoad(u_image_old_render, ivec2(texel_coord));
    if (u_reset_accumulation == 1.0) {
        old_render = vec4(0.0);
    }
    imageStore(u_image_render, texel_coord, vec4(final_color, 1.0) + old_render);
}