#include "Test_Array2D.h"

#include "Array2D.h"
#include "LevelData.h"

using namespace SCME;

void Test_Array2D::initTestCase()
{
}

void Test_Array2D::test2DArray()
{
    Array2D<Tile> myArray(100, 200);

    QCOMPARE(myArray.width(), 100);
    QCOMPARE(myArray.height(), 200);

    myArray.resize(QSize(300, 400));
    QCOMPARE(myArray.width(), 300);
    QCOMPARE(myArray.height(), 400);

    LevelTiles myTiles;
    QCOMPARE(myTiles.width(), 1024);
    QCOMPARE(myTiles.height(), 1024);
}

void Test_Array2D::cleanupTestCase()
{
    //qDebug("Called after the last test function was executed.");
}

