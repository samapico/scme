#include "UnitTest.h"

class Test_LevelData : public UnitTest
{
    Q_OBJECT

private:
    void TestFunc(QString& outMessage);

private slots:

    void initTestCase();

    void testLoadLevel_data();

    void testLoadLevel();

    void cleanupTestCase();
};

DECLARE_TEST(Test_LevelData)
