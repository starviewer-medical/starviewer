/*@
    "name": "test_HangingProtocolFiller",
    "requirements": ["workflow.hanging_protocols"]
 */

#include "autotest.h"
#include "hangingprotocolfiller.h"

#include "hangingprotocol.h"
#include "hangingprotocoltesthelper.h"
#include "hangingprotocolimageset.h"
#include "study.h"
#include "studytesthelper.h"

using namespace udg;
using namespace testing;

class test_HangingProtocolFiller : public QObject {
    Q_OBJECT

private slots:
    void fill_ShouldFillImageSetsAsExpected_data();
    void fill_ShouldFillImageSetsAsExpected();

};

Q_DECLARE_METATYPE(QSharedPointer<HangingProtocol>)

void test_HangingProtocolFiller::fill_ShouldFillImageSetsAsExpected_data()
{
    QTest::addColumn<QSharedPointer<HangingProtocol>>("hangingProtocol");
    QTest::addColumn<Study*>("study");
    QTest::addColumn<QList<Study*>>("priorStudies");
    QTest::addColumn<QVector<QVector<Series*>>>("expectedSeriesToDisplay");
    QTest::addColumn<QVector<int>>("expectedImageToDisplay");

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy();
        study->setParent(this);

        QTest::newRow("no series") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{}}
                                   << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(1);
        study->setParent(this);

        QTest::newRow("series but no images") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{}}
                                              << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(2, {0, 1});
        study->setParent(this);

        QTest::newRow("first series is empty") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{study->getSeries("1")}}
                                               << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(1, 1);
        study->setParent(this);

        QTest::newRow("no restrictions") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{study->getSeries("0")}}
                                         << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithAttributes("", 0, false, true, 0, 1, 2, 0));
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(2)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(2, 1);
        study->setParent(this);

        QTest::newRow("all different") << hangingProtocol << study << QList<Study*>{}
                                       << QVector<QVector<Series*>>{{study->getSeries("0")}, {study->getSeries("1")}}
                                       << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay(),
                                                       hangingProtocol->getImageSet(2)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithAttributes("", 0, false, false, 0, 1, 2, 0));
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(2)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(2, 1);
        study->setParent(this);

        QTest::newRow("not all different") << hangingProtocol << study << QList<Study*>{}
                                           << QVector<QVector<Series*>>{{study->getSeries("0")}, {study->getSeries("0")}}
                                           << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay(),
                                                           hangingProtocol->getImageSet(2)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithAttributes("", 0, false, true, 0, 1, 2, 0));
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(2)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(3, 1);
        study->setParent(this);
        study->getSeries("1")->setModality("XX");   // other series are OT by default

        QTest::newRow("all different, some modality incompatible") << hangingProtocol << study << QList<Study*>{}
                                                                   << QVector<QVector<Series*>>{{study->getSeries("0")}, {study->getSeries("2")}}
                                                                   << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay(),
                                                                                   hangingProtocol->getImageSet(2)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithAttributes("", 0, false, false, 0, 1, 2, 0));
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(2)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(3, 1);
        study->setParent(this);
        study->getSeries("1")->setModality("XX");   // other series are OT by default

        QTest::newRow("not all different, some modality incompatible") << hangingProtocol << study << QList<Study*>{}
                                                                       << QVector<QVector<Series*>>{{study->getSeries("0")}, {study->getSeries("0")}}
                                                                       << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay(),
                                                                                       hangingProtocol->getImageSet(2)->getImageToDisplay()};
    }

    // type series (default) already tested in all above cases

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(1)->setType(HangingProtocolImageSet::Type::Image);
        Study *study = StudyTestHelper::createStudy(1, 1);
        study->setParent(this);

        QTest::newRow("type image") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{study->getSeries("0")}} << QVector<int>{0};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"CT", "PT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(1)->setType(HangingProtocolImageSet::Type::Fusion);
        Study *study = StudyTestHelper::createStudy(2, 1);
        study->setParent(this);
        study->getSeries("0")->setModality("CT");
        study->getSeries("1")->setModality("PT");

        QTest::newRow("type fusion") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{study->getSeries("0"), study->getSeries("1")}}
                                     << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({"1", {{1, {1, "ViewPosition", "PA", 0}}}});
        Study *study = StudyTestHelper::createStudy(1, 1);
        study->setParent(this);

        QTest::newRow("image set left empty") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{}}
                                              << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(1)->setImageNumberInStudyModality(4);
        Study *study = StudyTestHelper::createStudy(3, 3);
        study->setParent(this);
        study->getSeries("1")->setModality("XX");   // other series are OT by default

        QTest::newRow("image number in study modality") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{study->getSeries("2")}}
                                                        << QVector<int>{1};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(1)->setImageNumberInStudyModality(10);
        Study *study = StudyTestHelper::createStudy(3, 3);
        study->setParent(this);
        study->getSeries("1")->setModality("XX");   // other series are OT by default

        QTest::newRow("image number in study modality (index out of range)") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{}}
                                                                             << QVector<int>{0};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({"1", {{1, {1, "Modality", "AA", 0}}}});
        hangingProtocol->getImageSet(1)->setImageNumberInStudyModality(4);
        Study *study = StudyTestHelper::createStudy(3, 3);
        study->setParent(this);
        study->getSeries("1")->setModality("XX");   // other series are OT by default

        QTest::newRow("image number in study modality (failed restriction)") << hangingProtocol << study << QList<Study*>{} << QVector<QVector<Series*>>{{}}
                                                                             << QVector<int>{0};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithAttributes("", 0, false, false, 1, 1, 2, 0));
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(1)->setAbstractPriorValue(-1);
        hangingProtocol->getImageSet(2)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(1, 1);
        study->setParent(this);
        study->setDate(QDate::fromString("2017-10-01", Qt::ISODate));
        QList<Study*> priorStudies;
        for (int i = 0; i < 4; i++)
        {
            Study *prior = StudyTestHelper::createStudy(1, 1);
            prior->setParent(this);
            prior->setID(QString("p%1").arg(i));                // this helps debugging
            prior->setDate(study->getDate().addDays(1 - i));    // one study newer, one same date, and the rest older
            priorStudies.append(prior);
        }

        QTest::newRow("last prior") << hangingProtocol << study << priorStudies
                                    << QVector<QVector<Series*>>{{priorStudies.last()->getSeries("0")}, {study->getSeries("0")}}
                                    << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay(), hangingProtocol->getImageSet(2)->getImageToDisplay()};
    }

    {
        auto hangingProtocol = QSharedPointer<HangingProtocol>(HangingProtocolTestHelper::createHangingProtocolWithAttributes("", 0, false, false, 1, 1, 2, 0));
        hangingProtocol->setProtocolsList({"OT"});
        hangingProtocol->getImageSet(1)->addRestrictionExpression({});
        hangingProtocol->getImageSet(1)->setAbstractPriorValue(1);
        hangingProtocol->getImageSet(2)->addRestrictionExpression({});
        Study *study = StudyTestHelper::createStudy(1, 1);
        study->setParent(this);
        study->setDate(QDate::fromString("2017-10-01", Qt::ISODate));
        QList<Study*> priorStudies;
        for (int i = 0; i < 4; i++)
        {
            Study *prior = StudyTestHelper::createStudy(1, 1);
            prior->setParent(this);
            prior->setID(QString("p%1").arg(i));                // this helps debugging
            prior->setDate(study->getDate().addDays(1 - i));    // one study newer, one same date, and the rest older
            priorStudies.append(prior);
        }

        QTest::newRow("first prior") << hangingProtocol << study << priorStudies
                                     << QVector<QVector<Series*>>{{priorStudies[2]->getSeries("0")}, {study->getSeries("0")}}
                                     << QVector<int>{hangingProtocol->getImageSet(1)->getImageToDisplay(),
                                                     hangingProtocol->getImageSet(2)->getImageToDisplay()};
    }
}

void test_HangingProtocolFiller::fill_ShouldFillImageSetsAsExpected()
{
    QFETCH(QSharedPointer<HangingProtocol>, hangingProtocol);
    QFETCH(Study*, study);
    QFETCH(QList<Study*>, priorStudies);
    QFETCH(QVector<QVector<Series*>>, expectedSeriesToDisplay);
    QFETCH(QVector<int>, expectedImageToDisplay);

    HangingProtocolFiller filler;
    filler.fill(hangingProtocol.get(), study, priorStudies);

    for (int i = 0; i < hangingProtocol->getNumberOfImageSets(); i++)
    {
        HangingProtocolImageSet *imageSet = hangingProtocol->getImageSet(i+1);
        QCOMPARE(imageSet->getSeriesToDisplay(), expectedSeriesToDisplay[i]);
        QCOMPARE(imageSet->getImageToDisplay(), expectedImageToDisplay[i]);
    }

    StudyTestHelper::cleanUp(study);
    for (Study *prior : priorStudies)
    {
        StudyTestHelper::cleanUp(prior);
    }
}

DECLARE_TEST(test_HangingProtocolFiller)

#include "test_hangingprotocolfiller.moc"
