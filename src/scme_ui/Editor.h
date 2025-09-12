#ifndef INC_Editor_H
#define INC_Editor_H

#include "UiGlobal.h"

#include <QtCore/QPointer>

#include <QtGui/QUndoGroup>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>

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
class TilesetWidget;
class LevelData;

/// @brief   Editor class
class SCME_UI_DLL Editor : public QMainWindow
{
    Q_OBJECT

signals:

    void uiError(const QString& message);

public:

    static EditorConfig& config();

    Editor(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    Editor(const QString& levelToOpen, QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    virtual ~Editor();

    inline std::shared_ptr<LevelData> level() const;

    TilesetWidget* tilesetWidget() const;

    QUndoStack* activeUndoStack() const;

    const QUndoGroup* undoGroup() const;

    QUndoGroup* undoGroup();

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

    void onUndoStackChanged();

    void updateStatusCursor(const LevelCoords& coords);

    void updateStatusZoom(float zoom);

    void updateWindowTitle();

private:

    void initEditorWidget();

    void initTileset(const LevelData* level);

    void initRadar();

    std::unique_ptr<Ui::EditorClass> ui;

    std::shared_ptr<LevelData> mLevel;

    QPointer<EditorWidget>    mEditorWidget;
    QPointer<ThumbnailWidget> mThumbnailWidget;

    QPointer<QLabel> mStatusBarCursor;
    QPointer<QLabel> mStatusBarZoom;
    QPointer<QLabel> mStatusBarDebug;

    /// List of QUndoStacks, one for each open document
    QUndoGroup mUndoGroup;

    QString mCurrentLevelFile;

    QList<QMetaObject::Connection> mLevelConnections;
};

//////////////////////////////////////////////////////////////////////////

std::shared_ptr<LevelData> Editor::level() const
{
    return mLevel;
}


///////////////////////////////////////////////////////////////////////////

} // End namespace SCME

//////////////////////////////////////////////////////////////////////////

#endif // INC_Editor_H

