#ifndef INC_Editor_H
#define INC_Editor_H

#include "UiGlobal.h"

#include <QtCore/QPointer>
#include <QtWidgets/QMainWindow>

#include "EditorConfig.h"
#include "Coords.h"

///////////////////////////////////////////////////////////////////////////

namespace Ui {
    class EditorClass;
}

namespace SCME {

//////////////////////////////////////////////////////////////////////////
//Forward declarations

class EditorWidget;
class ThumbnailWidget;
class LevelData;

/// @brief   Editor class
class SCME_UI_DLL Editor : public QMainWindow
{
    Q_OBJECT

signals:

    void uiError(const QString& message);

public:

    Editor(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    Editor(const QString& levelToOpen, QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    virtual ~Editor();

    inline const EditorConfig& config() const;

    inline std::shared_ptr<LevelData> level() const;

public slots:

    /// @returns True if the level is closed (or no level is currently loaded),
    ///          false if the user cancels the operation
    bool closeLevel();

    void newLevel();

    bool openLevel();

    bool openLevel(const QString& filename);

    bool saveLevel();

    bool saveLevelAs();

    bool saveLevelAs(const QString& filename);

    void toggleGridPreset();

    void onLevelLoaded();

private slots:

    void onUiError(const QString& message);

private:

    void initEditorWidget();

    void initTileset();

    void initRadar();

    std::unique_ptr<Ui::EditorClass> ui;

    EditorConfig mConfig;

    std::shared_ptr<LevelData> mLevel;

    QPointer<EditorWidget>    mEditorWidget;
    QPointer<ThumbnailWidget> mThumbnailWidget;
};


//////////////////////////////////////////////////////////////////////////

const EditorConfig& Editor::config() const
{
    return mConfig;
}

//////////////////////////////////////////////////////////////////////////

std::shared_ptr<LevelData> Editor::level() const
{
    return mLevel;
}


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//////////////////////////////////////////////////////////////////////////

#endif // INC_Editor_H

