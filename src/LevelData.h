#ifndef LEVELDATA_H
#define LEVELDATA_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "Tile.h"
#include "Tileset.h"


///////////////////////////////////////////////////////////////////////////

class QString;
class QDataStream;

//////////////////////////////////////////////////////////////////////////

namespace SCME {

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

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // LEVELDATA_H
