#include "Tileset.h"

#include <QtCore/QDebug>

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

QDataStream& ::operator>>(QDataStream& in, SCME::Tileset& tileset)
{
    //         'update the menu's
    //         'TORESTORE
    // '        frmGeneral.mnudiscardtileset.Enabled = True
    // '        frmGeneral.mnuExportTileset.Enabled = True
    //         'rewind to beginning
    //         Seek #f, 1

    //         Get #f, , bmpFileheader
    //         Get #f, , BMPInfoHeader

    qint64 startPos = in.device()->pos();

    in.readRawData((char*)&tileset.mFileHeader, sizeof(Tileset::BitmapFileHeader));

    in.readRawData((char*)&tileset.mInfoHeader, sizeof(Tileset::BitmapInfoHeader));


    //         'check if its 8/24 bit, normally it would be else we
    //         'have a corrupt lvl file
    //         'but check anyway
    //         AddDebug "OpenMap, Tileset found"
    //         AddDebug "OpenMap, " & BitmapHeaderInfoString(BMPInfoHeader)
    //         AddDebug "OpenMap, " & BitmapFileInfoString(bmpFileheader)
    // 
    //         If BMPInfoHeader.biBitCount < 8 Then
    //             AddDebug "OpenMap, Tileset is invalid"
    //             MessageBox "Invalid tileset within lvl file!", vbExclamation
    //             Close #f
    //             frmGeneral.IsBusy("frmMain" & id & ".OpenMap") = False
    //             Exit Sub
    //         End If
    
    Q_ASSERT(tileset.mInfoHeader.biBitCount >= 8);

    in.device()->seek(startPos);
    if (tileset.mImage.load(in.device(), "bmp"))
    {
        qDebug() << "Read " << in.device()->pos() - startPos << " bytes in tileset";
        qDebug() << "Now at " << in.device()->pos();
    }

    if (tileset.mFileHeader.bfReserved1)
    {
        qDebug() << "eLVL data at " << tileset.mFileHeader.bfReserved;

        in.device()->seek(tileset.mFileHeader.bfReserved);

        quint32 magic;
        quint32 len;
        quint32 reserved;

        in.setByteOrder(QDataStream::LittleEndian);
        in >> magic;
        Q_ASSERT(magic == 0x6c766c65);
        in >> len;
        in >> reserved;

        qDebug() << len << " bytes in eLVL";

        len -= 12;

        char* buf = new char[len+1];
        in.readRawData(buf, len);
        buf[len] = '\0';

        QByteArray str(buf, len);
        qDebug() << str.toPercentEncoding();

        delete[] buf;
    }
    else
    {
        qDebug() << "No eLVL data";
    }

    qDebug() << "Jumping to bfSize; " << tileset.mFileHeader.bfSize;
    in.device()->seek(tileset.mFileHeader.bfSize);

    tileset.mIsDefault = false;

    return in;
}
