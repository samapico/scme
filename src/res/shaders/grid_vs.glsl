#version 330 core
// We don't need to pass anything; the fragment shader uses gl_FragCoord + uniforms

uniform float uDrawZ;                        // z-coordinate for this draw

void main()
{
    // Positions that cover the whole screen with a single triangle
    const vec2 pos[3] = vec2[3](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );
    
    gl_Position = vec4(pos[gl_VertexID], uDrawZ, 1.0);
}