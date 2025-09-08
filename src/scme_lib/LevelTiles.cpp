#include "LevelTiles.h"

#include "TileInfo.h"

#include <QtGui/QImage>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

LevelTiles::LevelTiles() :
    Array2D<Tile>(MAP_SIZE)
{
    allocateImage();
}


//////////////////////////////////////////////////////////////////////////

LevelTiles::LevelTiles(const LevelTiles& orig) :
    Array2D<Tile>(orig)
{
    allocateImage();
    qDebug() << "LevelTiles copied";
}

//////////////////////////////////////////////////////////////////////////

LevelTiles::~LevelTiles()
{
    //Delete the image that uses the array bytes first
    mImage.reset();
}

//////////////////////////////////////////////////////////////////////////

LevelTiles& LevelTiles::operator=(const LevelTiles& rhs)
{
    //Copy tile data
    Array2D<Tile>::operator=(rhs);

    //Image should still be valid
    Q_ASSERT(mImage.get());

    qDebug() << "@LevelTiles::operator=: img data ptr = " << mImage->bits() << "tileptr=" << rowPtr(0);
    Q_ASSERT(mImage->bits() == reinterpret_cast<uchar*>(rowPtr(0)));

    return *this;
}

//////////////////////////////////////////////////////////////////////////

void LevelTiles::allocateImage()
{
    Q_ASSERT(!mImage); //Must be done only once

    //Allocate image using the existing tile array as memory
    Tile* t0 = rowPtr(0);

    mImage = std::make_unique<QImage>(
        reinterpret_cast<uchar*>(t0),
        width(),
        height(),
        QImage::Format::Format_Indexed8);
    mImage->setColorTable(getColorTableForTiles());

    Q_ASSERT(mImage->sizeInBytes() == MAP_W * MAP_H * size_t(1));

    //Note: if we provide QImage with a const uchar pointer, it will deep copy the data;
    //      we want to give it a non-const pointer, and it will use that data directly
    qDebug() << "@LevelTiles::allocateImage: img data ptr = " << mImage->bits() << "tileptr=" << rowPtr(0);
    Q_ASSERT(mImage->bits() == reinterpret_cast<uchar*>(rowPtr(0)));
}

//////////////////////////////////////////////////////////////////////////

LevelTiles::PixelPalette LevelTiles::getColorTableForTiles()
{
    qDebug() << "@getPixelPalette" << TilePixelColorPalette::sCurrentPalette.mPixelColorPalette;

    PixelPalette pp(256, Qt::Uninitialized);
    for (int i = 0; i < 256; i++)
        pp[i] = TileInfoDict.at(i).pixelColor().rgba();
    return pp;
}


//////////////////////////////////////////////////////////////////////////

QImage* LevelTiles::image() const
{
    return mImage.get();
}


//////////////////////////////////////////////////////////////////////////

const QImage* LevelTiles::imageConst() const
{
    return mImage.get();
}

//////////////////////////////////////////////////////////////////////////