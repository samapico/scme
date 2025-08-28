#ifndef INC_Tileset_H
#define INC_Tileset_H

#include "Global.h"

#include <QtGui/QImage>

//////////////////////////////////////////////////////////////////////////

class QDataStream;


///////////////////////////////////////////////////////////////////////////

namespace SCME {

//////////////////////////////////////////////////////////////////////////

class ExtraLevelData;


//////////////////////////////////////////////////////////////////////////


class SCME_LIB_DLL Tileset
{
public:

#pragma pack(push, 1)

    /// @note Same as BITMAPFILEHEADER struct from WinGDI.h
    struct BitmapFileHeader
    {
        uint16_t bfType;
        uint32_t bfSize;
        union
        {
            struct
            {
                uint16_t bfReserved1;
                uint16_t bfReserved2;
            };
            uint32_t bfReserved;
        };
        uint32_t bfOffBits;
    };

    /// @note Same as BITMAPINFOHEADER struct from WinGDI.h
    struct BitmapInfoHeader
    {
        uint32_t biSize;
        int32_t  biWidth;
        int32_t  biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t  biXPelsPerMeter;
        int32_t  biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };

    /// @note Same as RGBQUAD struct from WinGDI.h
    struct RGBQuad
    {
        uint8_t rgbBlue;
        uint8_t rgbGreen;
        uint8_t rgbRed;
        uint8_t rgbReserved;
    };

#pragma pack(pop)

    Tileset();
    virtual ~Tileset();

    bool isDefault() const;

    const QImage& image() const;

    void setImage(const QImage& image);

    void setDefault();

    static bool load(QDataStream& in, Tileset& tileset, ExtraLevelData& out_eLVLdata);

    const BitmapFileHeader& fileHeader() const;
    const BitmapInfoHeader& infoHeader() const;

protected:

    BitmapFileHeader mFileHeader;
    BitmapInfoHeader mInfoHeader;

    QImage mImage;

    bool mIsDefault;
};

///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_Tileset_H
