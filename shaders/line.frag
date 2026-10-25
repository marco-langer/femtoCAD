#version 330 core

// Must match maximumPatternElements in SceneView.cpp.
const int maximumPatternElements = 512;

in vec2 v_position;
flat in vec2 v_start;
flat in vec2 v_end;
flat in vec3 v_color;
flat in float v_lineWidth;  // device pixels
flat in int v_patternOffset;
flat in int v_patternCount;
flat in float v_patternScale;

// The dash patterns of all linetypes, in world units before scaling: a dash is
// positive, a gap negative and a dot 0.
uniform float u_patterns[maximumPatternElements];
uniform float u_worldPerPixel;         // world units per device pixel
// Patterns shorter than this are drawn continuous, since their dashes would
// blur into each other.
uniform float u_minimumPatternLength;  // device pixels

out vec4 fragColor;

float distanceToSegment(vec2 point, vec2 start, vec2 end)
{
    vec2 segment = end - start;
    float lengthSquared = dot(segment, segment);
    float t = lengthSquared > 0.0 ? clamp(dot(point - start, segment) / lengthSquared, 0.0, 1.0) : 0.0;
    return length(point - (start + t * segment));
}

float patternLength()
{
    float total = 0.0;
    for (int i = 0; i < v_patternCount; ++i) {
        total += abs(u_patterns[v_patternOffset + i]);
    }
    return total * v_patternScale;
}

// How much of the pattern's dashes and dots covers position, in world units
// from the pattern's start, with a one pixel soft edge. Dots are as long as the
// line is wide.
float elementsCoverage(float position, float lineWidth)
{
    float coverage = 0.0;
    float elementStart = 0.0;
    for (int i = 0; i < v_patternCount; ++i) {
        float length = u_patterns[v_patternOffset + i] * v_patternScale;
        float inside = -1.0;  // pixels inside the element, negative outside
        if (length > 0.0) {
            inside = min(position - elementStart, elementStart + length - position) / u_worldPerPixel;
        } else if (length == 0.0) {
            inside = lineWidth * 0.5 - abs(position - elementStart) / u_worldPerPixel;
        }
        coverage = max(coverage, clamp(inside + 0.5, 0.0, 1.0));
        elementStart += abs(length);
    }
    return coverage;
}

// How much of the dash pattern covers the point at distance along the line,
// in world units from its start.
float patternCoverage(float distance, float lineWidth)
{
    if (v_patternCount == 0) {
        return 1.0;
    }

    float total = patternLength();
    if (total / u_worldPerPixel < u_minimumPatternLength) {
        return 1.0;
    }

    // Elements at the end of one repetition and the start of the next are
    // next to each other, so look at both neighbors too for the soft edges.
    float position = mod(distance, total);
    return max(elementsCoverage(position, lineWidth),
               max(elementsCoverage(position + total, lineWidth),
                   elementsCoverage(position - total, lineWidth)));
}

void main()
{
    // Capsule around the segment, with a one pixel soft edge.
    float distance = distanceToSegment(v_position, v_start, v_end);
    float alpha = clamp(v_lineWidth * 0.5 + 0.5 - distance, 0.0, 1.0);

    // The pattern starts at the line's start. The round caps continue the
    // pattern of the line's ends.
    vec2 segment = v_end - v_start;
    float segmentLength = length(segment);
    float along = segmentLength > 0.0
        ? clamp(dot(v_position - v_start, segment / segmentLength), 0.0, segmentLength)
        : 0.0;
    alpha *= patternCoverage(along * u_worldPerPixel, v_lineWidth);

    if (alpha <= 0.0) {
        discard;
    }

    fragColor = vec4(v_color, alpha);
}
