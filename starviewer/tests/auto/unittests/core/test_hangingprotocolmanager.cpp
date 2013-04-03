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
    HangingProtocolImageSet::Restriction createRestriction(HangingProtocolImageSet::SelectorUsageFlag flag, QString selectorAttribute, QString valueRepresentation);
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
    expectedHangingProtocolsCTUS << repositoryHangingProtocolsCTUS.at(1) << repositoryHangingProtocolsCTUS.at(3);

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

    QList<HangingProtocol *> hangingProtocolsCandidates = testHangingProtocolManager.searchHangingProtocols(patient);
    QCOMPARE(hangingProtocolsCandidates.count(), expectedHangingProtocols.count());

    foreach(HangingProtocol *hangingProtocol, expectedHangingProtocols)
    {
        QVERIFY(hangingProtocolsCandidates.contains(hangingProtocol));
    }
}

QList<HangingProtocol*> test_HangingProtocolManager::getHangingProtocolsRepository()
{
    // MG estricte i totes les imatges diferents, amb institució
    HangingProtocol *MGhangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("MG", 10, true, true, false, 1, 4, 4);
    MGhangingProtocol->setProtocolsList(QStringList() << "MG");
    MGhangingProtocol->setInstitutionsRegularExpression(QRegExp("GIRONA|SABADELL", Qt::CaseInsensitive));
    //Restrictions MG
    HangingProtocolImageSet::Restriction restrictionMG_1 = createRestriction(HangingProtocolImageSet::Match, "ImageLaterality", "R");
    HangingProtocolImageSet::Restriction restrictionMG_2 = createRestriction(HangingProtocolImageSet::Match, "ImageLaterality", "L");
    HangingProtocolImageSet::Restriction restrictionMG_3 = createRestriction(HangingProtocolImageSet::Match, "CodeMeaning", "cranio-caudal");
    HangingProtocolImageSet::Restriction restrictionMG_4 = createRestriction(HangingProtocolImageSet::NoMatch, "CodeMeaning", "cranio-caudal");
    HangingProtocolImageSet::Restriction restrictionMG_5 = createRestriction(HangingProtocolImageSet::Match, "CodeMeaning", "later");
    // ImageSets MG
    HangingProtocolImageSet *imageSetMG_1 = MGhangingProtocol->getImageSet(1);
    imageSetMG_1->addRestriction(restrictionMG_1);
    imageSetMG_1->addRestriction(restrictionMG_4);
    imageSetMG_1->addRestriction(restrictionMG_5);
    imageSetMG_1->setTypeOfItem("image");
    HangingProtocolImageSet *imageSetMG_2 = MGhangingProtocol->getImageSet(2);
    imageSetMG_2->addRestriction(restrictionMG_2);
    imageSetMG_2->addRestriction(restrictionMG_4);
    imageSetMG_2->addRestriction(restrictionMG_5);
    imageSetMG_2->setTypeOfItem("image");
    HangingProtocolImageSet *imageSetMG_3 = MGhangingProtocol->getImageSet(3);
    imageSetMG_3->addRestriction(restrictionMG_1);
    imageSetMG_3->addRestriction(restrictionMG_3);
    imageSetMG_3->setTypeOfItem("image");
    HangingProtocolImageSet *imageSetMG_4 = MGhangingProtocol->getImageSet(4);
    imageSetMG_4->addRestriction(restrictionMG_2);
    imageSetMG_4->addRestriction(restrictionMG_3);
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
    HangingProtocol *CThangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("CT", 10, false, false, false, 2, 2, 2);
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
    HangingProtocol *MRhangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("MR", 10, true, true, false, 3, 2, 2);
    MRhangingProtocol->setProtocolsList(QStringList() << "MR");
    //Restrictions MR
    HangingProtocolImageSet::Restriction restrictionMR_1 = createRestriction(HangingProtocolImageSet::Match, "SeriesDescription", "description");
    HangingProtocolImageSet::Restriction restrictionMR_2 = createRestriction(HangingProtocolImageSet::NoMatch, "SeriesDescription", "description");
    // ImageSets MR
    HangingProtocolImageSet *imageSetMR_1 = MRhangingProtocol->getImageSet(1);
    imageSetMR_1->addRestriction(restrictionMR_1);
    HangingProtocolImageSet *imageSetMR_2 = MRhangingProtocol->getImageSet(2);
    imageSetMR_2->addRestriction(restrictionMR_2);
    //DisplaySets MR
    HangingProtocolDisplaySet *displaySetMR_1 = MRhangingProtocol->getDisplaySet(1);
    displaySetMR_1->setImageSet(imageSetMR_1);
    HangingProtocolDisplaySet *displaySetMR_2 = MRhangingProtocol->getDisplaySet(2);
    displaySetMR_2->setImageSet(imageSetMR_2);

    // US general amb numero d'imatge
    HangingProtocol *UShangingProtocol = HangingProtocolTestHelper::createHangingProtocolWithAttributes("US", 5, false, false, false, 4, 2, 2);
    UShangingProtocol->setProtocolsList(QStringList() << "US");
    // ImageSets US
    HangingProtocolImageSet *imageSetUS_1 = UShangingProtocol->getImageSet(1);
    imageSetUS_1->setTypeOfItem("image");
    imageSetUS_1->setImageNumberInPatientModality(0);
    HangingProtocolImageSet *imageSetUS_2 = UShangingProtocol->getImageSet(2);
    imageSetUS_2->setTypeOfItem("image");
    imageSetUS_2->setImageNumberInPatientModality(1);
    //DisplaySets US
    HangingProtocolDisplaySet *displaySetUS_1 = UShangingProtocol->getDisplaySet(1);
    displaySetUS_1->setImageSet(imageSetUS_1);
    HangingProtocolDisplaySet *displaySetUS_2 = UShangingProtocol->getDisplaySet(2);
    displaySetUS_2->setImageSet(imageSetUS_1);

    QList<HangingProtocol*> hangingProtocolRepository;
    hangingProtocolRepository << MGhangingProtocol << CThangingProtocol << MRhangingProtocol << UShangingProtocol;

    return hangingProtocolRepository;
}

HangingProtocolImageSet::Restriction test_HangingProtocolManager::createRestriction(HangingProtocolImageSet::SelectorUsageFlag flag, QString selectorAttribute, QString valueRepresentation )
{
    HangingProtocolImageSet::Restriction restriction;
    restriction.usageFlag = flag;
    restriction.selectorAttribute = selectorAttribute;
    restriction.valueRepresentation = valueRepresentation;
    return restriction;
}

DECLARE_TEST(test_HangingProtocolManager)

#include "test_hangingprotocolmanager.moc"
