#ifndef EDITOR_H
#define EDITOR_H

#include <QtGui/QMainWindow>
#include "ui_Editor.h"

#include "EditorConfig.h"


//////////////////////////////////////////////////////////////////////////
//Forward declarations

class EditorWidget;
class ThumbnailWidget;
class LevelData;

/// @brief   Editor class
class Editor : public QMainWindow
{
    Q_OBJECT

public:

    static const int TILE_WIDTH;
    static const int TILE_HEIGHT;
    static const QSize TILE_SIZE;

    Editor(QWidget *parent = 0, Qt::WFlags flags = 0);
    virtual ~Editor();

    /// Size (in tiles) of the current level
    QSize levelSize() const;

    /// Size (in pixels) of the current level
    inline const QSize& levelPixelSize() const;

    inline const EditorConfig& config() const;

    inline int    pixelToTileX(int pixel) const;
    inline int    pixelToTileY(int pixel) const;
    inline QPoint pixelToTile (const QPoint& pixel) const;

    inline int    tileToPixelX(int tile) const;
    inline int    tileToPixelY(int tile) const;
    inline QPoint tileToPixel (const QPoint& tile) const;

    QPoint boundPixelToLevel(const QPoint& pixel) const;

    inline const LevelData* level() const;

public slots:
    
    /// @returns True if the level is closed (or no level is currently loaded),
    ///          false if the user cancels the operation         
    bool closeLevel();

    void newLevel();

    void openLevel();

    void saveLevel();

    void saveLevelAs();

    void toggleGridPreset();

    void onLevelLoaded();

private:

    void initEditorWidget();

    void initTileset();

    void initRadar();

    Ui::EditorClass ui;

    EditorConfig mConfig;
    QSize mLevelPixelSize;

    LevelData*       mLevel;

    EditorWidget*    mEditorWidget;
    ThumbnailWidget* mThumbnailWidget;
};


//////////////////////////////////////////////////////////////////////////

int Editor::pixelToTileX(int pixel) const
{
    return pixel / TILE_WIDTH;
}

//////////////////////////////////////////////////////////////////////////

int Editor::pixelToTileY(int pixel) const
{
    return pixel / TILE_HEIGHT;
}

//////////////////////////////////////////////////////////////////////////

QPoint Editor::pixelToTile(const QPoint& pixel) const
{
    return QPoint(pixel.x() / TILE_WIDTH, pixel.y() / TILE_HEIGHT);
}

//////////////////////////////////////////////////////////////////////////

int Editor::tileToPixelX(int tile) const
{
    return tile*TILE_WIDTH;
}

//////////////////////////////////////////////////////////////////////////

int Editor::tileToPixelY(int tile) const
{
    return tile*TILE_HEIGHT;
}

//////////////////////////////////////////////////////////////////////////

QPoint Editor::tileToPixel(const QPoint& tile) const
{
    return QPoint(tile.x() * TILE_WIDTH, tile.y() * TILE_HEIGHT);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

const QSize& Editor::levelPixelSize() const
{
    return mLevelPixelSize;
}

//////////////////////////////////////////////////////////////////////////

const EditorConfig& Editor::config() const
{
    return mConfig;
}

//////////////////////////////////////////////////////////////////////////

const LevelData* Editor::level() const
{
    return mLevel;
}


//////////////////////////////////////////////////////////////////////////

#endif // EDITOR_H
