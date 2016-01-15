#include "autotest.h"

#include <QRegExp>

#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "patienttesthelper.h"
#include "hangingprotocoltesthelper.h"
#include "hangingprotocolimagesettesthelper.h"
#include "hangingprotocolmanager.h"
#include "hangingprotocol.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "hangingprotocolimagesetrestriction.h"

using namespace udg;
using namespace testing;

class TestHangingProtocolManager : public HangingProtocolManager {

public:
    void addHangingProtocolToRepository(HangingProtocol *hangingProtocol)
    {
        m_availableHangingProtocols.append(hangingProtocol);
    }
};

class test_HangingProtocolManager : public QObject {
Q_OBJECT

private slots:
    void searchHangingProtocols_ShouldReturnExpectedHangingProtocols_data();
    void searchHangingProtocols_ShouldReturnExpectedHangingProtocols();

private:
    QList<HangingProtocol*> getHangingProtocolsRepository();
    HangingProtocolImageSetRestriction createRestriction(QString selectorAttribute, QString valueRepresentation);
};

Q_DECLARE_METATYPE(QList<HangingProtocol*>)
Q_DECLARE_METATYPE(Patient*)

void test_HangingProtocolManager::searchHangingProtocols_ShouldReturnExpectedHangingProtocols_data()
{
    QTest::addColumn<QList<HangingProtocol*> >("hangingProtocolRepository");
    QTest::addColumn<Patient*>("patient");
    QTest::addColumn<QList<HangingProtocol*> >("expectedHangingProtocols");

    // PATIENTS
    //Pacient MG amb institució
    Patient *MGPatient = PatientTestHelper::create(1, 4, 1);
    MGPatient->getStudies().at(0)->addModality("MG");
    MGPatient->getStudies().at(0)->getSeries().at(0)->setModality("MG");
    MGPatient->getStudies().at(0)->getSeries().at(0)->setInstitutionName("Girona");
    MGPatient->getStudies().at(0)->getSeries().at(0)->getImages().at(0)->setImageLaterality(QString("R").at(0));
    MGPatient->getStudies().at(0)->getSeries().at(0)->getImages().at(0)->setViewCodeMeaning(QString("cranio-caudal"));
    MGPatient->getStudies().at(0)->getSeries().at(1)->setModality("MG");
    MGPatient->getStudies().at(0)->getSeries().at(1)->setInstitutionName("Girona");
    MGPatient->getStudies().at(0)->getSeries().at(1)->getImages().at(0)->setImageLaterality(QString("L").at(0));
    MGPatient->getStudies().at(0)->getSeries().at(1)->getImages().at(0)->setViewCodeMeaning(QString("cranio-caudal"));
    MGPatient->getStudies().at(0)->getSeries().at(2)->setModality("MG");
    MGPatient->getStudies().at(0)->getSeries().at(2)->setInstitutionName("Girona");
    MGPatient->getStudies().at(0)->getSeries().at(2)->getImages().at(0)->setImageLaterality(QString("R").at(0));
    MGPatient->getStudies().at(0)->getSeries().at(2)->getImages().at(0)->setViewCodeMeaning(QString("lateral"));
    MGPatient->getStudies().at(0)->getSeries().at(3)->setModality("MG");
    MGPatient->getStudies().at(0)->getSeries().at(3)->setInstitutionName("Girona");
    MGPatient->getStudies().at(0)->getSeries().at(3)->getImages().at(0)->setImageLaterality(QString("L").at(0));
    MGPatient->getStudies().at(0)->getSeries().at(3)->getImages().at(0)->setViewCodeMeaning(QString("lateral"));
    //CT
    Patient *CTPatient = PatientTestHelper::create(1, 2, 1);
    CTPatient->getStudies().at(0)->addModality("CT");
    CTPatient->getStudies().at(0)->getSeries().at(0)->setModality("CT");
    CTPatient->getStudies().at(0)->getSeries().at(1)->setModality("CT");
    //MR
    Patient *MRPatient = PatientTestHelper::create(1, 2, 1);
    MRPatient->getStudies().at(0)->addModality("MR");
    MRPatient->getStudies().at(0)->getSeries().at(0)->setModality("MR");
    MRPatient->getStudies().at(0)->getSeries().at(1)->setModality("MR");
    //CT+US
    Patient *CTUSPatient = PatientTestHelper::create(2, 2, 1);
    CTUSPatient->getStudies().at(0)->addModality("CT");
    CTUSPatient->getStudies().at(0)->getSeries().at(0)->setModality("CT");
    CTUSPatient->getStudies().at(0)->getSeries().at(1)->setModality("CT");
    CTUSPatient->getStudies().at(1)->addModality("US");
    CTUSPatient->getStudies().at(1)->getSeries().at(0)->setModality("US");
    CTUSPatient->getStudies().at(1)->getSeries().at(1)->setModality("US");

    QList<HangingProtocol*> repositoryHangingProtocolsMG = getHangingProtocolsRepository();
    QList<HangingProtocol*> expectedHangingProtocolsMG;
    expectedHangingProtocolsMG << repositoryHangingProtocolsMG.at(0);
    QList<HangingProtocol*> repositoryHangingProtocolsCT = getHangingProtocolsRepository();
    QList<HangingProtocol*> expectedHangingProtocolsCT;
    expectedHangingProtocolsCT << repositoryHangingProtocolsCT.at(1);
    QList<HangingProtocol*> repositoryHangingProtocolsMR = getHangingProtocolsRepository();
    QList<HangingProtocol*> expectedHangingProtocolsMR;
    QList<HangingProtocol*> repositoryHangingProtocolsCTUS = getHangingProtocolsRepository();
    QList<HangingProtocol*> expectedHangingProtocolsCTUS;
    expectedHangingProtocolsCTUS << repositoryHangingProtocolsCTUS.at(1);   // only hanging protocols for the first study are searched

    QTest::newRow("Patint with strict hanging protocol") << repositoryHangingProtocolsMG << MGPatient << expectedHangingProtocolsMG;
    QTest::newRow("Patient with non-strict hanging protocol") << repositoryHangingProtocolsCT << CTPatient << expectedHangingProtocolsCT;
    QTest::newRow("Patient without hanging") << repositoryHangingProtocolsMR << MRPatient << expectedHangingProtocolsMR;
    QTest::newRow("Patient with multiple hangings") << repositoryHangingProtocolsCTUS << CTUSPatient << expectedHangingProtocolsCTUS;
}

