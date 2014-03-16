#include "editor.h"
#include "glWidget.h"
#include "appver.h"

//////////////////////////////////////////////////////////////////////////

const int Editor::TILE_WIDTH(16);
const int Editor::TILE_HEIGHT(16);
const QSize Editor::TILE_SIZE(16,16);

//////////////////////////////////////////////////////////////////////////

Editor::Editor(QWidget *parent, Qt::WFlags flags) :
    QMainWindow(parent, flags),
    mLevelSize(1024, 1024)
{
    mLevelPixelSize = QSize(mLevelSize.width() * TILE_WIDTH, mLevelSize.height() * TILE_HEIGHT);

    ui.setupUi(this);

    setCentralWidget(new GLWidget(this));

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
