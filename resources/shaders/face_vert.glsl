#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 instanced_position;
layout(location = 2) in vec4 instanced_tex_coord;
layout(location = 3) in vec3 instanced_normal;

out vec2 tex_coord;

uniform mat4 axolote_camera;

void main() {
    vec3 original_normal = vec3(0.0f, 0.0f, 1.0f);

    // Normalize the instance normal
    vec3 target_normal = normalize(instanced_normal);

    // Compute the rotation axis and angle
    vec3 rotation_axis = cross(original_normal, target_normal);
    float cos_theta = dot(original_normal, target_normal);
    float angle = acos(clamp(cos_theta, -1.0, 1.0));

    mat3 rotation_matrix;

    if (target_normal == vec3(0.0, 0.0, -1.0)) {
        // Special case: target is (0, 0, -1), rotate 180° around the y-axis
        rotation_matrix = mat3(
            -1.0,  0.0,  0.0,
             0.0,  1.0,  0.0,
             0.0,  0.0, -1.0
        );
    } else if (length(rotation_axis) > 0.0001) {
        // General case: apply axis-angle rotation
        rotation_axis = normalize(rotation_axis);
        float sin_theta = sin(angle);
        float cos_theta = cos(angle);

        float x = rotation_axis.x, y = rotation_axis.y, z = rotation_axis.z;
        rotation_matrix = mat3(
            cos_theta + x * x * (1.0 - cos_theta), y * x * (1.0 - cos_theta) + z * sin_theta, z * x * (1.0 - cos_theta) - y * sin_theta,
            x * y * (1.0 - cos_theta) - z * sin_theta, cos_theta + y * y * (1.0 - cos_theta), z * y * (1.0 - cos_theta) + x * sin_theta,
            x * z * (1.0 - cos_theta) + y * sin_theta, y * z * (1.0 - cos_theta) - x * sin_theta, cos_theta + z * z * (1.0 - cos_theta)
        );
    } else {
        // No rotation needed
        rotation_matrix = mat3(1.0);
    }

    // Rotate the vertex position
    vec3 current_pos = rotation_matrix * aPos + instanced_position;

    // Transform to clip space
    gl_Position = axolote_camera * vec4(current_pos, 1.0f);

    if (gl_VertexID == 0) {
        tex_coord = instanced_tex_coord.xy;
    }
    else if (gl_VertexID == 1) {
        tex_coord = instanced_tex_coord.zy;
    }
    else if (gl_VertexID == 2) {
        tex_coord = instanced_tex_coord.zw;
    }
    else {
        tex_coord = instanced_tex_coord.xw;
    }
}

