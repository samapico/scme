#ifndef INC_LevelTiles_H
#define INC_LevelTiles_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "Array2D.h"
#include "Tile.h"

#include <memory>

//////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

/// Specialization of Array2D for a 1024x1024 array of tiles that can also be accessed via a QImage mapped to the same memory
class SCME_LIB_DLL LevelTiles : public Array2D<Tile>
{
public:

    typedef QList<uint> PixelPalette;

    static PixelPalette getColorTableForTiles();

    LevelTiles();

    LevelTiles(const LevelTiles& orig);

    LevelTiles& operator=(const LevelTiles& rhs);

    ~LevelTiles();

    /// Do not allow Array2D::resize
    void resize(const QSize& dimensions) = delete;

    QImage* image() const;

    const QImage* imageConst() const;

protected:
    std::unique_ptr<QImage> mImage;

private:

    void allocateImage();
};

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_LevelTiles_H
