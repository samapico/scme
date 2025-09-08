#include "Editor.h"
#include "ui_Editor.h"

#include "EditorWidget.h"
#include "ThumbnailWidget.h"
#include "LambdaEventFilter.h"

#include "LevelData.h"

#include "../appver.h"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>

#include <QtCore/QString>
#include <QtCore/QDebug>

///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;


//////////////////////////////////////////////////////////////////////////

Editor::Editor(const QString& levelToOpen, QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    ui(std::make_unique<Ui::EditorClass>())
{
    ui->setupUi(this);

    connect(this, &Editor::uiError, this, &Editor::onUiError);

    setWindowTitle(tr("SCME v%1").arg(applicationVersionString()));

    connect(ui->changeGridPreset, &QPushButton::clicked, this, &Editor::toggleGridPreset);

    connect(ui->actionNew    , &QAction::triggered, this, &Editor::newLevel);
    connect(ui->actionOpen   , &QAction::triggered, this, qOverload<>(&Editor::openLevel));
    connect(ui->actionSave   , &QAction::triggered, this, &Editor::saveLevel);
    connect(ui->actionSave_As, &QAction::triggered, this, qOverload<>(&Editor::saveLevelAs));
    connect(ui->actionClose  , &QAction::triggered, this, &Editor::closeLevel);

    initEditorWidget();
    initRadar();

    if (levelToOpen.isEmpty())
    {
        //Create new level
        newLevel();
    }
    else
    {
        openLevel(levelToOpen);
    }
}

//////////////////////////////////////////////////////////////////////////

Editor::Editor(QWidget* parent, Qt::WindowFlags flags) :
    Editor(QString(), parent, flags)
{
}

//////////////////////////////////////////////////////////////////////////

Editor::~Editor()
{
}

//////////////////////////////////////////////////////////////////////////

TilesetWidget* Editor::tilesetWidget() const
{
    return ui->tilesetWidget;
}

//////////////////////////////////////////////////////////////////////////

void Editor::onUiError(const QString& message)
{
    qWarning() << "Error:" << message;
    QMessageBox::warning(this, tr("Error"), message);
}

//////////////////////////////////////////////////////////////////////////

void Editor::toggleGridPreset()
{
    static int gridPreset_s = EditorConfig::Grey;

    gridPreset_s++;
    gridPreset_s %= EditorConfig::GridPresetCount;

    mConfig.setGridPreset((EditorConfig::GridPreset)gridPreset_s);

    if (centralWidget())
        centralWidget()->update();
}

//////////////////////////////////////////////////////////////////////////

void Editor::initEditorWidget()
{
    if (!mEditorWidget)
    {
        mEditorWidget = new EditorWidget(this);
        setCentralWidget(mEditorWidget);

        connect(mEditorWidget, &EditorWidget::levelTilesetChanged, this, &Editor::initTileset);
    }
}

//////////////////////////////////////////////////////////////////////////

void Editor::initRadar()
{
    if (!mThumbnailWidget)
    {
        mThumbnailWidget = new ThumbnailWidget(this);

        Q_ASSERT(mEditorWidget);
        connect(mThumbnailWidget, &ThumbnailWidget::doCenterView, mEditorWidget, &EditorWidget::setViewCenterSmooth, Qt::QueuedConnection);
        connect(mEditorWidget, &EditorWidget::viewMoved, mThumbnailWidget, &ThumbnailWidget::redrawViewBounds, Qt::QueuedConnection);
        connect(mEditorWidget, &EditorWidget::levelTilesChanged, mThumbnailWidget, &ThumbnailWidget::redrawLevel, Qt::QueuedConnection);

        ui->dockRadar->setWidget(mThumbnailWidget);
    }
}

//////////////////////////////////////////////////////////////////////////

void Editor::initTileset(const LevelData* level)
{
    ui->tilesetWidget->onTilesetChanged(level ? level->tileset() : Tileset());
}

//////////////////////////////////////////////////////////////////////////

void Editor::newLevel()
{
    if (closeLevel())
    {
        qApp->setOverrideCursor(Qt::WaitCursor);

        Q_ASSERT(!mLevel);
        mLevel = std::make_shared<LevelData>();

        onLevelLoaded();

        qApp->restoreOverrideCursor();
    }
}

//////////////////////////////////////////////////////////////////////////

bool Editor::openLevel(const QString& filename)
{
    if (!QFile::exists(filename))
    {
        emit uiError(tr("File not found: %1").arg(filename));
        return false;
    }

    qApp->setOverrideCursor(Qt::WaitCursor);

    Q_ASSERT(!mLevel);
    mLevel = std::make_shared<LevelData>();

    bool bLoaded = mLevel->loadFromFile(filename);

    qApp->restoreOverrideCursor();

    if (bLoaded)
    {
        onLevelLoaded();
    }
    else
    {
        emit uiError(tr("Failed to load level: %1").arg(filename));
    }

    return bLoaded;
}

//////////////////////////////////////////////////////////////////////////

bool Editor::openLevel()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Levels (*.lvl)"));

    if (!path.isEmpty())
    {
        if (closeLevel()) //user can cancel here if there are unsaved changes
        {
            return openLevel(path);
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

bool Editor::saveLevel()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    /// ...

    qApp->restoreOverrideCursor();

    return false;
}

//////////////////////////////////////////////////////////////////////////

bool Editor::saveLevelAs(const QString& filename)
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    /// ...

    qApp->restoreOverrideCursor();

    return false;
}

//////////////////////////////////////////////////////////////////////////

bool Editor::saveLevelAs()
{
    return saveLevelAs(QString());
}

//////////////////////////////////////////////////////////////////////////

bool Editor::closeLevel()
{
    bool cancel = false;

    if (mLevel && false /*@todo isDirty()?*/)
    {
        int result = QMessageBox::question(this, tr("Save changes"), tr("Save changes to the current level?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

        if (result == QMessageBox::Yes)
        {
            saveLevel();
        }
        else if (result == QMessageBox::Cancel)
        {
            cancel = true;
        }
    }

    if (!cancel)
    {
        mLevel = nullptr;
    }

    onLevelLoaded();

    return !cancel;
}

//////////////////////////////////////////////////////////////////////////

void Editor::onLevelLoaded()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    if (mEditorWidget)
    {
        mEditorWidget->onLevelChanged();

        if (mLevel)
        {
            mEditorWidget->setDefaultZoom();
            mEditorWidget->setViewCenter(mLevel->bounds().center());
        }
        else
        {
            mEditorWidget->update();
        }
    }
    qApp->restoreOverrideCursor();
}
