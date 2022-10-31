/*@
    "name": "test_StarviewerApplicationCommandLine",
    "requirements": ["technical.url_handler", "archive.ris"]
 */

#include "autotest.h"
#include "starviewerapplicationcommandline.h"

using namespace udg;

class test_StarviewerApplicationCommandLine : public QObject {

    Q_OBJECT

private slots:

    void parse_ShouldReturnExpectedValue_data();
    void parse_ShouldReturnExpectedValue();

};

void test_StarviewerApplicationCommandLine::parse_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<bool>("expectedValue");

    QTest::newRow("no arguments") << (QStringList() << "autotests") << true;
    QTest::newRow("uid") << (QStringList() << "autotests" << "-studyinstanceuid" << "1.2.3.4") << true;
    QTest::newRow("invalid uid") << QStringList{"autotests", "-studyinstanceuid", "1.2.3.4.f"} << false;
    QTest::newRow("accession number") << (QStringList() << "autotests" << "-accessionnumber" << "1312") << true;
    QTest::newRow("invalid accession number") << QStringList{"autotests", "-accessionnumber", "13\\\\12"} << false;
    QTest::newRow("url uid") << QStringList{"autotests", "-url", "starviewer://studyinstanceuid/1.398445.33939.4"} << true;
    QTest::newRow("url uid (final slash)") << QStringList{"autotests", "-url", "starviewer://studyinstanceuid/1.398445.33939.4/"} << true;
    QTest::newRow("invalid url uid (invalid uid)") << QStringList{"autotests", "-url", "starviewer://studyinstanceuid/1.398445.33939.4.f"} << false;
    QTest::newRow("invalid url uid (extra path)") << QStringList{"autotests", "-url", "starviewer://studyinstanceuid/1.398445.33939.4/a"} << false;
    QTest::newRow("url uid (query)") << QStringList{"autotests", "-url", "starviewer://studyinstanceuid/1.398445.33939.4?a=b"} << true;
    QTest::newRow("url uid (anchor)") << QStringList{"autotests", "-url", "starviewer://studyinstanceuid/1.398445.33939.4#top"} << true;
    QTest::newRow("url accession") << QStringList{"autotests", "-url", "starviewer://accessionnumber/498346745"} << true;
    QTest::newRow("url accession (final slash)") << QStringList{"autotests", "-url", "starviewer://accessionnumber/498346745/"} << true;
    QTest::newRow("invalid url accession (invalid accession)") << QStringList{"autotests", "-url", "starviewer://accessionnumber/4983\\\\46745"} << false;
    QTest::newRow("invalid url accession (extra path)") << QStringList{"autotests", "-url", "starviewer://accessionnumber/498346745/a"} << false;
    QTest::newRow("url accession (query)") << QStringList{"autotests", "-url", "starviewer://accessionnumber/498346745?a=b"} << true;
    QTest::newRow("url accession (anchor)") << QStringList{"autotests", "-url", "starviewer://accessionnumber/498346745#top"} << true;
    QTest::newRow("invalid url") << QStringList{"autotests", "-url", "startviwer://foo/bar"} << false;
    QTest::newRow("several") << (QStringList() << "autotests" << "-accessionnumber" << "1714" << "-studyinstanceuid" << "1.1.1.2") << true;
    QTest::newRow("unknown option") << (QStringList() << "autotests" << "-foo" << "bar") << false;
    QTest::newRow("missing value") << (QStringList() << "autotests" << "-studyinstanceuid") << false;
    QTest::newRow("double dash") << (QStringList() << "autotests" << "--studyinstanceuid" << "1.1.1.1" << "--accessionnumber" << "20171001") << true;
}

void test_StarviewerApplicationCommandLine::parse_ShouldReturnExpectedValue()
{
    QFETCH(QStringList, arguments);
    QFETCH(bool, expectedValue);

    StarviewerApplicationCommandLine commandLine;
    QString errorText;

    QCOMPARE(commandLine.parse(arguments, errorText), expectedValue);
}

DECLARE_TEST(test_StarviewerApplicationCommandLine)

#include "test_starviewerapplicationcommandline.moc"
