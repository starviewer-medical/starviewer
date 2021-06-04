/*@
    "name": "test_StarviewerApplicationCommandLine",
    "requirements": ["archive.ris"]
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
    QTest::newRow("accession number") << (QStringList() << "autotests" << "-accessionnumber" << "1312") << true;
    QTest::newRow("both") << (QStringList() << "autotests" << "-accessionnumber" << "1714" << "-studyinstanceuid" << "1.1.1.2") << true;
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
