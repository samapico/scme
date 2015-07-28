#include "Editor.h"
#include "EditorWidget.h"
#include "ThumbnailWidget.h"

#include "LevelData.h"

#include "appver.h"

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include <QtCore/QString>
#include <QtCore/QDebug>

///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

const int Editor::TILE_WIDTH(16);
const int Editor::TILE_HEIGHT(16);
const QSize Editor::TILE_SIZE(16,16);

//////////////////////////////////////////////////////////////////////////

Editor::Editor(QWidget *parent, Qt::WFlags flags) :
    QMainWindow(parent, flags),
    mEditorWidget(0),
    mThumbnailWidget(0),
    mLevel(0)
{

    ui.setupUi(this);

    setWindowTitle(tr("SCME v%1.%2.%3.%4%5").arg(
        QString::number(APP_VERSION_MAJOR),
        QString::number(APP_VERSION_MINOR),
        QString::number(APP_VERSION_REV),
        QString::number(APP_VERSION_BUILD),
#ifdef _DEBUG
        tr(" (DEBUG)")
#else
        QString()
#endif
        ));

    bool bConnect = true;

    bConnect &= connect(ui.changeGridPreset, SIGNAL(clicked()), this, SLOT(toggleGridPreset()));

    bConnect &= connect(ui.actionNew    , SIGNAL(triggered()), this, SLOT(newLevel()));
    bConnect &= connect(ui.actionOpen   , SIGNAL(triggered()), this, SLOT(openLevel()));
    bConnect &= connect(ui.actionSave   , SIGNAL(triggered()), this, SLOT(saveLevel()));
    bConnect &= connect(ui.actionSave_As, SIGNAL(triggered()), this, SLOT(saveLevelAs()));
    bConnect &= connect(ui.actionClose  , SIGNAL(triggered()), this, SLOT(closeLevel()));

    //Create new level
    newLevel();
    
    Q_ASSERT(bConnect);
}

//////////////////////////////////////////////////////////////////////////

Editor::~Editor()
{
    delete mLevel;
}

//////////////////////////////////////////////////////////////////////////

QPoint Editor::boundPixelToLevel(const QPoint& pixel) const
{
    QPoint bounded = pixel;

    if (bounded.x() < 0)
        bounded.setX(0);
    else if (bounded.x() > levelPixelSize().width())
        bounded.setX(levelPixelSize().width());

    if (bounded.y() < 0)
        bounded.setY(0);
    else if (bounded.y() > levelPixelSize().height())
        bounded.setY(levelPixelSize().height());

    return bounded;
}

//////////////////////////////////////////////////////////////////////////

void Editor::toggleGridPreset()
{
    static int gridPreset_s = -1;

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
        connect(mThumbnailWidget, SIGNAL(doCenterView(const QPoint&)), mEditorWidget, SLOT(setViewCenterSmooth(const QPoint&)));
        connect(mEditorWidget, SIGNAL(viewMoved(const QRect&)), mThumbnailWidget, SLOT(redrawView(const QRect&)));

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
        mLevelPixelSize = QSize(mLevel->size().width() * TILE_WIDTH, mLevel->size().height() * TILE_HEIGHT);
    else
        mLevelPixelSize = QSize(0,0);

    initEditorWidget();
    initTileset();
    initRadar();

    if (mEditorWidget)
    {
        if (mLevel)
        {
            mEditorWidget->setZoomFactor(1.0);
            mEditorWidget->setViewCenter(QPoint(levelPixelSize().width() / 2, levelPixelSize().height() / 2));
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
