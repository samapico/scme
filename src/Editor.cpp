#include "Editor.h"
#include "EditorWidget.h"
#include "ThumbnailWidget.h"

#include "LevelData.h"

#include "appver.h"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

#include <QtCore/QString>
#include <QtCore/QDebug>

///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

Editor::Editor(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    mEditorWidget(0),
    mThumbnailWidget(0),
    mLevel(0)
{

    ui.setupUi(this);

    setWindowTitle(tr("SCME v%1.%2.%3%5").arg(
        QString::number(APP_VERSION_MAJOR),
        QString::number(APP_VERSION_MINOR),
        QString::number(APP_VERSION_REV),
#ifdef _DEBUG
        tr(" (DEBUG)")
#else
        QString()
#endif
        ));

    connect(ui.changeGridPreset, &QPushButton::clicked, this, &Editor::toggleGridPreset);

    connect(ui.actionNew    , &QAction::triggered, this, &Editor::newLevel);
    connect(ui.actionOpen   , &QAction::triggered, this, &Editor::openLevel);
    connect(ui.actionSave   , &QAction::triggered, this, &Editor::saveLevel);
    connect(ui.actionSave_As, &QAction::triggered, this, &Editor::saveLevelAs);
    connect(ui.actionClose  , &QAction::triggered, this, &Editor::closeLevel);

    //Create new level
    newLevel();
}

//////////////////////////////////////////////////////////////////////////

Editor::~Editor()
{
    delete mLevel;
}

//////////////////////////////////////////////////////////////////////////

LevelCoords Editor::boundPixelToLevel(const LevelCoords& pixel) const
{
    return mLevelBounds.bounded(pixel);
}

//////////////////////////////////////////////////////////////////////////

ScreenCoords Editor::boundScreenPixelToLevel(const ScreenCoords& screenPixel) const
{
    LevelCoords level = screenPixel.toLevel();

    LevelCoords levelBounded = boundPixelToLevel(level);

    if (level != levelBounded)
        return levelBounded.toScreen(mEditorWidget);

    return screenPixel;
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
    }
}

//////////////////////////////////////////////////////////////////////////

void Editor::initRadar()
{
    delete mThumbnailWidget;
    mThumbnailWidget = 0;

    if (mLevel)
    {
        mThumbnailWidget = new ThumbnailWidget(this);

        Q_ASSERT(mEditorWidget);
        connect(mThumbnailWidget, &ThumbnailWidget::doCenterView, mEditorWidget, &EditorWidget::setViewCenterSmooth, Qt::QueuedConnection);
        connect(mEditorWidget, &EditorWidget::viewMoved, mThumbnailWidget, &ThumbnailWidget::redrawView, Qt::QueuedConnection);

        ui.dockRadar->setWidget(mThumbnailWidget);
    }
}

//////////////////////////////////////////////////////////////////////////

void Editor::initTileset()
{
    if (mLevel)
        ui.labelTileset->setPixmap(QPixmap::fromImage(mLevel->tileset().image()));
    else
        ui.labelTileset->setPixmap(QPixmap());
}

//////////////////////////////////////////////////////////////////////////

void Editor::newLevel()
{
    if (closeLevel())
    {
        Q_ASSERT(!mLevel);
        mLevel = new LevelData;

        onLevelLoaded();
    }
}

//////////////////////////////////////////////////////////////////////////

void Editor::openLevel()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Levels (*.lvl)"));

    if (!path.isEmpty())
    {
        if (closeLevel())
        {
            Q_ASSERT(!mLevel);
            mLevel = new LevelData;
            if (!mLevel->loadFromFile(path))
            {
                qWarning() << "Error opening '" << path << "'";
            }

            onLevelLoaded();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void Editor::saveLevel()
{

}

//////////////////////////////////////////////////////////////////////////

void Editor::saveLevelAs()
{

}

//////////////////////////////////////////////////////////////////////////

bool Editor::closeLevel()
{
    bool cancel = false;

    if (mLevel && mLevel->isDirty())
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
        delete mLevel;
        mLevel = 0;
    }

    onLevelLoaded();

    return !cancel;
}

//////////////////////////////////////////////////////////////////////////

void Editor::onLevelLoaded()
{
    if (mLevel)
        mLevelBounds = LevelBounds(LevelCoords(0, 0), QSizeF(mLevel->size().width() * TILE_W, mLevel->size().height() * TILE_H));
    else
        mLevelBounds = LevelBounds();

    initEditorWidget();
    initTileset();
    initRadar();

    if (mEditorWidget)
    {
        if (mLevel)
        {
            mEditorWidget->setDefaultZoom();
            mEditorWidget->setViewCenter(mLevelBounds.center());
        }
        else
        {
            mEditorWidget->update();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

QSize Editor::levelSize() const
{
    return mLevel ? mLevel->size() : QSize(0, 0);
}
