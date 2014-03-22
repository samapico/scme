#ifndef LEVELDATA_H
#define LEVELDATA_H

//////////////////////////////////////////////////////////////////////////

#include "Tile.h"
#include "Tileset.h"

//////////////////////////////////////////////////////////////////////////

class QString;
class QDataStream;

//////////////////////////////////////////////////////////////////////////

class LevelData
{

public:
    LevelData();
    virtual ~LevelData();

    bool loadFromFile(const QString& filepath);

    bool saveToFile(const QString& filepath) const;

    const Tileset& tileset() const;

    void setDirty(bool);

    bool isDirty() const;

    inline const QSize& size() const;

protected:

    bool load(QDataStream& in);

    Tile* mTiles;
    mutable bool mIsDirty;

    Tileset mTileset;

    QSize mSize;
};


//////////////////////////////////////////////////////////////////////////

const QSize& LevelData::size() const
{
    return mSize;
}


//////////////////////////////////////////////////////////////////////////

#endif // LEVELDATA_H
