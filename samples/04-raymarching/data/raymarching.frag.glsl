#version 450

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 0) uniform u_info {
    vec2 u_resolution;
};

#define PI 3.14159265359

mat3 lookat_matrix(vec3 origin, vec3 target, float roll) {
    vec3 rr = vec3(sin(roll), cos(roll), 0.0);
    vec3 ww = normalize(target - origin);
    vec3 uu = normalize(cross(ww, rr));
    vec3 vv = normalize(cross(uu, ww));
    return mat3(uu, vv, ww);
}

struct Transform {
    vec3 pos;
    vec3 rot;
};

mat3 rotateX(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat3(1.0, 0.0, 0.0, 0.0, c, -s, 0.0, s, c);
}

mat3 rotateY(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat3(c, 0.0, s, 0.0, 1.0, 0.0, -s, 0.0, c);
}

mat3 rotateZ(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat3(c, -s, 0.0, s, c, 0.0, 0.0, 0.0, 1.0);
}

vec3 rotate(vec3 p, vec3 angles) {
    mat3 m = rotateX(angles.x) * rotateY(angles.y) * rotateZ(angles.z);
    return p * m;
}

vec3 transform(vec3 p, Transform t) {
    vec3 p_ = p - t.pos;
    p_ = rotate(p_, t.rot);
    return p_;
}

vec2 sphere(vec3 point, float radius, float id) {
    return vec2(length(point) - radius, id);
}

vec2 plane(vec3 point, vec3 normal, float distance, float id) {
    return vec2(dot(point, normal) + distance, id);
}

vec2 vec_min(vec2 a, vec2 b) {
    return a.x > b.x ? b : a;
}

#define SPHERE_PRIMITIVE 0
#define PLANE_PRIMITIVE 1

struct PrimitiveInfo {
    float sphere_radius;
    float plane_distance;
    vec3 plane_normal;
};

struct Object {
    int id;
    int primitive;
    Transform transform;
    vec3 color;
    PrimitiveInfo primitive_info;
};

vec2 primitive(vec3 point, Object object) {
    if (object.primitive == SPHERE_PRIMITIVE) {
        return sphere(transform(point, object.transform), object.primitive_info.sphere_radius,
                      object.id);
    } else if (object.primitive == PLANE_PRIMITIVE) {
        return plane(point, object.primitive_info.plane_normal,
                     object.primitive_info.plane_distance, object.id);
    }
}

#define NUM_OBJECTS 3
#define PLANE_1 1
#define SPHERE_1 2
#define SPHERE_2 3
const Object objects[NUM_OBJECTS] = {
    Object(PLANE_1, PLANE_PRIMITIVE, Transform(vec3(0, 0, 0), vec3(0, 0, 0)), vec3(0.6),
           PrimitiveInfo(0.0, 2.0, vec3(0.0, 1.0, 0.0))),
    Object(SPHERE_1, SPHERE_PRIMITIVE, Transform(vec3(0, 0, 8), vec3(0, 0, 0)), vec3(1.0, 0.0, 0.0),
           PrimitiveInfo(1.0, 0.0, vec3(0.0))),
    Object(SPHERE_2, SPHERE_PRIMITIVE, Transform(vec3(-2, 0, 8), vec3(0, 0, 0)),
           vec3(1.0, 0.0, 1.0), PrimitiveInfo(1.0, 0.0, vec3(0.0)))};

vec2 scene(vec3 point) {
    vec2 min_ = primitive(point, objects[0]);

    for (int i = 1; i < NUM_OBJECTS; i++) {
        min_ = vec_min(min_, primitive(point, objects[i]));
    }

    return min_;
}

vec2 march(vec3 ray_origin, vec3 ray_direction) {
    vec3 current_point = ray_origin;
    float dist_total = 0.;
    float id;
    for (int i = 0; i < 3000; i++) {
        current_point = ray_origin + ray_direction * dist_total;
        vec2 s = scene(current_point);
        id = s.y;
        float dist = s.x;
        dist_total += dist;
        if (dist < 0.00001) {
            break;
        }
        if (dist_total > 1000.) {
            id = 0.;
            break;
        }
    }
    return vec2(dist_total, id);
}

vec3 normal(vec3 point) {
    if (point.z > 1000.) return vec3(0.);
    float dist = scene(point).x;
    float delta = 0.00001;
    vec2 dir = vec2(delta, 0.);
    float dx = scene(point + dir.xyy).x - dist;
    float dy = scene(point + dir.yxy).x - dist;
    float dz = scene(point + dir.yyx).x - dist;
    return normalize(vec3(dx, dy, dz));
}

float lighting(vec3 point, vec3 normal, vec3 light_pos) {
    vec3 direction_to_light = normalize(light_pos);
    if (march(point + normal * 0.001, direction_to_light).x < length(light_pos - point)) {
        return 0.;
    }
    return max(0., dot(normal, direction_to_light));
}

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution.xy - 0.5;
    uv.x *= u_resolution.x / u_resolution.y;

    vec3 camera_pos = vec3(0, 0, 0);
    vec3 camera_target = vec3(0, 0, 1);

    mat3 matrix = lookat_matrix(camera_pos, camera_target, 0.0);
    vec3 view = matrix * normalize(vec3(uv, 1.0));

    vec2 march_out = march(camera_pos, view);
    float depth = march_out.x;
    vec3 col = objects[int(march_out.y) - 1].color;

    if (march_out.y == 0.0) {
        o_color = mix(vec4(0.80, 0.94, 0.99, 1.), vec4(0., 0.72, 0.96, 1.), uv.y * 3.);
        return;
    }

    vec3 point = camera_pos + view * depth;
    vec3 norm = normal(point);
    vec3 light_position = vec3(0., 2.0, 0.0);
    float directional_light = lighting(point, norm, light_position);

    vec3 light = vec3(0);

    vec3 ambient_light = vec3(0.3);
    light += ambient_light + directional_light;

    col *= light;

    o_color = vec4(col, 1.);
}