#ifndef INC_ExtraLevelData_H
#define INC_ExtraLevelData_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"
#include <QtCore/QByteArray>


//////////////////////////////////////////////////////////////////////////

class QDataStream;

namespace SCME {

///////////////////////////////////////////////////////////////////////////

/// eLVL data section
class SCME_LIB_DLL ExtraLevelData
{
public:
    /// Peek \a in bytes to see if a eLVL data chunk is present; always returns to the original file position
    static bool hasELVL(QDataStream& in);

    static bool load(QDataStream& in, ExtraLevelData& eLvlData);

    QByteArray mBytes;

private:
    static constexpr uint32_t MAGIC_ELVL = 0x6c766c65; //"elvl"
};


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_ExtraLevelData_H
