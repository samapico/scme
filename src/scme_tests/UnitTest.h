#ifndef INC_UnitTest_H
#define INC_UnitTest_H

#include "AutoTest.h"

#include <QtCore/QObject>
#include <QTest>
#include <QtCore/QFile>

/// Common utilities for unit tests
class UnitTest : public QObject
{
public:
    UnitTest(QObject* parent = nullptr) : QObject(parent)
    {
    }

    ~UnitTest() override = default;

protected:

    QDir testDataRootDir() const;

    QString testFilePath(const QString& filename) const;

    QDir testDir(const QString& dirname) const;

    QFile testFile(const QString& filename) const;
};

#endif // INC_UnitTest_H