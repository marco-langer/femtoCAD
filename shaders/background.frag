#version 330 core

in float v_height;

uniform vec4 u_topColor;
uniform vec4 u_bottomColor;

out vec4 fragColor;

// Interleaved gradient noise in [0, 1), from the pixel position.
float noise(vec2 pixel)
{
    return fract(52.9829189 * fract(dot(pixel, vec2(0.06711056, 0.00583715))));
}

void main()
{
    vec3 color = mix(u_bottomColor.rgb, u_topColor.rgb, v_height);

    // A dark gradient spans only a few 8-bit steps and shows bands. Noise of
    // up to half a step either way breaks them up.
    color += (noise(gl_FragCoord.xy) - 0.5) / 255.0;

    fragColor = vec4(color, 1.0);
}
