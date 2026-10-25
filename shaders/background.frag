#version 330 core

in float v_height;

uniform vec4 u_topColor;
uniform vec4 u_bottomColor;

// The world origin in window coordinates (device pixels, y up).
uniform vec2 u_origin;
uniform float u_axisWidth;  // device pixels
uniform vec4 u_xAxisColor;
uniform vec4 u_yAxisColor;
uniform bool u_axesVisible;

out vec4 fragColor;

// Interleaved gradient noise in [0, 1), from the pixel position.
float noise(vec2 pixel)
{
    return fract(52.9829189 * fract(dot(pixel, vec2(0.06711056, 0.00583715))));
}

// Coverage of a pixel by a line at the given distance, with a one pixel soft edge.
float coverage(float distance)
{
    return clamp(u_axisWidth * 0.5 + 0.5 - distance, 0.0, 1.0);
}

void main()
{
    vec3 color = mix(u_bottomColor.rgb, u_topColor.rgb, v_height);

    // A dark gradient spans only a few 8-bit steps and shows bands. Noise of
    // up to half a step either way breaks them up.
    color += (noise(gl_FragCoord.xy) - 0.5) / 255.0;

    // The x axis is the horizontal line through the origin, the y axis the vertical one.
    if (u_axesVisible) {
        color = mix(color, u_xAxisColor.rgb, coverage(abs(gl_FragCoord.y - u_origin.y)));
        color = mix(color, u_yAxisColor.rgb, coverage(abs(gl_FragCoord.x - u_origin.x)));
    }

    fragColor = vec4(color, 1.0);
}
