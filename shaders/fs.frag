#version 440 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform uint frame;
uniform float exposure;
uniform bool tone_mapping;
uniform bool sRGB_conversion;

vec3 LessThan(vec3 f, float value)
{
    return vec3(
        (f.x < value) ? 1.0f : 0.0f,
        (f.y < value) ? 1.0f : 0.0f,
        (f.z < value) ? 1.0f : 0.0f
    );
}

vec3 LinearToSRGB(vec3 rgb)
{
    rgb = clamp(rgb, 0.0f, 1.0f);

    return mix(
        pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
        rgb * 12.92f,
        LessThan(rgb, 0.0031308f)
    );
}

vec3 SRGBToLinear(vec3 rgb)
{
    rgb = clamp(rgb, 0.0f, 1.0f);

    return mix(
        pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
        rgb / 12.92f,
        LessThan(rgb, 0.04045f)
    );
}

// ACES tone mapping curve fit to go from HDR to LDR
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}

void main()
{
    vec3 pixel_color = texture(screenTexture, TexCoords).rgb / frame;

    // Exposure
    pixel_color *= exposure;
    // Tonemapping
    pixel_color = tone_mapping ? ACESFilm(pixel_color) : pixel_color;
    // Converto to SRGB
    pixel_color = sRGB_conversion ? LinearToSRGB(pixel_color) : pixel_color;

    FragColor = vec4(pixel_color, 1);
}