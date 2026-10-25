#version 330 core

in vec2 v_position;
flat in vec2 v_start;
flat in vec2 v_end;
flat in vec3 v_color;

uniform float u_lineWidth;  // device pixels

out vec4 fragColor;

float distanceToSegment(vec2 point, vec2 start, vec2 end)
{
    vec2 segment = end - start;
    float lengthSquared = dot(segment, segment);
    float t = lengthSquared > 0.0 ? clamp(dot(point - start, segment) / lengthSquared, 0.0, 1.0) : 0.0;
    return length(point - (start + t * segment));
}

void main()
{
    // Capsule around the segment, with a one pixel soft edge.
    float distance = distanceToSegment(v_position, v_start, v_end);
    float alpha = clamp(u_lineWidth * 0.5 + 0.5 - distance, 0.0, 1.0);
    if (alpha <= 0.0) {
        discard;
    }

    fragColor = vec4(v_color, alpha);
}
