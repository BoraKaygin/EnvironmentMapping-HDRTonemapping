#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrScene;
uniform float exposure;

void main() {
    vec3 meanColor = exp(textureLod(hdrScene, vec2(0.5, 0.5), 64).rgb);

    vec3 fragColor = exp(texture(hdrScene, TexCoords).rgb);

    float fragLuminance = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
    float meanLuminance = 0.2126 * meanColor.r + 0.7152 * meanColor.g + 0.0722 * meanColor.b;

    float scaledLuminance = (fragLuminance * exposure) / meanLuminance;
    float TMLuminance = scaledLuminance / (1.0 + scaledLuminance);

    vec3 toneMappedColor = clamp((TMLuminance / fragLuminance) * fragColor, vec3(0, 0, 0), vec3(1, 1, 1));

    float p = 1.0 / 2.2;
    vec3 gammaCorrected = pow(toneMappedColor, vec3(p, p, p));
    
    FragColor = vec4(gammaCorrected, 1.0);
}