#include "Editor.h"
#include "EditorWidget.h"
#include "ThumbnailWidget.h"

#include "appver.h"

//////////////////////////////////////////////////////////////////////////

const int Editor::TILE_WIDTH(16);
const int Editor::TILE_HEIGHT(16);
const QSize Editor::TILE_SIZE(16,16);

//////////////////////////////////////////////////////////////////////////

Editor::Editor(QWidget *parent, Qt::WFlags flags) :
    QMainWindow(parent, flags),
    mLevelSize(1024, 1024),
    mEditorWidget(0),
    mThumbnailWidget(0)
{
    mLevelPixelSize = QSize(mLevelSize.width() * TILE_WIDTH, mLevelSize.height() * TILE_HEIGHT);

    ui.setupUi(this);

    mEditorWidget = new EditorWidget(this);
    setCentralWidget(mEditorWidget);

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

    initRadar();

    bool bConnect = true;
    
    bConnect &= connect(mEditorWidget, SIGNAL(viewMoved(const QRect&)), mThumbnailWidget, SLOT(redrawView(const QRect&)));
    bConnect &= connect(mThumbnailWidget, SIGNAL(doCenterView(const QPoint&)), mEditorWidget, SLOT(setCenter(const QPoint&)));

    bConnect &= connect(ui.changeGridPreset, SIGNAL(clicked()), this, SLOT(toggleGridPreset()));

    Q_ASSERT(bConnect);
}

//////////////////////////////////////////////////////////////////////////

Editor::~Editor()
{

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

    centralWidget()->update();
}

//////////////////////////////////////////////////////////////////////////

void Editor::initRadar()
{
    mThumbnailWidget = new ThumbnailWidget(this);
    ui.dockRadar->setWidget(mThumbnailWidget);
}
