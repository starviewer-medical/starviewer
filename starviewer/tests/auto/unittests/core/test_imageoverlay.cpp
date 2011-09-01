#include "autotest.h"
#include "imageoverlay.h"

using namespace udg;

class test_ImageOverlay : public QObject {
Q_OBJECT

private slots:
    void ImageOverlay_InitializesClassAsExpected();
    
    void setDICOMFormattedOrigin_ShouldBeUpdated_data();
    void setDICOMFormattedOrigin_ShouldBeUpdated();

    void setDICOMFormattedOrigin_WrongFormattedOriginShouldNotModifyOrigin_data();
    void setDICOMFormattedOrigin_WrongFormattedOriginShouldNotModifyOrigin();
};

void test_ImageOverlay::ImageOverlay_InitializesClassAsExpected()
{    
    ImageOverlay overlay;
    QCOMPARE(overlay.getRows(), unsigned int(0));
    QCOMPARE(overlay.getColumns(), unsigned int(0));
    QCOMPARE(overlay.getXOrigin(), 1);
    QCOMPARE(overlay.getYOrigin(), 1);
}

void test_ImageOverlay::setDICOMFormattedOrigin_ShouldBeUpdated_data()
{
    QTest::addColumn<QString>("origin");
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");

    QTest::newRow("1,1") << "1\\1" << 1 << 1;
    QTest::newRow("0,0") << "0\\0" << 0 << 0;
    QTest::newRow("-1,1") << "-1\\1" << -1 << 1;
    QTest::newRow("1,-1") << "1\\-1" << 1 << -1;
    QTest::newRow("134,92") << "134\\92" << 134 << 92;
    QTest::newRow("-5984,0") << "-5984\\0" << -5984 << 0;
    QTest::newRow("4,-4873") << "4\\-4873" << 4 << -4873;
}

void test_ImageOverlay::setDICOMFormattedOrigin_ShouldBeUpdated()
{
    QFETCH(QString, origin);
    QFETCH(int, x);
    QFETCH(int, y);

    ImageOverlay overlay;
    overlay.setDICOMFormattedOrigin(origin);
    
    QCOMPARE(overlay.getXOrigin(), x);
    QCOMPARE(overlay.getYOrigin(), y);
}

void test_ImageOverlay::setDICOMFormattedOrigin_WrongFormattedOriginShouldNotModifyOrigin_data()
{
    QTest::addColumn<QString>("origin");

    QTest::newRow("empty string") << "";
    QTest::newRow("only one digit") << "123";
    QTest::newRow("first element is empty") << "\\123";
    QTest::newRow("second element is empty") << "123\\";
    QTest::newRow("only contains separator") << "\\";
    QTest::newRow("contains float numbers") << "123.2\\12";
    QTest::newRow("contains non-digit characters") << "ABC\\12";
    QTest::newRow("contains non-digit characters 2") << "12e+33\\12";
}

void test_ImageOverlay::setDICOMFormattedOrigin_WrongFormattedOriginShouldNotModifyOrigin()
{
    QFETCH(QString, origin);

    ImageOverlay overlay;
    int x = overlay.getXOrigin();
    int y = overlay.getYOrigin();
    overlay.setDICOMFormattedOrigin(origin);

    QCOMPARE(overlay.getXOrigin(), x);
    QCOMPARE(overlay.getYOrigin(), y);
}

DECLARE_TEST(test_ImageOverlay)

#include "test_imageoverlay.moc"
