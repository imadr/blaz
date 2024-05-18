#version 450

layout(location = 0) in vec2 v_texcoord;

layout(location = 0) out vec4 o_color;

layout(std140, binding = 0) uniform u_info {
    vec2 u_resolution;
    vec3 u_camera_position;
    vec3 u_camera_target;
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

float sphere(vec3 point, float radius) {
    return length(point) - radius;
}

float plane(vec3 point, vec3 normal) {
    return abs(dot(point, normal));
}

float box(vec3 point, vec3 size) {
    vec3 q = abs(point) - size;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float cylinder(vec3 point, float height, float radius) {
    vec2 d = abs(vec2(length(point.xz), point.y)) - vec2(radius, height);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float torus(vec3 point, vec2 radius) {
    vec2 q = vec2(length(point.xz) - radius.x, point.y);
    return length(q) - radius.y;
}

vec2 vec_min(vec2 a, vec2 b) {
    return a.x > b.x ? b : a;
}

#define SPHERE_PRIMITIVE 0
#define PLANE_PRIMITIVE 1
#define BOX_PRIMITIVE 2
#define CYLINDER_PRIMITIVE 3
#define TORUS_PRIMITIVE 4

struct PrimitiveInfo {
    float sphere_radius;
    vec3 plane_normal;
    vec3 box_size;
    float cylinder_height;
    float cylinder_radius;
    vec2 torus_radius;
};

struct Material {
    vec3 albedo;
    vec3 reflectivity;
};

struct Object {
    int id;
    int primitive;
    Transform transform;
    PrimitiveInfo primitive_info;
    Material material;
};

vec2 primitive(vec3 point, Object object) {
    float sdf = 0.0f;
    vec3 t_point = transform(point, object.transform);
    if (object.primitive == SPHERE_PRIMITIVE) {
        sdf = sphere(t_point, object.primitive_info.sphere_radius);
    } else if (object.primitive == PLANE_PRIMITIVE) {
        sdf = plane(t_point, object.primitive_info.plane_normal);
    } else if (object.primitive == BOX_PRIMITIVE) {
        sdf = box(t_point, object.primitive_info.box_size);
    } else if (object.primitive == CYLINDER_PRIMITIVE) {
        sdf = cylinder(t_point, object.primitive_info.cylinder_height,
                       object.primitive_info.cylinder_radius);
    } else if (object.primitive == TORUS_PRIMITIVE) {
        sdf = torus(t_point, object.primitive_info.torus_radius);
    }
    return vec2(sdf, object.id);
}

#define NUM_OBJECTS 5
#define PLANE_1 1
#define SPHERE_1 2
#define BOX_1 3
#define CYLINDER_1 4
#define TORUS_1 5

Material plane_material = Material(vec3(1.0), vec3(0.0));
Material sphere_material = Material(vec3(1.0, 0.0, 0.0), vec3(0.2));
Material box_material = Material(vec3(0.0, 1.0, 0.0), vec3(0.0));
Material cylinder_material = Material(vec3(0.0, 0.0, 1.0), vec3(0.0));
Material torus_material = Material(vec3(1.0, 1.0, 1.0), vec3(0.5));

const Object objects[NUM_OBJECTS] = {
    Object(PLANE_1, PLANE_PRIMITIVE, Transform(vec3(0, -1, 0), vec3(0, 0, 0)),
           PrimitiveInfo(0.0, vec3(0.0, 1.0, 0.0), vec3(0), 0, 0, vec2(0)), plane_material),
    Object(SPHERE_1, SPHERE_PRIMITIVE, Transform(vec3(0, 0, 0), vec3(0, 0, 0)),
           PrimitiveInfo(1.0, vec3(0.0), vec3(0), 0, 0, vec2(0)), sphere_material),
    Object(BOX_1, BOX_PRIMITIVE, Transform(vec3(4, 0, 0), vec3(0, 0, 0)),
           PrimitiveInfo(0.0, vec3(0.0), vec3(1.0), 0, 0, vec2(0.0)), box_material),
    Object(CYLINDER_1, CYLINDER_PRIMITIVE, Transform(vec3(-4, 0, 0), vec3(0, 0, 0)),
           PrimitiveInfo(0.0, vec3(0.0), vec3(0.0), 1.0, 1.0, vec2(0.0)), cylinder_material),
    Object(TORUS_1, TORUS_PRIMITIVE, Transform(vec3(-4, -0.5, 4), vec3(0, 0, 0)),
           PrimitiveInfo(0.0, vec3(0.0), vec3(0.0), 0.0, 0.0, vec2(1.0, 0.5)), torus_material)};

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
    float delta = 0.001;
    vec2 dir = vec2(delta, 0.);
    float dx = scene(point + dir.xyy).x - dist;
    float dy = scene(point + dir.yxy).x - dist;
    float dz = scene(point + dir.yyx).x - dist;
    return normalize(vec3(dx, dy, dz));
}

vec3 light_direction = normalize(vec3(0.5, 2.0, 1.0));
vec3 ambient_light = vec3(0.1);

vec3 render(inout vec3 ray_origin, inout vec3 ray_direction, inout vec3 reflectivity) {
    vec2 march_out = march(ray_origin, ray_direction);

    if (march_out.y == 0.0) {  // SKY
        return vec3(0.0, 0.72, 0.96);
    }

    float depth = march_out.x;
    vec3 point = ray_origin + ray_direction * depth;
    vec3 norm = normal(point);
    vec3 reflected = reflect(ray_direction, norm);
    ray_origin = point + norm;
    ray_direction = reflected;

    vec3 color = objects[int(march_out.y) - 1].material.albedo;
    vec3 diffuse = ambient_light;
    int in_shadow = 0;
    if (march(point + norm * 0.001, light_direction).x < length(light_direction - point)) {
        in_shadow = 1;
    }

    if (in_shadow != 1) {
        diffuse += vec3(max(0.0, dot(norm, light_direction)));
    }

    if (march_out.y == 1) {  // CHECKBOARD
        vec2 tile = floor(point.xz / 3.0);
        color = clamp(vec3(mod((tile.x + tile.y), 2)), 0.5, 1.0);
    }

    reflectivity = objects[int(march_out.y) - 1].material.reflectivity;

    return color * diffuse;
}

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution.xy - 0.5;
    uv.x *= u_resolution.x / u_resolution.y;

    mat3 matrix = lookat_matrix(u_camera_position, u_camera_target, 0.0);
    vec3 view = matrix * normalize(vec3(uv, 1.0));

    vec3 ray_origin = u_camera_position;
    vec3 ray_direction = view;

    vec3 reflectivity = vec3(0.0);
    vec3 diffuse = render(ray_origin, ray_direction, reflectivity);

    vec3 bounce_reflectivity = reflectivity;
    vec3 specular = vec3(0.0);

    vec3 fil = vec3(1.0);
    for (int i = 0; i < 3; i++) {
        fil *= bounce_reflectivity;
        vec3 bounce = fil * render(ray_origin, ray_direction, bounce_reflectivity);
        specular += bounce;
    }

    vec3 final_color = diffuse * (1 - reflectivity) + specular;

    final_color = pow(final_color, vec3(1.0 / 2.2));

    o_color = vec4(final_color, 1.0);
}