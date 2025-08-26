#version 330 core
layout (location = 0) in vec2 aPos;          // base quad: (0,0),(16,0),(0,16),(16,16)
layout (location = 1) in vec2 aUVUnit;       // base UV within a tile: (0,0),(1,0),(0,1),(1,1)
layout (location = 2) in uvec2 iTileXY;      // instanced: tile grid coords (uint16)
layout (location = 3) in uint  iTileIndex;   // instanced: tile index 1..190

uniform mat4 uMVP;                           // pixel->clip transform
uniform vec2 uAtlasSize;                     // (304,160)
uniform vec2 uTilePx;                        // (16,16)
uniform uvec2 uGridDim;                      // (19,10) tiles per row/col

out vec2 vUV;

void main() {
    // compute world position in pixels
    vec2 world = aPos + vec2(iTileXY) * uTilePx;

    // Compute tile origin in pixels from tileIndex
    uint col = iTileIndex % uGridDim.x;
    uint row = iTileIndex / uGridDim.x;

    vec2 tileOrigin = vec2(col, row) * uTilePx;

    // Per-vertex UV within the chosen tile (aUVUnit in [0,1])
    // Optional half-texel padding to avoid bleeding:
    vec2 eps = vec2(0.01); // tweak if needed (in pixels)
    vec2 uvPx = tileOrigin + aUVUnit * (uTilePx - 2.0*eps) + eps;

    vUV = uvPx / uAtlasSize;

    gl_Position = uMVP * vec4(world, 0.0, 1.0);
}