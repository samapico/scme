#version 330 core
layout (location = 0) in vec2 aPos;          // base quad: (0,0),(16,0),(0,16),(16,16)
layout (location = 1) in vec2 aUVUnit;       // base UV within a tile: (0,0),(1,0),(0,1),(1,1)
layout (location = 2) in ivec2 iTileXY;       // instanced: tile grid coords (int16)
layout (location = 3) in uint  iTileIndex;   // instanced: tile index 1..255
layout (location = 4) in uint  iTileSize;    // instanced: tile size multiplier

uniform mat4 uMVP;                           // pixel->clip transform
uniform vec2 uAtlasSize;                     // (304,160+64)
uniform vec2 uTilePx;                        // (16,16)
uniform uvec2 uGridDim;                      // (19,10+4) tiles per row/col
uniform float uDrawZ;                        // z-coordinate for this draw
uniform vec2 uDrawOffset;
uniform float uZoom;        // zoom factor

out vec2 vUV;

void main() {
    // compute world position in pixels
    vec2 world = (aPos * iTileSize + vec2(iTileXY) * uTilePx) + uDrawOffset;

    // Compute tile origin in pixels from tileIndex
    uint col = iTileIndex % uGridDim.x;
    uint row = iTileIndex / uGridDim.x;

    vec2 tileOrigin = vec2(col, row) * uTilePx;

    // Per-vertex UV within the chosen tile (aUVUnit in [0,1])
    // Optional half-texel padding to avoid bleeding:
    vec2 eps = vec2(0.25, 0.25) / uZoom; // tweak if needed (in pixels)
    vec2 uvPx = tileOrigin + aUVUnit * (uTilePx - 2.0*eps) + eps;

    vUV = uvPx / uAtlasSize;

    gl_Position = uMVP * vec4(world, uDrawZ, 1.0);
}