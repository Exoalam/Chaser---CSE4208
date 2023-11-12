#version 330 core

in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube dawnSkybox;
uniform samplerCube duskSkybox;
uniform float timeOfDay; // Ranges from 0.0 (dawn) to 1.0 (dusk)

void main() {
    vec3 dawnColor = texture(dawnSkybox, TexCoords).rgb;
    vec3 duskColor = texture(duskSkybox, TexCoords).rgb;
    vec3 color = mix(dawnColor, duskColor, timeOfDay);
    FragColor = vec4(color, 1.0);
}
