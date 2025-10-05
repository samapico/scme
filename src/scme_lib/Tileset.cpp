#include "Tileset.h"

#include "ExtraLevelData.h"

#include <QtCore/QDebug>
#include <QtCore/QIODevice>
#include <QtGui/QPainter>

///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

Tileset::Tileset() :
    mIsDefault(false)
{
}

//////////////////////////////////////////////////////////////////////////

Tileset::~Tileset()
{
}

//////////////////////////////////////////////////////////////////////////

const QImage& Tileset::image() const
{
    return mImage;
}

//////////////////////////////////////////////////////////////////////////

const Tileset::BitmapFileHeader& Tileset::fileHeader() const
{
    return mFileHeader;
}

//////////////////////////////////////////////////////////////////////////

const Tileset::BitmapInfoHeader& Tileset::infoHeader() const
{
    return mInfoHeader;
}

//////////////////////////////////////////////////////////////////////////

bool Tileset::isDefault() const
{
    return mIsDefault;
}

//////////////////////////////////////////////////////////////////////////

const QPixmap& Tileset::pixmapWithExtraTiles() const
{
    return mPixmapWithExtraTiles;
}

//////////////////////////////////////////////////////////////////////////

void Tileset::setImage(const QImage& image)
{
    mIsDefault = false;
    mImage = image;
}

//////////////////////////////////////////////////////////////////////////

void Tileset::setDefault()
{
    mIsDefault = mImage.load(":/graphics/tiles.bmp");
    Q_ASSERT(mIsDefault);
}

//////////////////////////////////////////////////////////////////////////

QPixmap Tileset::addExtraTilesToImage(const QImage& image)
{
    //Load the extra tiles graphics once
    static const QPixmap pxExtra_s(":/ui/tileset_special.bmp");

    Q_ASSERT(pxExtra_s.width() == TILESET_W);
    Q_ASSERT(pxExtra_s.height() == TILESET_EXTRA_H);

    //Combine the level's tileset image with the extra tiles
    QPixmap px(TILESET_SIZE_WITH_EXTRA);

    QPainter painter(&px);

    painter.drawPixmap(0, 0, QPixmap::fromImage(image));
    painter.drawPixmap(0, TILESET_H, pxExtra_s);

    return px;
}


//////////////////////////////////////////////////////////////////////////

bool Tileset::load(QDataStream& in, Tileset& tileset, ExtraLevelData& out_eLVLdata)
{
    Q_ASSERT(in.device());
    if (!in.device())
        return false;

    qint64 startPos = in.device()->pos();

    in.readRawData((char*)&tileset.mFileHeader, sizeof(Tileset::BitmapFileHeader));

    in.readRawData((char*)&tileset.mInfoHeader, sizeof(Tileset::BitmapInfoHeader));

    Q_ASSERT(tileset.mInfoHeader.biBitCount >= 8);

    in.device()->seek(startPos);
    if (tileset.mImage.load(in.device(), "bmp"))
    {
        LogInfo() << "Read" << in.device()->pos() - startPos << "bytes in tileset";
        LogDebug() << "Now at" << in.device()->pos();
    }

    if (tileset.mFileHeader.bfReserved1)
    {
        LogInfo() << "Possible eLVL data at" << tileset.mFileHeader.bfReserved;

        in.device()->seek(tileset.mFileHeader.bfReserved);

        if (ExtraLevelData::load(in, out_eLVLdata))
        {
            LogInfo() << "eLVL data:" << out_eLVLdata.mBytes.toPercentEncoding();
        }

        LogDebug() << "Now at" << in.device()->pos();
    }
    else
    {
        LogInfo() << "No eLVL data";
    }

    LogDebug() << "Jumping to bfSize;" << tileset.mFileHeader.bfSize;
    in.device()->seek(tileset.mFileHeader.bfSize);

    tileset.mIsDefault = false;
    tileset.mPixmapWithExtraTiles = addExtraTilesToImage(tileset.mImage);

    return in.status() == QDataStream::Status::Ok;
}
