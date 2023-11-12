#version 330 core

layout (location = 0) in vec3 aPos; // The position variable has attribute position 0

out vec3 TexCoords; // TexCoords to be passed to the fragment shader

uniform mat4 projection; // Projection matrix
uniform mat4 view; // View matrix

void main() {
    TexCoords = aPos; // Pass the position as the texture coordinate for the cubemap
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Replace z with w for the depth to always be at maximum, preventing the skybox from being affected by the depth buffer
}
