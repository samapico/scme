#ifndef EDITOR_H
#define EDITOR_H

#include <QtGui/QMainWindow>
#include "ui_editor.h"

#include "editorConfig.h"

class Editor : public QMainWindow
{
    Q_OBJECT

public:

    static const int TILE_WIDTH;
    static const int TILE_HEIGHT;
    static const QSize TILE_SIZE;

    Editor(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~Editor();

    /// Size (in tiles) of the current level
    inline const QSize& levelSize() const;

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

public slots:
    void toggleGridPreset();

private:
    Ui::EditorClass ui;

    EditorConfig mConfig;

    QSize mLevelSize;
    QSize mLevelPixelSize;
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

const QSize& Editor::levelSize() const
{
    return mLevelSize;
}

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

#endif // EDITOR_H
