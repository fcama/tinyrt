#version 440 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform uint frame;

void main()
{
    vec3 pixel_color = texture(screenTexture, TexCoords).rgb;

    // Gamma Correction + Averaging
    FragColor = vec4(sqrt(pixel_color / frame), 1);
}