void test_HangingProtocolManager::searchHangingProtocols_ShouldReturnExpectedHangingProtocols()
{
    QFETCH(QList<HangingProtocol*>, hangingProtocolRepository);
    QFETCH(Patient *, patient);
    QFETCH(QList<HangingProtocol*>, expectedHangingProtocols);

    TestHangingProtocolManager testHangingProtocolManager;

    foreach (HangingProtocol *hangingProtocol, hangingProtocolRepository)
    {
        testHangingProtocolManager.addHangingProtocolToRepository(hangingProtocol);
    }

    QList<HangingProtocol *> hangingProtocolsCandidates = testHangingProtocolManager.searchHangingProtocols(patient->getStudies().first());
    QCOMPARE(hangingProtocolsCandidates.count(), expectedHangingProtocols.count());

    for (int i = 0; i < expectedHangingProtocols.count(); i++)
    {
        // Compare identifiers because the returned hanging protocols are filled copies of the originals
        QCOMPARE(hangingProtocolsCandidates.at(i)->getIdentifier(), expectedHangingProtocols.at(i)->getIdentifier());
    }
}

QList<HangingProtocol*> test_HangingProtocolManager::getHangingProtocolsRepository()
{
    // MG estricte i totes les imatges diferents, amb institució
    HangingProtocol *MGhangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("MG", 10, true, true, 0, 1, 4, 4);
    MGhangingProtocol->setProtocolsList(QStringList() << "MG");
    MGhangingProtocol->setInstitutionsRegularExpression(QRegExp("GIRONA|SABADELL", Qt::CaseInsensitive));
    //Restrictions MG
    HangingProtocolImageSetRestriction restrictionMG_1 = createRestriction("ImageLaterality", "R");
    HangingProtocolImageSetRestriction restrictionMG_2 = createRestriction("ImageLaterality", "L");
    HangingProtocolImageSetRestriction restrictionMG_3 = createRestriction("CodeMeaning", "cranio-caudal");
    HangingProtocolImageSetRestriction restrictionMG_4 = createRestriction("CodeMeaning", "cranio-caudal");
    HangingProtocolImageSetRestriction restrictionMG_5 = createRestriction("CodeMeaning", "later");
    QMap<int, HangingProtocolImageSetRestriction> restrictions;
    restrictions[1] = restrictionMG_1;
    restrictions[2] = restrictionMG_2;
    restrictions[3] = restrictionMG_3;
    restrictions[4] = restrictionMG_4;
    restrictions[5] = restrictionMG_5;
    // ImageSets MG
    HangingProtocolImageSet *imageSetMG_1 = MGhangingProtocol->getImageSet(1);
    imageSetMG_1->setRestrictionExpression(HangingProtocolImageSetRestrictionExpression("1 and not 4 and 5", restrictions));
    imageSetMG_1->setTypeOfItem("image");
    HangingProtocolImageSet *imageSetMG_2 = MGhangingProtocol->getImageSet(2);
    imageSetMG_2->setRestrictionExpression(HangingProtocolImageSetRestrictionExpression("2 and not 4 and 5", restrictions));
    imageSetMG_2->setTypeOfItem("image");
    HangingProtocolImageSet *imageSetMG_3 = MGhangingProtocol->getImageSet(3);
    imageSetMG_3->setRestrictionExpression(HangingProtocolImageSetRestrictionExpression("1 and 3", restrictions));
    imageSetMG_3->setTypeOfItem("image");
    HangingProtocolImageSet *imageSetMG_4 = MGhangingProtocol->getImageSet(4);
    imageSetMG_4->setRestrictionExpression(HangingProtocolImageSetRestrictionExpression("2 and 3", restrictions));
    imageSetMG_4->setTypeOfItem("image");
    //DisplaySets MG
    HangingProtocolDisplaySet *displaySetMG_1 = MGhangingProtocol->getDisplaySet(1);
    displaySetMG_1->setImageSet(imageSetMG_1);
    HangingProtocolDisplaySet *displaySetMG_2 = MGhangingProtocol->getDisplaySet(2);
    displaySetMG_2->setImageSet(imageSetMG_2);
    HangingProtocolDisplaySet *displaySetMG_3 = MGhangingProtocol->getDisplaySet(3);
    displaySetMG_3->setImageSet(imageSetMG_3);
    HangingProtocolDisplaySet *displaySetMG_4 = MGhangingProtocol->getDisplaySet(4);
    displaySetMG_4->setImageSet(imageSetMG_4);

    // CT general no estricte i no cal que les series siguin diferents
    HangingProtocol *CThangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("CT", 10, false, false, 0, 2, 2, 2);
    CThangingProtocol->setProtocolsList(QStringList() << "CT");
    // ImageSets CT
    HangingProtocolImageSet *imageSetCT_1 = CThangingProtocol->getImageSet(1);
    HangingProtocolImageSet *imageSetCT_2 = CThangingProtocol->getImageSet(2);
    //DisplaySets CT
    HangingProtocolDisplaySet *displaySetCT_1 = CThangingProtocol->getDisplaySet(1);
    displaySetCT_1->setImageSet(imageSetCT_1);
    HangingProtocolDisplaySet *displaySetCT_2 = CThangingProtocol->getDisplaySet(2);
    displaySetCT_2->setImageSet(imageSetCT_2);

    // MR no aplicable
    HangingProtocol *MRhangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("MR", 10, true, true, 0, 3, 2, 2);
    MRhangingProtocol->setProtocolsList(QStringList() << "MR");
    //Restrictions MR
    HangingProtocolImageSetRestriction restrictionMR_1 = createRestriction("SeriesDescription", "description");
    HangingProtocolImageSetRestriction restrictionMR_2 = createRestriction("SeriesDescription", "description");
    restrictions.clear();
    restrictions[1] = restrictionMR_1;
    restrictions[2] = restrictionMR_2;
    // ImageSets MR
    HangingProtocolImageSet *imageSetMR_1 = MRhangingProtocol->getImageSet(1);
    imageSetMR_1->setRestrictionExpression(HangingProtocolImageSetRestrictionExpression("1", restrictions));
    HangingProtocolImageSet *imageSetMR_2 = MRhangingProtocol->getImageSet(2);
    imageSetMR_2->setRestrictionExpression(HangingProtocolImageSetRestrictionExpression("not 2", restrictions));
    //DisplaySets MR
    HangingProtocolDisplaySet *displaySetMR_1 = MRhangingProtocol->getDisplaySet(1);
    displaySetMR_1->setImageSet(imageSetMR_1);
    HangingProtocolDisplaySet *displaySetMR_2 = MRhangingProtocol->getDisplaySet(2);
    displaySetMR_2->setImageSet(imageSetMR_2);

    // US general amb numero d'imatge
    HangingProtocol *UShangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("US", 5, false, false, 0, 4, 2, 2);
    UShangingProtocol->setProtocolsList(QStringList() << "US");
    // ImageSets US
    HangingProtocolImageSet *imageSetUS_1 = UShangingProtocol->getImageSet(1);
    imageSetUS_1->setTypeOfItem("image");
    imageSetUS_1->setImageNumberInStudyModality(0);
    HangingProtocolImageSet *imageSetUS_2 = UShangingProtocol->getImageSet(2);
    imageSetUS_2->setTypeOfItem("image");
    imageSetUS_2->setImageNumberInStudyModality(1);
    //DisplaySets US
    HangingProtocolDisplaySet *displaySetUS_1 = UShangingProtocol->getDisplaySet(1);
    displaySetUS_1->setImageSet(imageSetUS_1);
    HangingProtocolDisplaySet *displaySetUS_2 = UShangingProtocol->getDisplaySet(2);
    displaySetUS_2->setImageSet(imageSetUS_1);

    QList<HangingProtocol*> hangingProtocolRepository;
    hangingProtocolRepository << MGhangingProtocol << CThangingProtocol << MRhangingProtocol << UShangingProtocol;

    return hangingProtocolRepository;
}

HangingProtocolImageSetRestriction test_HangingProtocolManager::createRestriction(QString selectorAttribute, QString valueRepresentation)
{
    HangingProtocolImageSetRestriction restriction;
    restriction.setSelectorAttribute(selectorAttribute);
    restriction.setSelectorValue(valueRepresentation);
    return restriction;
}

DECLARE_TEST(test_HangingProtocolManager)

#include "test_hangingprotocolmanager.moc"
