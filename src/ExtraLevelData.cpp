#include "ExtraLevelData.h"

#include "Global.h"
#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtCore/QIODevice>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;


//////////////////////////////////////////////////////////////////////////

bool ExtraLevelData::hasELVL(QDataStream& in)
{
    qDebug() << "@ExtraLevelData::hasELVL" << in.device()->pos();

    if (!in.atEnd())
    {
        in.startTransaction();
        qDebug() << "@ExtraLevelData::hasELVL" << in.device()->pos();

        //Read bytes
        uint32_t magic;
        in >> magic;
        qDebug() << "@ExtraLevelData::hasELVL after read at" << in.device()->pos();

        //Rollback read to return to original seek position
        in.rollbackTransaction();
        qDebug() << "@ExtraLevelData::hasELVL rolled back to" << in.device()->pos();

        return magic == MAGIC_ELVL; //"elvl"
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

bool ExtraLevelData::load(QDataStream& in, ExtraLevelData& eLvlData)
{
    in.startTransaction();

    uint32_t magic;
    uint32_t len;
    uint32_t reserved;

    in >> magic;

    if (magic != MAGIC_ELVL)
    {
        in.rollbackTransaction();
        qWarning() << "invalid eLVL data:" << intAsHexString(magic);

        return false;
    }

    in >> len; //total length of the section, including the { magic, len, reserved } header
    in >> reserved;

    qDebug() << len << " bytes in eLVL";

    len -= (sizeof(magic) + sizeof(len) + sizeof(reserved));

    QByteArray eLvlBytes(len, Qt::Uninitialized);
    in.readRawData(eLvlBytes.data(), len);

    qDebug() << eLvlBytes.toPercentEncoding();

    eLvlData.mBytes = eLvlBytes;

    return in.commitTransaction();
}
