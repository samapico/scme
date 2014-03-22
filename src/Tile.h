#ifndef TILE_H
#define TILE_H

#include "LevelObject.h"

class Tile : public LevelObject
{

public:
    Tile();
    virtual ~Tile();

private:
    unsigned char id;
};

#endif // TILE_H
