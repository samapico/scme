#include "UnitTest.h"

QDir UnitTest::testDataRootDir() const
{
    //Working directory is either the output directory (e.g. "bin") or the project's directory (e.g. "src/scme_tests")
    QDir cd(".");

    for (const QString& relpath : {
        QDir(QCoreApplication::applicationDirPath()).filePath("../testdata"), //This one should always work
        QString("../../testdata"), //from src/scme_tests
        QString("../testdata"),    //from bin
        })
    {
        if (cd.exists(relpath))
            return QDir(cd.filePath(relpath));
    }

    Q_ASSERT(0 && "testDataRootDir not found");
    return QDir();
}


QString UnitTest::testFilePath(const QString& filename) const
{
    return testDataRootDir().filePath(filename);
}


QDir UnitTest::testDir(const QString& dirname) const
{
    return QDir(testDataRootDir().filePath(dirname));
}


QFile UnitTest::testFile(const QString& filename) const
{
    return QFile(testFilePath(filename));
}
