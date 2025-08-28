#include <QtTest>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "../src/LevelData.h"

using namespace SCME;



class scme_tests : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase_data()
    {
        QTest::addColumn<QString>("filename");

        QDir dirTestFiles("../testdata/lvl");
        for (const QString& strLevel : dirTestFiles.entryList())
        {
            QTest::newRow(qPrintable(strLevel)) << dirTestFiles.filePath(strLevel);
        }
    }

    void initTestCase()
    {
        //qDebug("Called before the first test function is executed.");
    }

    void init()
    {
        //qDebug("Called before each test function is executed.");
    }

    void test2DArray()
    {
        Array2D<Tile> myArray(100, 200);

        QCOMPARE(myArray.width(), 100);
        QCOMPARE(myArray.height(), 200);

        myArray.resize(QSize(300, 400));
        QCOMPARE(myArray.width(), 300);
        QCOMPARE(myArray.height(), 400);


        LevelData::MapTiles myTiles;
        QCOMPARE(myTiles.width(), 1024);
        QCOMPARE(myTiles.height(), 1024);
    }

    void testLoadLevel()
    {
        QFETCH_GLOBAL(QString, filename);

        qDebug() << "Loading" << filename;

        LevelData data;
        QVERIFY(data.loadFromFile(filename));
    }

    void cleanup()
    {
        //qDebug("Called after every test function.");
    }

    void cleanupTestCase()
    {
        //qDebug("Called after the last test function was executed.");
    }
};

QTEST_MAIN(scme_tests)
#include "scme_tests.moc"
