#include "Test_LevelData.h"

#include "LevelData.h"
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>

#include <QtCore/QDir>


using namespace SCME;

void Test_LevelData::initTestCase()
{
}

void Test_LevelData::testLoadLevel_data()
{
    //qDebug("Called before the first test function is executed.");
    QTest::addColumn<QString>("filename");

    QDir dirTestFiles = testDir("lvl");
    for (const QString& strLevel : dirTestFiles.entryList(QDir::Files))
    {
        QTest::newRow(qPrintable(strLevel)) << dirTestFiles.filePath(strLevel);
    }
}

void Test_LevelData::testLoadLevel()
{
    QFETCH(QString, filename);

    LevelData data;
    QVERIFY(data.loadFromFile(filename));
}

void Test_LevelData::cleanupTestCase()
{
}
