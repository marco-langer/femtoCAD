#version 330 core

// One instance per line. The quad's corners come from gl_VertexID (triangle
// strip, 4 vertices), so there is no per-vertex buffer.
layout(location = 0) in vec2 a_start;
layout(location = 1) in vec2 a_end;
layout(location = 2) in vec3 a_color;
layout(location = 3) in float a_lineweight;  // millimeters
// The line's dash pattern in u_patterns of line.frag: first element, number of
// elements (0 for a continuous line) and the scale of its lengths.
layout(location = 4) in vec3 a_pattern;

uniform mat4 u_viewProjection;
uniform vec2 u_viewportSize;  // device pixels
// With lineweights, a line is a_lineweight wide, but at least u_minimumLineWidth.
// Without, all lines are u_lineWidth wide.
uniform bool u_lineweightsVisible;
uniform float u_pixelsPerMillimeter;  // device pixels
uniform float u_minimumLineWidth;     // device pixels
uniform float u_lineWidth;            // device pixels

// Everything in device pixels, for the distance calculation in line.frag.
out vec2 v_position;
flat out vec2 v_start;
flat out vec2 v_end;
flat out vec3 v_color;
flat out float v_lineWidth;
flat out int v_patternOffset;
flat out int v_patternCount;
flat out float v_patternScale;

vec2 toPixels(vec2 world)
{
    vec4 clip = u_viewProjection * vec4(world, 0.0, 1.0);
    return (clip.xy * 0.5 + 0.5) * u_viewportSize;
}

void main()
{
    vec2 start = toPixels(a_start);
    vec2 end = toPixels(a_end);

    vec2 direction = end - start;
    float len = length(direction);
    vec2 along = len > 0.0 ? direction / len : vec2(1.0, 0.0);
    vec2 across = vec2(-along.y, along.x);

    float lineWidth = u_lineweightsVisible
        ? max(a_lineweight * u_pixelsPerMillimeter, u_minimumLineWidth)
        : u_lineWidth;

    // Half the line width plus one pixel for the soft edge, around the segment
    // and past its ends for the round caps.
    float extent = lineWidth * 0.5 + 1.0;

    float endFactor = float(gl_VertexID / 2);
    float side = (gl_VertexID % 2 == 0) ? -1.0 : 1.0;
    vec2 position = mix(start - along * extent, end + along * extent, endFactor) + across * side * extent;

    v_position = position;
    v_start = start;
    v_end = end;
    v_color = a_color;
    v_lineWidth = lineWidth;
    v_patternOffset = int(a_pattern.x);
    v_patternCount = int(a_pattern.y);
    v_patternScale = a_pattern.z;
    gl_Position = vec4(position / u_viewportSize * 2.0 - 1.0, 0.0, 1.0);
}
