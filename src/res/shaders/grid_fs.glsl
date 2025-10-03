#version 330 core
const int GRIDLEVELS = 5;

const float epsilon = 1.e-5;
uniform vec2 uTileSize;     // screen size of a tile (TILE_SIZE)

uniform vec4 uBounds;       // left, top, right, bottom, in level pixel coordinates
uniform mat4 uMVP;          // pixel->clip transform
uniform float uZoom;        // zoom factor

uniform float uGridSpacings[GRIDLEVELS]; // grid spacing, in tiles, in descending order
uniform vec4  uGridColors[GRIDLEVELS];   // color for each grid level; when zoomed out, some colors may be set with an alpha of 0 to avoid filling the screen
uniform float uGridThickness[GRIDLEVELS];    // 
uniform float uSmallestGrid;

out vec4 FragColor;


bool isOnGrid(float p, float spacing, float halfThickness)
{
    float m = mod(p, spacing);
    
    return m <= halfThickness || (spacing - m) < halfThickness;
}

void main()
{
    // 1px screen-space half-thickness for crisp lines (can be made zoom-aware if desired)
    float halfThicknessBase = 0.5 / uZoom / 16.0;

    vec2 levelPos = vec2(
        uBounds.x + (gl_FragCoord.x - 0.5) / uZoom, //bounds.left + gl.x/zoom
        uBounds.w - (gl_FragCoord.y + 0.5) / uZoom
    )/uTileSize;
    
    if (   levelPos.x < -halfThicknessBase
        || levelPos.x > (1024.0) + halfThicknessBase
        || levelPos.y < -halfThicknessBase
        || levelPos.y > (1024.0) + halfThicknessBase
        || (!isOnGrid(levelPos.x, uSmallestGrid, halfThicknessBase * uGridThickness[0]) && !isOnGrid(levelPos.y, uSmallestGrid, halfThicknessBase * uGridThickness[0]))
        )
    {
        //Out of level bounds
        //or
        //Not on any grid
        discard;
        return;
    }
    
    // Draw from coarsest to finest level; first match wins.
    for (int i = 0; i < GRIDLEVELS; ++i)
    {
        float halfThickness = uGridThickness[i] * 0.5 / uZoom / 16.0;
        
        vec4 gridColor = uGridColors[i];
    
        // If we're close to either a vertical or horizontal line, draw it
        if (gridColor.a > 0 && (isOnGrid(levelPos.x, uGridSpacings[i], halfThickness) || isOnGrid(levelPos.y, uGridSpacings[i], halfThickness)))
        {
            FragColor = gridColor;
            return;
        }
    }

    // No grid line at this fragment
    FragColor = vec4(1,0,0,1);
    //discard;
}
