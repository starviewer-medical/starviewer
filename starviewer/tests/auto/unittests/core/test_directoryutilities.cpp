/*@
    "name": "test_DirectoryUtilities",
    "requirements": ["archive.dimse.retrieve", "archive.wado.retrieve_wado_uri", "archive.wado.retrieve_wado_rs", "archive.offline_data.dicomdir"]
 */

#include "autotest.h"
#include "directoryutilities.h"

using namespace udg;

class test_DirectoryUtilities : public QObject {
    Q_OBJECT

private slots:
    void sanitizeFilename_SanitizesAsExpected_data();
    void sanitizeFilename_SanitizesAsExpected();
};

void test_DirectoryUtilities::sanitizeFilename_SanitizesAsExpected_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("expectedSanitizedFilename");

    QTest::newRow("null") << QString() << QString();
    QTest::newRow("empty") << QString("") << QString("");
    QTest::newRow("already sanitized") << "asdf" << "asdf";
    QTest::newRow("slash") << QString("foo/bar") << QString("foo_bar");
    QTest::newRow("backslash") << QString("foo\\bar")
#ifdef Q_OS_WIN
                               << QString("foo_bar");
#else
                               << QString("foo\\bar");
#endif
    QTest::newRow("mixed and multiple") << QString("a\\b\\c/d\\e/f")
#ifdef Q_OS_WIN
                                        << QString("a_b_c_d_e_f");
#else
                                        << QString("a\\b\\c_d\\e_f");
#endif
}

void test_DirectoryUtilities::sanitizeFilename_SanitizesAsExpected()
{
    QFETCH(QString, filename);
    QFETCH(QString, expectedSanitizedFilename);

    DirectoryUtilities::sanitizeFilename(filename);

    QCOMPARE(filename, expectedSanitizedFilename);
}

DECLARE_TEST(test_DirectoryUtilities)

#include "test_directoryutilities.moc"
