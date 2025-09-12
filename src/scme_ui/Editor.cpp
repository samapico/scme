#include "Editor.h"
#include "ui_Editor.h"

#include "EditorWidget.h"
#include "ThumbnailWidget.h"
#include "LambdaEventFilter.h"
#include "QtAwesome.h"

#include "LevelData.h"

#include "../appver.h"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QStyle>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>

#include <QtCore/QString>
#include <QtCore/QDebug>

///////////////////////////////////////////////////////////////////////////

static std::unique_ptr<fa::QtAwesome> sAwesome;


using namespace ::SCME;


//////////////////////////////////////////////////////////////////////////

Editor::Editor(const QString& levelToOpen, QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    ui(std::make_unique<Ui::EditorClass>())
{
    ui->setupUi(this);

    mStatusBarCursor = new QLabel(this);
    mStatusBarZoom = new QLabel(this);
    mStatusBarDebug = new QLabel(this);

    ui->statusBar->addPermanentWidget(mStatusBarCursor, 1);
    ui->statusBar->addPermanentWidget(mStatusBarZoom, 1);
    ui->statusBar->addPermanentWidget(mStatusBarDebug, 8);

    connect(this, &Editor::uiError, this, &Editor::onUiError);

    updateWindowTitle();

    connect(ui->changeGridPreset, &QPushButton::clicked, this, &Editor::toggleGridPreset);

    connect(ui->actionNew    , &QAction::triggered, this, &Editor::newLevel);
    connect(ui->actionOpen   , &QAction::triggered, this, qOverload<>(&Editor::openLevel));
    connect(ui->actionSave   , &QAction::triggered, this, &Editor::saveLevel);
    connect(ui->actionSave_As, &QAction::triggered, this, qOverload<>(&Editor::saveLevelAs));
    connect(ui->actionClose  , &QAction::triggered, this, &Editor::closeLevel);

    connect(&mUndoGroup, &QUndoGroup::activeStackChanged, this, &Editor::onUndoStackChanged);
    connect(&mUndoGroup, &QUndoGroup::indexChanged, this, &Editor::onUndoStackChanged);
    connect(&mUndoGroup, &QUndoGroup::cleanChanged, this, &Editor::updateWindowTitle);

    QAction* actionUndo = mUndoGroup.createUndoAction(this);
    QAction* actionRedo = mUndoGroup.createRedoAction(this);

    actionUndo->setIcon(awesome()->icon("rotate-left"));
    actionRedo->setIcon(awesome()->icon("rotate-right"));

    actionUndo->setShortcut(QKeySequence::Undo);
    actionRedo->setShortcut(QKeySequence::Redo);

    ui->menu_Edit->addAction(actionUndo);
    ui->menu_Edit->addAction(actionRedo);

    ui->mainToolBar->addAction(actionUndo);
    ui->mainToolBar->addAction(actionRedo);

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

fa::QtAwesome* Editor::awesome()
{
    if (!sAwesome)
    {
        sAwesome = std::make_unique<fa::QtAwesome>();
        sAwesome->initFontAwesome();
    }

    return sAwesome.get();
}


//////////////////////////////////////////////////////////////////////////

EditorConfig& Editor::config()
{
    return EditorConfig::sGlobalConfig;
}

//////////////////////////////////////////////////////////////////////////

void Editor::onUiError(const QString& message)
{
    qWarning() << "Error:" << message;
    QMessageBox::warning(this, tr("Error"), message);
}

//////////////////////////////////////////////////////////////////////////

void Editor::onUndoStackChanged()
{
    auto stack = mUndoGroup.activeStack();

    if (!stack)
    {
        mStatusBarDebug->setText("<null undo>");
        return;
    }

    mStatusBarDebug->setText(QString("%1/%2 actions: %3").arg(stack->index()).arg(stack->count()).arg(stack->undoText()));
}

//////////////////////////////////////////////////////////////////////////

void Editor::updateStatusCursor(const LevelCoords& coords)
{
    TileCoords xy = coords.tile();

    auto pLevel = level();
    if (pLevel)
        xy = pLevel->boundTileToLevel(xy);

    mStatusBarCursor->setText("(" + QString::number(xy.x()) + "," + QString::number(xy.y()) + ")");
}

//////////////////////////////////////////////////////////////////////////

void Editor::updateStatusZoom(float zoom)
{
    mStatusBarZoom->setText(zoomFactorAsString(zoom));
}

//////////////////////////////////////////////////////////////////////////

void Editor::updateWindowTitle()
{
    QString strTitle = tr("SCME v%1").arg(applicationVersionString());

    if (!mCurrentLevelFile.isEmpty())
    {
        strTitle.append(" - ");

        if (!mUndoGroup.isClean())
            strTitle.append("*");

        strTitle.append(mCurrentLevelFile);
    }

    setWindowTitle(strTitle);
}

//////////////////////////////////////////////////////////////////////////

void Editor::toggleGridPreset()
{
    static int gridPreset_s = EditorConfig::Grey;

    gridPreset_s++;
    gridPreset_s %= EditorConfig::GridPresetCount;

    config().setGridPreset((EditorConfig::GridPreset)gridPreset_s);

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

        mUndoGroup.setActiveStack(mEditorWidget->undoStack());

        connect(mEditorWidget, &EditorWidget::levelTilesetChanged, this, &Editor::initTileset);

        connect(mEditorWidget, &EditorWidget::cursorMoved, this, &Editor::updateStatusCursor);

        connect(mEditorWidget, &EditorWidget::zoomFactorTargetChanged, this, &Editor::updateStatusZoom);
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

        connect(mThumbnailWidget, &ThumbnailWidget::cursorMoved, this, &Editor::updateStatusCursor);

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

        mCurrentLevelFile = tr("<Untitled level>");

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

    std::shared_ptr<LevelData> loadedLevel = std::make_shared<LevelData>();
    bool bLoaded = loadedLevel->loadFromFile(filename);

    qApp->restoreOverrideCursor();

    if (bLoaded)
    {
        Q_ASSERT(!mLevel);
        mLevel = loadedLevel;

        mCurrentLevelFile = filename;

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

    mUndoGroup.activeStack()->setClean();

    qApp->restoreOverrideCursor();

    return false;
}

//////////////////////////////////////////////////////////////////////////

bool Editor::saveLevelAs(const QString& filename)
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    /// ...

    mUndoGroup.activeStack()->setClean();

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

    mCurrentLevelFile = QString();
    onLevelLoaded();

    return !cancel;
}

//////////////////////////////////////////////////////////////////////////

void Editor::onLevelLoaded()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    updateWindowTitle();

    for (const auto& c : mLevelConnections)
        QObject::disconnect(c);
    mLevelConnections.clear();

    if (mEditorWidget)
    {
        mEditorWidget->onLevelChanged();

        if (mLevel)
        {
            mEditorWidget->setDefaultZoom();
            mEditorWidget->setViewCenter(mLevel->bounds().center());

            mLevelConnections = {
                connect(mLevel.get(), &LevelData::tilesChanged, mEditorWidget, &EditorWidget::levelTilesChanged),
            };
        }
        else
        {
            mEditorWidget->update();
        }
    }

    qApp->restoreOverrideCursor();
}

//////////////////////////////////////////////////////////////////////////

QUndoStack* Editor::activeUndoStack() const
{
    return mUndoGroup.activeStack();
}

//////////////////////////////////////////////////////////////////////////

const QUndoGroup* Editor::undoGroup() const
{
    return &mUndoGroup;
}

//////////////////////////////////////////////////////////////////////////

QUndoGroup* Editor::undoGroup()
{
    return &mUndoGroup;
}

//////////////////////////////////////////////////////////////////////////
