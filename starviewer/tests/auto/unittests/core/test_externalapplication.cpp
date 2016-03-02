#include "autotest.h"

#include "externalapplication.h"

using namespace udg;

class test_ExternalApplication : public QObject {
Q_OBJECT
private slots:
    
    void getReplacedUrl_ReturnsExpectedValues_data();
    void getReplacedUrl_ReturnsExpectedValues();
};


Q_DECLARE_METATYPE(ExternalApplication)

void test_ExternalApplication::getReplacedUrl_ReturnsExpectedValues_data()
{
    QTest::addColumn<ExternalApplication>("extApp");
    QTest::addColumn<QHash<QString,QString>>("replacements");
    QTest::addColumn<QString>("expectedString");

    QHash<QString,QString> replacements;

    replacements = QHash<QString,QString>();
    QTest::newRow("test0.1") << ExternalApplication() << replacements << QString();
    QTest::newRow("test0.2") << ExternalApplication(QString(),QString()) << replacements << QString();
    QTest::newRow("test0.3") << ExternalApplication("","") << replacements << "";
    QTest::newRow("test0.4") << ExternalApplication("test","http://example.com") << replacements << "http://example.com";
    QTest::newRow("test0.5") << ExternalApplication("test","http://example{%dummy%}.com") << replacements << "http://example.com";

    replacements = QHash<QString,QString>();
    replacements["foo"] = "bar";
    QTest::newRow("test1.1") << ExternalApplication("test","http://example.com/{%dummy%}") << replacements << "http://example.com/";
    QTest::newRow("test1.2") << ExternalApplication("test","{%foo%}") << replacements << "bar";
    QTest::newRow("test1.3") << ExternalApplication("test","http://example.com/{%foo%}") << replacements << "http://example.com/bar";
    QTest::newRow("test1.4") << ExternalApplication("test","http://example.com/{%foo%}{%foo%}") << replacements << "http://example.com/barbar";
    QTest::newRow("test1.5") << ExternalApplication("test","http://example.com/{%foo%}foo%}") << replacements << "http://example.com/barfoo%}";

    replacements = QHash<QString,QString>();
    replacements["thug"] = "life";
    replacements["nyan"] = "cat";
    replacements["n"] = "";
    replacements["%"] = "percent";
    replacements["{%"] = "percentopen";
    QTest::newRow("test2.1") << ExternalApplication("test","http://example.com/{%nyan%}{%thug%}") << replacements << "http://example.com/catlife";
    QTest::newRow("test2.2") << ExternalApplication("test","{%nyan%}://example.com/") << replacements << "cat://example.com/";
    QTest::newRow("test2.3") << ExternalApplication("test","{%n%}") << replacements << "";
    QTest::newRow("test2.4") << ExternalApplication("test","a{%n%}") << replacements << "a";
    QTest::newRow("test2.5") << ExternalApplication("test","{%n%}a") << replacements << "a";
    QTest::newRow("test2.6") << ExternalApplication("test","{%nyan%}{%n%}") << replacements << "cat";
    QTest::newRow("test2.7") << ExternalApplication("test","{%n%}{%nyan%}") << replacements << "cat";
    QTest::newRow("test2.8") << ExternalApplication("test","{%%}") << replacements << "";
    QTest::newRow("test2.9") << ExternalApplication("test","{%%%}") << replacements << "percent";
    QTest::newRow("test2.10") << ExternalApplication("test","{%{%%}") << replacements << "percentopen";
}

void test_ExternalApplication::getReplacedUrl_ReturnsExpectedValues()
{
    typedef QHash<QString,QString> ReplacementsType; //TODO: mirar si es pot posar directe al QFETCH
    QFETCH(ExternalApplication, extApp);
    QFETCH(ReplacementsType, replacements);
    QFETCH(QString, expectedString);

    QCOMPARE(extApp.getReplacedUrl(replacements), expectedString);
}


DECLARE_TEST(test_ExternalApplication)

#include "test_externalapplication.moc"
