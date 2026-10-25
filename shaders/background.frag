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

// Dot grid: minor dots u_gridSpacing apart, major dots ten times as far.
// Offsets are the position of one dot of each level in window coordinates,
// kept within one spacing so the arithmetic stays in float precision.
uniform float u_gridSpacing;  // device pixels
uniform vec2 u_gridMinorOffset;
uniform vec2 u_gridMajorOffset;
uniform float u_gridMinorRadius;  // device pixels
uniform float u_gridMajorRadius;  // device pixels
// How strongly the minor dots show, 0 to 1. Major dots always show fully.
uniform float u_gridMinorIntensity;
// Alpha is the strength of a fully shown dot.
uniform vec4 u_gridColor;
uniform bool u_gridVisible;

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

// Distance from a pixel to the nearest point of a square lattice. The point
// is moved to the center of its pixel, so all dots cover the same pixels
// instead of smearing differently across neighbors.
float distanceToLattice(vec2 pixel, vec2 offset, float spacing)
{
    vec2 point = offset + round((pixel - offset) / spacing) * spacing;
    return length(pixel - (floor(point) + 0.5));
}

// Coverage of a pixel by a dot at the given distance, with a one pixel soft edge.
float dotCoverage(float distance, float radius)
{
    return clamp(radius + 0.5 - distance, 0.0, 1.0);
}

void main()
{
    vec3 color = mix(u_bottomColor.rgb, u_topColor.rgb, v_height);

    // A dark gradient spans only a few 8-bit steps and shows bands. Noise of
    // up to half a step either way breaks them up.
    color += (noise(gl_FragCoord.xy) - 0.5) / 255.0;

    if (u_gridVisible) {
        float minorDistance = distanceToLattice(gl_FragCoord.xy, u_gridMinorOffset, u_gridSpacing);
        float majorDistance = distanceToLattice(gl_FragCoord.xy, u_gridMajorOffset, u_gridSpacing * 10.0);
        float strength = max(dotCoverage(minorDistance, u_gridMinorRadius) * u_gridMinorIntensity,
                             dotCoverage(majorDistance, u_gridMajorRadius));
        color = mix(color, u_gridColor.rgb, u_gridColor.a * strength);
    }

    // The x axis is the horizontal line through the origin, the y axis the vertical one.
    if (u_axesVisible) {
        color = mix(color, u_xAxisColor.rgb, coverage(abs(gl_FragCoord.y - u_origin.y)));
        color = mix(color, u_yAxisColor.rgb, coverage(abs(gl_FragCoord.x - u_origin.x)));
    }

    fragColor = vec4(color, 1.0);
}
