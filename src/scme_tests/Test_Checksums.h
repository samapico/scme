#include "UnitTest.h"

class Test_Checksums : public UnitTest
{
    Q_OBJECT
private slots:

    void initTestCase();

    void testMd5_Empty();

    void testMd5_LVZ_data();

    void testMd5_LVZ();

    void cleanupTestCase();
};

DECLARE_TEST(Test_Checksums)
