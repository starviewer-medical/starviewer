#include "autotest.h"
#include "applicationupdatechecker.h"
#include <QNetworkReply>
#include <QUrl>

using namespace udg;

class TestingQNetworkReply : public QNetworkReply {
public:

    TestingQNetworkReply(QObject *parent = 0)
     : QNetworkReply(parent)
    {
    }

    void setTestingError(QNetworkReply::NetworkError errorCode, const QString &errorString)
    {
        setError(errorCode, errorString);
    }

    // Necessari per que la classe no sigui abstracte i es pugui instanciar
    qint64 readData(char *data, qint64 maxSize)
    {
        Q_UNUSED(data);
        Q_UNUSED(maxSize);
        return 0;
    }

    // Necessari per que la classe no sigui abstracte i es pugui instanciar
    void abort(){}
};

class TestingApplicationUpdateChecker : public ApplicationUpdateChecker {
public:
    QNetworkReply::NetworkError m_testingReplyErrorCode;
    QString m_testingReplyErrorString;
    bool m_testingTimeOut;
    QString m_testingReplyData;

protected:
    void performOnlinePetition(const QUrl &url)
    {
        Q_UNUSED(url);
        TestingQNetworkReply *reply = new TestingQNetworkReply();
        reply->setTestingError(m_testingReplyErrorCode, m_testingReplyErrorString);
        
        if (m_testingTimeOut)
        {
            checkForUpdatesReplyTimeout();
        }
        else
        {
            checkForUpdatesReply(reply);
        }
    }

    QString readReplyData(QNetworkReply *reply)
    {
        Q_UNUSED(reply);
        return m_testingReplyData;
    }
};

Q_DECLARE_METATYPE(QNetworkReply::NetworkError)

class test_ApplicationUpdateChecker : public QObject {
Q_OBJECT

private slots:
    void run_ShouldCheckForUpdates_data();
    void run_ShouldCheckForUpdates();

    void run_ShouldCheckForUpdatesAndFail_data();
    void run_ShouldCheckForUpdatesAndFail();
};

void test_ApplicationUpdateChecker::run_ShouldCheckForUpdates_data()
{
   // Entrada
    QTest::addColumn<QNetworkReply::NetworkError>("testingReplyErrorCode");
    QTest::addColumn<QString>("testingReplyErrorString");
    QTest::addColumn<bool>("testingTimeOut");
    QTest::addColumn<QString>("testingReplyData");

    // Sortida    
    QTest::addColumn<bool>("testingIsOnlineCheckOk");

    // Unused
    QString unusedString = "";

    QString json = "{\"updateAvailable\":true,\"version\":\"0.10.1\",\"releaseNotesURL\":\"http://starviewer.udg.edu/releasenotes/releasenotes-0.10.1.html\"}";
    QTest::newRow("update available") << QNetworkReply::NoError << unusedString << false << json
                                      << true;
    json = "{\"updateAvailable\":false}";
    QTest::newRow("up to date") << QNetworkReply::NoError << unusedString << false << json
                                << true;
}

void test_ApplicationUpdateChecker::run_ShouldCheckForUpdates()
{
    QFETCH(QNetworkReply::NetworkError, testingReplyErrorCode);
    QFETCH(QString, testingReplyErrorString);
    QFETCH(bool, testingTimeOut);
    QFETCH(QString, testingReplyData);

    QFETCH(bool, testingIsOnlineCheckOk);

    TestingApplicationUpdateChecker checker;
    checker.m_testingReplyErrorCode = testingReplyErrorCode;
    checker.m_testingReplyErrorString = testingReplyErrorString;
    checker.m_testingTimeOut = testingTimeOut;
    checker.m_testingReplyData = testingReplyData;
    checker.checkForUpdates();

    QCOMPARE(checker.isOnlineCheckOk(), testingIsOnlineCheckOk);
}

void test_ApplicationUpdateChecker::run_ShouldCheckForUpdatesAndFail_data()
{
    // Entrada
    QTest::addColumn<QNetworkReply::NetworkError>("testingReplyErrorCode");
    QTest::addColumn<QString>("testingReplyErrorString");
    QTest::addColumn<bool>("testingTimeOut");
    QTest::addColumn<QString>("testingReplyData");

    // Sortida    
    QTest::addColumn<bool>("testingIsOnlineCheckOk");
    QTest::addColumn<QString>("testingErrorDescription");

    // Unused
    QNetworkReply::NetworkError unusedNetworkError = QNetworkReply::ConnectionRefusedError;
    QString unusedString = "";

    QTest::newRow("error") << QNetworkReply::ConnectionRefusedError << "Connection Refused" << false << unusedString
                           << false << "Error connecting to the server. Server response is: Connection Refused";

    QTest::newRow("timeout") << unusedNetworkError << unusedString << true << unusedString
                             << false << "Error requesting release notes: timeout";

    QString json = "{ json error }";
    QTest::newRow("json error") << QNetworkReply::NoError << unusedString << false << json
                                << false << "Error parsing JSON.";

    json = "{\"updateAvailable\":\"true\"}";
    QTest::newRow("updateAvailable is not boolean") << QNetworkReply::NoError << unusedString << false << json
                                                    << false << "Error parsing JSON.";

    json = "{\"updateAvailable\":true,\"version\":true,\"releaseNotesURL\":\"http://starviewer.udg.edu/releasenotes/releasenotes-0.10.1.html\"}";
    QTest::newRow("version or releaseNotes is not a string") << QNetworkReply::NoError << unusedString << false << json
                                                             << false << "Error parsing JSON: version or releaseNotesURL are not strings";
}
 
void test_ApplicationUpdateChecker::run_ShouldCheckForUpdatesAndFail()
{
    QFETCH(QNetworkReply::NetworkError, testingReplyErrorCode);
    QFETCH(QString, testingReplyErrorString);
    QFETCH(bool, testingTimeOut);
    QFETCH(QString, testingReplyData);

    QFETCH(bool, testingIsOnlineCheckOk);
    QFETCH(QString, testingErrorDescription);

    TestingApplicationUpdateChecker checker;
    checker.m_testingReplyErrorCode = testingReplyErrorCode;
    checker.m_testingReplyErrorString = testingReplyErrorString;
    checker.m_testingTimeOut = testingTimeOut;
    checker.m_testingReplyData = testingReplyData;
    checker.checkForUpdates();

    QCOMPARE(checker.isOnlineCheckOk(), testingIsOnlineCheckOk);
    if (!checker.isOnlineCheckOk())
    {
        QCOMPARE(checker.getErrorDescription(), testingErrorDescription);
    }
}

DECLARE_TEST(test_ApplicationUpdateChecker)

#include "test_applicationupdatechecker.moc"

