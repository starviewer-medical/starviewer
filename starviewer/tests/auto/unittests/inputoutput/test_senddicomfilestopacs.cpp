#include "autotest.h"
#include "testingsenddicomfilestopacs.h"

#include "image.h"

using namespace udg;
using namespace testing;

class test_SendDICOMFilesToPACS : public QObject {

    Q_OBJECT

private slots:

    void send_ShouldSendExpectedNumberOfFiles_data();
    void send_ShouldSendExpectedNumberOfFiles();

};

Q_DECLARE_METATYPE(QList<Image*>)
Q_DECLARE_METATYPE(PACSRequestStatus::SendRequestStatus)

void test_SendDICOMFilesToPACS::send_ShouldSendExpectedNumberOfFiles_data()
{
    QTest::addColumn< QList<Image*> >("images");
    QTest::addColumn<PACSRequestStatus::SendRequestStatus>("expectedReturnValue");
    QTest::addColumn<int>("expectedNumberOfFilesSentSuccessfully");
    QTest::addColumn<int>("expectedNumberOfFilesSentFailed");
    QTest::addColumn<int>("expectedNumberOfFilesSentWarning");

    QTest::newRow("empty list") << QList<Image*>() << PACSRequestStatus::SendAllDICOMFilesFailed << 0 << 0 << 0;
    QTest::newRow("one image") << (QList<Image*>() << new Image(this)) << PACSRequestStatus::SendOk << 1 << 0 << 0;

    {
        QList<Image*> images;

        for (int i = 0; i < 10; i++)
        {
            Image *image = new Image(this);
            image->setPath(QString::number(i));
            images.append(image);
        }

        QTest::newRow("multiple images with different paths") << images << PACSRequestStatus::SendOk << 10 << 0 << 0;
    }

    {
        QList<Image*> images;

        for (int i = 0; i < 10; i++)
        {
            Image *image = new Image(this);
            image->setPath("path");
            images.append(image);
        }

        QTest::newRow("multiple images with the same path") << images << PACSRequestStatus::SendOk << 1 << 0 << 0;
    }

    {
        QList<Image*> images;

        for (int i = 0; i < 2; i++)
        {
            Image *image = new Image(this);
            image->setPath("1");
            images.append(image);
        }

        for (int i = 2; i < 5; i++)
        {
            Image *image = new Image(this);
            image->setPath("2");
            images.append(image);
        }

        for (int i = 5; i < 6; i++)
        {
            Image *image = new Image(this);
            image->setPath("3");
            images.append(image);
        }

        for (int i = 6; i < 10; i++)
        {
            Image *image = new Image(this);
            image->setPath("4");
            images.append(image);
        }

        QTest::newRow("multiple images with mixed paths") << images << PACSRequestStatus::SendOk << 4 << 0 << 0;
    }
}

void test_SendDICOMFilesToPACS::send_ShouldSendExpectedNumberOfFiles()
{
    QFETCH(QList<Image*>, images);
    QFETCH(PACSRequestStatus::SendRequestStatus, expectedReturnValue);
    QFETCH(int, expectedNumberOfFilesSentSuccessfully);
    QFETCH(int, expectedNumberOfFilesSentFailed);
    QFETCH(int, expectedNumberOfFilesSentWarning);

    TestingSendDICOMFilesToPACS sender((PacsDevice())); // double parentheses are necessary to avoid compiler confusion

    QCOMPARE(sender.send(images), expectedReturnValue);
    QCOMPARE(sender.getNumberOfDICOMFilesSentSuccesfully(), expectedNumberOfFilesSentSuccessfully);
    QCOMPARE(sender.getNumberOfDICOMFilesSentFailed(), expectedNumberOfFilesSentFailed);
    QCOMPARE(sender.getNumberOfDICOMFilesSentWarning(), expectedNumberOfFilesSentWarning);
}

DECLARE_TEST(test_SendDICOMFilesToPACS)

#include "test_senddicomfilestopacs.moc"
