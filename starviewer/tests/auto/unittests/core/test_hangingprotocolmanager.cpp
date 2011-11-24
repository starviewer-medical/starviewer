#include "autotest.h"

#include <QRegExp>

#include "patient.h"
#include "study.h"
#include "series.h"
#include "patienttesthelper.h"
#include "hangingprotocoltesthelper.h"
#include "hangingprotocolmanager.h"
#include "hangingprotocol.h"


using namespace udg;
using namespace testing;

class TestHangingProtocolManager : public HangingProtocolManager {

public:
    void addHangingProtocolToRepository(HangingProtocol *hangingProtocol)
    {
        m_availableHangingProtocols.append(hangingProtocol);
    }

protected:
    virtual void copyHangingProtocolRepository()
    {
    }

};

class test_HangingProtocolManager : public QObject {
Q_OBJECT

private slots:
    void searchHangingProtocols_TestInstitutionTag_ShouldReturnAvailableHangingProtocols_data();
    void searchHangingProtocols_TestInstitutionTag_ShouldReturnAvailableHangingProtocols();

private:
    QList<HangingProtocol*> getHangingProtocolRepositoryToTestInstitutionTag();
};

Q_DECLARE_METATYPE(QList<HangingProtocol*>)
Q_DECLARE_METATYPE(Patient*)

void test_HangingProtocolManager::searchHangingProtocols_TestInstitutionTag_ShouldReturnAvailableHangingProtocols_data()
{
    QTest::addColumn<QList<HangingProtocol*> >("hangingProtocolRepository");
    QTest::addColumn<Patient*>("patient");
    QTest::addColumn<int>("numberOfApplyableHangingProtocols");

    Patient *patientInstitutionGirona = PatientTestHelper::create(1, 1, 1);
    patientInstitutionGirona->getStudies().at(0)->getSeries().at(0)->setInstitutionName("Girona");
    patientInstitutionGirona->getStudies().at(0)->addModality("CT");
    patientInstitutionGirona->getStudies().at(0)->getSeries().at(0)->setModality("CT");

    Patient *patientInstitutionSabadell = PatientTestHelper::create(1, 1, 1);
    patientInstitutionSabadell->getStudies().at(0)->getSeries().at(0)->setInstitutionName("Sabadell");
    patientInstitutionSabadell->getStudies().at(0)->addModality("CT");
    patientInstitutionSabadell->getStudies().at(0)->getSeries().at(0)->setModality("CT");

    Patient *patientEmptyInstitution = PatientTestHelper::create(1, 1, 1);
    patientEmptyInstitution->getStudies().at(0)->addModality("CT");
    patientEmptyInstitution->getStudies().at(0)->getSeries().at(0)->setModality("CT");

    QTest::newRow("Patient with no institution") << getHangingProtocolRepositoryToTestInstitutionTag() << patientEmptyInstitution << 1 ;
    QTest::newRow("Patient with from institution Girona ") << getHangingProtocolRepositoryToTestInstitutionTag() << patientInstitutionGirona << 2;
    QTest::newRow("Patient with fom institution Sabadell") << getHangingProtocolRepositoryToTestInstitutionTag() << patientInstitutionSabadell << 3;
}

void test_HangingProtocolManager::searchHangingProtocols_TestInstitutionTag_ShouldReturnAvailableHangingProtocols()
{
    QFETCH(QList<HangingProtocol*>, hangingProtocolRepository);
    QFETCH(Patient *, patient);
    QFETCH(int, numberOfApplyableHangingProtocols);

    TestHangingProtocolManager testHangingProtocolManager;

    foreach (HangingProtocol *hangingProtocol, hangingProtocolRepository)
    {
        testHangingProtocolManager.addHangingProtocolToRepository(hangingProtocol);
    }

    QList<HangingProtocol *> hangingProtocolsToApply = testHangingProtocolManager.searchHangingProtocols(patient);
    QCOMPARE(hangingProtocolsToApply.count(), numberOfApplyableHangingProtocols);

    foreach(HangingProtocol *hangingProtocol, hangingProtocolsToApply)
    {
        QString institutionName = patient->getStudies().at(0)->getSeries().at(0)->getInstitutionName();
        QRegExp institutionRegularExpression = hangingProtocol->getInstitutionsRegularExpression();

        QVERIFY(institutionName.contains(hangingProtocol->getInstitutionsRegularExpression()) || institutionRegularExpression.isEmpty());
    }
}

QList<HangingProtocol*> test_HangingProtocolManager::getHangingProtocolRepositoryToTestInstitutionTag()
{
    HangingProtocol *hangingProtocolWithOutInstitution(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
    HangingProtocol *hangingProtocolWithInstitutionSabadell(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
    HangingProtocol *hangingProtocolWithInstitutionGironaAndSabadell(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());

    hangingProtocolWithInstitutionSabadell->setInstitutionsRegularExpression(QRegExp("SABADELL", Qt::CaseInsensitive));
    hangingProtocolWithInstitutionGironaAndSabadell->setInstitutionsRegularExpression(QRegExp("GIRONA|SABADELL", Qt::CaseInsensitive));

    hangingProtocolWithOutInstitution->setProtocolsList(QList<QString>() << "CT");
    hangingProtocolWithInstitutionSabadell->setProtocolsList(QList<QString>() << "CT");
    hangingProtocolWithInstitutionGironaAndSabadell->setProtocolsList(QList<QString>() << "CT");

    QList<HangingProtocol*> hangingProtocolRepository;
    hangingProtocolRepository << hangingProtocolWithOutInstitution << hangingProtocolWithInstitutionSabadell << hangingProtocolWithInstitutionGironaAndSabadell;

    return hangingProtocolRepository;
}

DECLARE_TEST(test_HangingProtocolManager)

#include "test_hangingprotocolmanager.moc"
