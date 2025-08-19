#ifndef EDITOR_H
#define EDITOR_H

#include "Global.h"

#include <QtWidgets/QMainWindow>
#include "ui_Editor.h"

#include "EditorConfig.h"
#include "Coords.h"

///////////////////////////////////////////////////////////////////////////

namespace SCME {

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

    Editor(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    virtual ~Editor();

    /// Size (in tiles) of the current level
    QSize levelSize() const;

    /// Size (in pixels) of the current level
    inline const LevelBounds& levelBounds() const;

    inline const EditorConfig& config() const;

    LevelCoords boundPixelToLevel(const LevelCoords& pixel) const;

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
    LevelBounds mLevelBounds; //Entire level bounds (in pixels)

    LevelData*       mLevel;

    EditorWidget*    mEditorWidget;
    ThumbnailWidget* mThumbnailWidget;
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

const LevelBounds& Editor::levelBounds() const
{
    return mLevelBounds;
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


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//////////////////////////////////////////////////////////////////////////

#endif // EDITOR_H
