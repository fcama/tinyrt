#version 440 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, vec2(TexCoords.x, -TexCoords.y));
    //FragColor = vec4(1, 1, 1, 1);
}