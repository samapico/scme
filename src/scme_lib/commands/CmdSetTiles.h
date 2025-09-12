#ifndef INC_CmdSetTiles_H
#define INC_CmdSetTiles_H

//////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "CmdBase.h"
#include "Coords.h"


//////////////////////////////////////////////////////////////////////////

namespace SCME {

class LevelData;

//////////////////////////////////////////////////////////////////////////

class SCME_LIB_DLL CmdSetTiles : public CmdBase
{
public:

    struct TileChange
    {
        TileChange() = default;
        TileChange(const TileCoords& xy, TileId id) : mXY(xy), mTileId(id) {}

        TileCoords mXY;
        TileId mTileId;
    };

    CmdSetTiles(std::shared_ptr<LevelData> level, const TileChange& tile, int compressId, QUndoCommand* parent = nullptr);

    CmdSetTiles(std::shared_ptr<LevelData> level, const QList<TileChange>& tiles, int compressId, QUndoCommand* parent = nullptr);

    virtual ~CmdSetTiles() = default;

    void redo() override;

    void undo() override;

    int id() const override;

    bool mergeWith(const QUndoCommand* command) override;

protected:

    static void applyChanges(std::shared_ptr<LevelData> level, const QList<TileChange>& changes, QList<TileChange>* reverseChanges);

    std::shared_ptr<LevelData> mLevel;

    QList<TileChange> mChanges;
    QList<TileChange> mChangesUndo;

    int mCompressId = 0;
};


//////////////////////////////////////////////////////////////////////////

} // End namespace SCME

///////////////////////////////////////////////////////////////////////////

#endif // INC_CmdSetTiles_H
