#ifndef INC_LevelData_H
#define INC_LevelData_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "Array2D.h"
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

    inline QSize size() const;

protected:

    bool load(QDataStream& in);

    Array2D<Tile> mTiles;
    mutable bool mIsDirty = false;

    Tileset mTileset;
};


//////////////////////////////////////////////////////////////////////////

QSize LevelData::size() const
{
    return mTiles.size();
}


//////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_LevelData_H
