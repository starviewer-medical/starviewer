#include "autotest.h"
#include "imageorientation.h"

using namespace udg;

class test_ImageOrientation : public QObject {
Q_OBJECT

private slots:
    // bool setDICOMFormattedImageOrientation(const QString &imageOrientation);
    void setDICOMFormattedImageOrientation_ShouldReturnTrue_data();
    void setDICOMFormattedImageOrientation_ShouldReturnTrue();
    void setDICOMFormattedImageOrientation_ShouldReturnFalse_data();
    void setDICOMFormattedImageOrientation_ShouldReturnFalse();
};

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldReturnTrue_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<bool>("result");

    QTest::newRow("six random number values correctly formatted") << "1\\2\\3\\0\\0\\0" << true;
    QTest::newRow("six random floating point number values correctly formatted") << "1.0\\2.1\\3.\\.0\\.0\\3.5" << true;
    QTest::newRow("six random string values correctly formatted") << "aaa\\bbbb\\ccc\\dddd\\eeee\\ffff" << true;
    QTest::newRow("six random alphanumeric values correctly formatted") << "aaa\\1\\bbbb\\.2\\cccc\\3.2" << true;
    QTest::newRow("six random alphanumeric values correctly formatted with empty elements") << "aaa\\1\\\\.2\\cccc\\3.2" << true;
}

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldReturnTrue()
{
    QFETCH(QString, string);
    QFETCH(bool, result);

    ImageOrientation imageOrientation;
    QCOMPARE(imageOrientation.setDICOMFormattedImageOrientation(string), result);
}

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldReturnFalse_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<bool>("result");

    QTest::newRow("empty string") << "" << false;
    QTest::newRow("less than 6 elements") << "1.0\\2.1\\3." << false;
    QTest::newRow("more than 6 elements") << "aaa\\bbbb\\ccc\\dddd\\eeee\\ffff\\123\\1.8" << false;
    QTest::newRow("extra separator at the end") << "aaa\\bbbb\\ccc\\dddd\\eeee\\ffff\\" << false;
    QTest::newRow("extra separator at the beggining") << "\\aaa\\bbbb\\ccc\\dddd\\eeee\\ffff" << false;
    QTest::newRow("extra separator at the beggining and at the end") << "\\aaa\\bbbb\\ccc\\dddd\\eeee\\ffff\\" << false;
}

void test_ImageOrientation::setDICOMFormattedImageOrientation_ShouldReturnFalse()
{
    QFETCH(QString, string);
    QFETCH(bool, result);

    ImageOrientation imageOrientation;
    QCOMPARE(imageOrientation.setDICOMFormattedImageOrientation(string), result);
}

DECLARE_TEST(test_ImageOrientation)

#include "test_imageorientation.moc"
