#include "Test_Checksums.h"

#include "LevelObject.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>

#include <QtCore/QDir>


using namespace SCME;

void Test_Checksums::initTestCase()
{
}

void Test_Checksums::testMd5_Empty()
{
    QCryptographicHash h(QCryptographicHash::Algorithm::Md5);

    QByteArray r1 = h.result();

    h.addData(QByteArray());

    QByteArray r2 = h.result();

    QCOMPARE(r2, r1);
    QCOMPARE(r1.toHex(), "d41d8cd98f00b204e9800998ecf8427e"); //Expected checksum for "nothing"
}

void Test_Checksums::testMd5_LVZ_data()
{
    QTest::addColumn<QByteArray>("md5");

    QTest::newRow("Antimatter.lvz") << QByteArray("8e6201f6b9db9456f2d3921acf3141f6");
    QTest::newRow("Armor.lvz") << QByteArray("66f732ff766ec0b598af7d586aa9f146");
    QTest::newRow("Platforms.lvz") << QByteArray("651794e8d2f072a425ee33e8cb69af0c");
    QTest::newRow("pubtiles.lvz") << QByteArray("978f173864835209aded4a63bb2a984b");
}

void Test_Checksums::testMd5_LVZ()
{
    QDir lvzDir = testDir("lvz");

    QVERIFY(lvzDir.exists());

    QString lvzFile = lvzDir.filePath(QTest::currentDataTag());
    QVERIFY(QFile::exists(lvzFile));

    QCryptographicHash h(QCryptographicHash::Algorithm::Md5);

    QFile f(lvzFile);

    QVERIFY(f.open(QIODevice::ReadOnly));

    h.addData(f.readAll());

    QFETCH(QByteArray, md5);

    QCOMPARE(h.result().toHex(), md5);
}

void Test_Checksums::cleanupTestCase()
{
}
