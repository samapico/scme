#include "CmdSetTiles.h"

#include "LevelData.h"
#include "TileInfo.h"


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

CmdSetTiles::CmdSetTiles(std::shared_ptr<LevelData> level, const TileChange& tile, int compressId, QUndoCommand* parent /*= nullptr*/) :
    CmdSetTiles(level, QList<TileChange>({tile}), compressId, parent)
{
}

//////////////////////////////////////////////////////////////////////////

CmdSetTiles::CmdSetTiles(std::shared_ptr<LevelData> level, const QList<TileChange>& tiles, int compressId, QUndoCommand* parent /*= nullptr*/) :
    CmdBase(QObject::tr("Set tiles"), parent),
    mLevel(level),
    mCompressId(compressId),
    mChanges(tiles)
{
}

//////////////////////////////////////////////////////////////////////////

int CmdSetTiles::id() const
{
    static int sId = static_cast<int>(qHash("CmdSetTiles"));

    return mCompressId ? sId : -1;
}

//////////////////////////////////////////////////////////////////////////

void CmdSetTiles::redo()
{
    QList<TileChange> undoChanges;

    applyChanges(mLevel, mChanges, &undoChanges);

    mChangesUndo = undoChanges;

    setObsolete(mChangesUndo.isEmpty());
}

//////////////////////////////////////////////////////////////////////////

void CmdSetTiles::undo()
{
    applyChanges(mLevel, mChangesUndo, nullptr);
}

//////////////////////////////////////////////////////////////////////////

void CmdSetTiles::applyChanges(std::shared_ptr<LevelData> level, const QList<TileChange>& changes, QList<TileChange>* reverseChanges)
{
    //Apply changes
    Q_ASSERT(level);
    if (!level)
        return;

    LevelTiles& tiles = level->tiles();

    for (const TileChange& change : changes)
    {
        const int tileSize = TileInfoDict[change.mTileId].mSize;
        if (tileSize > 1 && (
            change.mXY.x() + tileSize > level->size().width() ||
            change.mXY.y() + tileSize > level->size().height())
            )
        {
            // Cannot put this tile, it would be out of bounds
            continue;
        }

        // @todo: Large objects on this tile to delete?


        for (int x = 0; x < tileSize; x++)
        {
            for (int y = 0; y < tileSize; y++)
            {
                TileCoords brushXY(change.mXY.x() + x, change.mXY.y() + y);

                Tile& t = tiles(brushXY);

                // Replace the tile, or if the new tile is a large object, clear other tiles
                TileId tid = (x || y) ? Tile::Void : change.mTileId;

                if (t.mId != tid)
                {
                    if (reverseChanges)
                        reverseChanges->prepend({ brushXY, t.mId });

                    t.mId = tid;
                }
            }
        }
    }

    emit level->tilesChanged(level.get());
}

//////////////////////////////////////////////////////////////////////////

bool CmdSetTiles::mergeWith(const QUndoCommand* command)
{
    auto pCmd = dynamic_cast<const CmdSetTiles*>(command);

    if (pCmd && mLevel && pCmd->mLevel == mLevel && pCmd->mCompressId == mCompressId)
    {
        Q_ASSERT(pCmd->id() == id());

        mChanges.append(pCmd->mChanges);
        mChangesUndo = pCmd->mChangesUndo + mChangesUndo;
        return true;
    }

    return false;
}