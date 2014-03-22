#ifndef TILESET_H
#define TILESET_H

//////////////////////////////////////////////////////////////////////////

class QDataStream;

#include <QtGui/QImage>

//////////////////////////////////////////////////////////////////////////

class Tileset
{

public:

#pragma pack(push, 1)

    /// @note Same as BITMAPFILEHEADER struct from WinGDI.h
    struct BitmapFileHeader
    {
        quint16 bfType;
        quint32 bfSize;
        union
        {
            struct
            {
                quint16 bfReserved1;
                quint16 bfReserved2;
            };
            quint32 bfReserved;
        };
        quint32 bfOffBits;
    };

    /// @note Same as BITMAPINFOHEADER struct from WinGDI.h
    struct BitmapInfoHeader
    {
        quint32 biSize;
        qint32  biWidth;
        qint32  biHeight;
        quint16 biPlanes;
        quint16 biBitCount;
        quint32 biCompression;
        quint32 biSizeImage;
        qint32  biXPelsPerMeter;
        qint32  biYPelsPerMeter;
        quint32 biClrUsed;
        quint32 biClrImportant;
    };

    /// @note Same as RGBQUAD struct from WinGDI.h
    struct RGBQuad
    {
        quint8 rgbBlue;
        quint8 rgbGreen;
        quint8 rgbRed;
        quint8 rgbReserved;
    };

#pragma pack(pop)

    Tileset();
    virtual ~Tileset();

    bool isDefault() const;

    const QImage& image() const;

    void setImage(const QImage& image);

    void setDefault();

protected:

    BitmapFileHeader mFileHeader;
    BitmapInfoHeader mInfoHeader;

    QImage mImage;

    bool mIsDefault;

    friend QDataStream& operator>> (QDataStream& in, Tileset&);
};

QDataStream& operator>> (QDataStream& in, Tileset&);

#endif // TILESET_H
