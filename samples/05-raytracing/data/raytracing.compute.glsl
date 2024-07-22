#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D u_image_compute_output;
layout(rgba32f, binding = 1) uniform image2D u_image_accumulation;

mat3 lookat_matrix(vec3 origin, vec3 target, float roll) {
    vec3 rr = vec3(sin(roll), cos(roll), 0.0);
    vec3 ww = normalize(target - origin);
    vec3 uu = normalize(cross(ww, rr));
    vec3 vv = normalize(cross(uu, ww));
    return mat3(uu, vv, ww);
}

struct Hit {
    float t;
    vec3 point;
    vec3 normal;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

#define SPHERE 0

struct Hittable {
    int primitive;
    vec3 position;
    float sphere_radius;
};

vec3 at(Ray r, float t) {
    return r.origin + r.direction * t;
}

bool ray_sphere_intersection(Ray ray, float ray_min, float ray_max, Hittable sphere, out Hit hit) {
    vec3 oc = sphere.position - ray.origin;
    float a = dot(ray.direction, ray.direction);
    float b = -2.0 * dot(ray.direction, oc);
    float c = dot(oc, oc) - sphere.sphere_radius * sphere.sphere_radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    }

    float sqrt_discriminant = sqrt(discriminant);
    float root = (-b - sqrt_discriminant) / (2 * a);
    if (root <= ray_min || root >= ray_max) {
        root = (-b + sqrt_discriminant) / (2 * a);
        if (root <= ray_min || root >= ray_max) {
            return false;
        }
    }

    hit.t = root;
    hit.point = at(ray, hit.t);
    hit.normal = (hit.point - sphere.position) / sphere.sphere_radius;
    return true;
}

#define RAY_MAX 9999
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
    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453123);
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

const int num_hittables = 2;
const Hittable hittables[num_hittables] = Hittable[](Hittable(SPHERE, vec3(0.0, 0.0, 1.0), 0.5),
                                                     Hittable(SPHERE, vec3(0.0, -100.5, 0.0), 100));

void main() {
    ivec2 texel_coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 resolution = imageSize(u_image_compute_output);
    vec2 uv = texel_coord / resolution.xy - 0.5;
    uv.x *= resolution.x / resolution.y;
    float fov = 2.0;
    uv *= fov;

    vec3 camera_position = vec3(0.0, 0.0, 0);
    vec3 camera_target = vec3(0, 0, 1.0);
    mat3 matrix = lookat_matrix(camera_position, camera_target, 0.0);

    vec3 final_color = vec3(0.0);
    for (int i = 0; i < SAMPLE_PER_RAY; i++) {
        vec2 offset = vec2(random(uv + vec2(i)), random(uv + random(uv) + vec2(i))) - vec2(0.5);
        offset /= resolution.xy;

        vec3 view = matrix * normalize(vec3(uv + offset, 1.0));
        Ray ray = Ray(camera_position, view);

        vec3 color = vec3(1.0);
        for (int j = 0; j < MAX_BOUNCES; j++) {
            float ray_min = 0.0001;
            float closest_t = RAY_MAX;
            Hit hit = Hit(0.0, vec3(0.0), vec3(0.0));
            bool hit_anything = false;
            for (int k = 0; k < num_hittables; k++) {
                if (hittables[k].primitive == SPHERE) {
                    if (ray_sphere_intersection(ray, ray_min, closest_t, hittables[k], hit)) {
                        closest_t = hit.t;
                        hit_anything = true;
                    }
                }
            }

            if (hit_anything) {
                vec3 direction = random_unit_vector_on_hemisphere(uv, hit.normal);
                ray = Ray(hit.point, direction);
                color *= 0.5;
            } else {
                color *= vec3(0.5, 0.7, 1.0);
                break;
            }
        }

        final_color += color;
    }

    final_color /= SAMPLE_PER_RAY;

    imageStore(u_image_compute_output, texel_coord, vec4(final_color, 1.0));
}