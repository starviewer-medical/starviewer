#include "autotest.h"
#include "imageoverlay.h"

using namespace udg;

class test_ImageOverlay : public QObject {
Q_OBJECT

private slots:
    void ImageOverlay_InitializesClassAsExpected();
    
    void setType_OnlyAdmitsStipulatedValues_data();
    void setType_OnlyAdmitsStipulatedValues();

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
    QCOMPARE(overlay.getType(), QChar());
}

void test_ImageOverlay::setType_OnlyAdmitsStipulatedValues_data()
{
    QTest::addColumn<QChar>("type");
    QTest::addColumn<QChar>("result");

    QTest::newRow("Graphics enum") << QChar('G') << QChar('G');
    QTest::newRow("ROI enum") << QChar('R') << QChar('R');
    
    QTest::newRow("empty value") << QChar() << QChar();
    // ASCII 71 == 'G'
    for (char c = 0; c < 71; ++c)
    {
        QTest::newRow(qPrintable(QString("wrong value '%1'").arg(c))) << QChar::fromAscii(c) << QChar();
    }
    // ASCII 82 == 'R'
    for (char c = 72; c < 82; ++c)
    {
        QTest::newRow(qPrintable(QString("wrong value '%1'").arg(c))) << QChar::fromAscii(c) << QChar();
    }
    // La resta de caràcters ASCII
    for (char c = 83; c < 127; ++c)
    {
        QTest::newRow(qPrintable(QString("wrong value '%1'").arg(c))) << QChar::fromAscii(c) << QChar();
    }
}

void test_ImageOverlay::setType_OnlyAdmitsStipulatedValues()
{
    QFETCH(QChar, type);
    QFETCH(QChar, result);

    ImageOverlay overlay;
    overlay.setType(type);
    
    QCOMPARE(overlay.getType(), result);
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
