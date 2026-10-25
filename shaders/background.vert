#version 330 core

// One triangle that covers the whole viewport. Its corners come from
// gl_VertexID (3 vertices), so there is no vertex buffer.

// 0 at the bottom edge of the viewport, 1 at the top.
out float v_height;

void main()
{
    // (-1, -1), (3, -1), (-1, 3)
    vec2 position = vec2(float((gl_VertexID & 1) << 2) - 1.0, float((gl_VertexID & 2) << 1) - 1.0);

    v_height = position.y * 0.5 + 0.5;
    gl_Position = vec4(position, 0.0, 1.0);
}
