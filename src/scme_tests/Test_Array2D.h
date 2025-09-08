#include "UnitTest.h"


class Test_Array2D : public UnitTest
{
    Q_OBJECT

private slots:

    void initTestCase();

    void test2DArray();

    void cleanupTestCase();
};


DECLARE_TEST(Test_Array2D)
