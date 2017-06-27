#include "autotest.h"
#include "applicationversionchecker.h"

using namespace udg;

class test_ApplicationVersionChecker : public QObject {

    Q_OBJECT

private slots:

    void isNewerVersion_ShouldReturnExpectedValue_data();
    void isNewerVersion_ShouldReturnExpectedValue();

};

void test_ApplicationVersionChecker::isNewerVersion_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QString>("currentVersion");
    QTest::addColumn<QString>("oldVersion");
    QTest::addColumn<bool>("expectedValue");

    QTest::newRow("both empty") << QString("") << QString("") << false;
    QTest::newRow("current empty") << QString("") << QString("0") << false;
    QTest::newRow("old empty") << QString("0") << QString("") << true;
    QTest::newRow("higher major") << QString("1.0.0") << QString("0.14.0") << true;
    QTest::newRow("lower major") << QString("0.14.0") << QString("1.0.0") << false;
    QTest::newRow("higher minor") << QString("0.14.0") << QString("0.13.3") << true;
    QTest::newRow("lower minor") << QString("0.13.3") << QString("0.14.0") << false;
    QTest::newRow("higher patch") << QString("0.13.3") << QString("0.13.2") << true;
    QTest::newRow("lower patch") << QString("0.13.2") << QString("0.13.3") << false;
    QTest::newRow("extra number in current") << QString("0.11.0.1") << QString("0.11.0") << true;
    QTest::newRow("extra number in old") << QString("0.11.0") << QString("0.11.0.1") << false;
    QTest::newRow("same number no suffix") << QString("0.14.0") << QString("0.14.0") << false;
    QTest::newRow("current has suffix") << QString("0.14.0-devel") << QString("0.14.0") << false;
    QTest::newRow("old has suffix") << QString("0.14.0") << QString("0.14.0-beta3") << true;
    QTest::newRow("devel vs devel") << QString("1.0-devel") << QString("1.0-devel") << false;
    QTest::newRow("devel vs alpha") << QString("1.0-devel") << QString("1.0-alpha1") << false;
    QTest::newRow("devel vs beta") << QString("1.0-devel") << QString("1.0-beta2") << false;
    QTest::newRow("devel vs RC") << QString("1.0-devel") << QString("1.0-RC3") << false;
    QTest::newRow("alpha vs devel") << QString("1.0-alpha1") << QString("1.0-devel") << true;
    QTest::newRow("alpha vs alpha") << QString("1.0-alpha2") << QString("1.0-alpha2") << false;
    QTest::newRow("alpha1 vs alpha2") << QString("1.0-alpha1") << QString("1.0-alpha2") << false;
    QTest::newRow("alpha2 vs alpha1") << QString("1.0-alpha2") << QString("1.0-alpha1") << true;
    QTest::newRow("alpha vs beta") << QString("1.0-alpha3") << QString("1.0-beta2") << false;
    QTest::newRow("alpha vs RC") << QString("1.0-alpha4") << QString("1.0-RC3") << false;
    QTest::newRow("beta vs devel") << QString("1.0-beta1") << QString("1.0-devel") << true;
    QTest::newRow("beta vs alpha") << QString("1.0-beta2") << QString("1.0-alpha1") << true;
    QTest::newRow("beta vs beta") << QString("1.0-beta3") << QString("1.0-beta3") << false;
    QTest::newRow("beta1 vs beta2") << QString("1.0-beta1") << QString("1.0-beta2") << false;
    QTest::newRow("beta2 vs beta1") << QString("1.0-beta2") << QString("1.0-beta1") << true;
    QTest::newRow("beta vs RC") << QString("1.0-beta4") << QString("1.0-RC3") << false;
    QTest::newRow("RC vs devel") << QString("1.0-RC1") << QString("1.0-devel") << true;
    QTest::newRow("RC vs alpha") << QString("1.0-RC2") << QString("1.0-alpha1") << true;
    QTest::newRow("RC vs beta") << QString("1.0-RC3") << QString("1.0-beta2") << true;
    QTest::newRow("RC vs RC") << QString("1.0-RC4") << QString("1.0-RC4") << false;
    QTest::newRow("RC1 vs RC2") << QString("1.0-RC1") << QString("1.0-RC2") << false;
    QTest::newRow("RC2 vs RC1") << QString("1.0-RC2") << QString("1.0-RC1") << true;
    QTest::newRow("unexpected suffix in current") << QString("1.0-foo") << QString("1.0-devel") << false;
    QTest::newRow("unexpected suffix in old") << QString("1.0-devel") << QString("1.0-bar") << false;
    QTest::newRow("unexpected suffix in both") << QString("1.0-foo") << QString("1.0-bar") << false;
}

void test_ApplicationVersionChecker::isNewerVersion_ShouldReturnExpectedValue()
{
    QFETCH(QString, currentVersion);
    QFETCH(QString, oldVersion);
    QFETCH(bool, expectedValue);

    QCOMPARE(ApplicationVersionChecker::isNewerVersion(currentVersion, oldVersion), expectedValue);
}

DECLARE_TEST(test_ApplicationVersionChecker)

#include "test_applicationversionchecker.moc"
