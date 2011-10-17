#include "autotest.h"
#include "patient.h"
#include "patienttesthelper.h"

using namespace udg;
using namespace testing;

class test_Patient : public QObject {
Q_OBJECT

private slots:
    void compareTo_ShoudReturnExpectedPatientsSimilarity_data();
    void compareTo_ShoudReturnExpectedPatientsSimilarity();
};

Q_DECLARE_METATYPE(Patient::PatientsSimilarity)

void test_Patient::compareTo_ShoudReturnExpectedPatientsSimilarity_data()
{
    QTest::addColumn<QString>("idPatient1");
    QTest::addColumn<QString>("namePatient1");
    QTest::addColumn<QString>("idPatient2");
    QTest::addColumn<QString>("namePatient2");
    QTest::addColumn<Patient::PatientsSimilarity>("patientsSimilarity");

    QTest::newRow("same ids, exact names") << "1" << "PI RIPOLL PERE" << "1" << "PI RIPOLL PERE" << Patient::SamePatients;
    QTest::newRow("same ids, exact names with lower letters") << "1" << "pI rIPOLl Pere" << "1" << "PI RIPOLL PERE" << Patient::SamePatients;
    QTest::newRow("same ids, exact names with strage symbol and whitespaces") << "1" << "   PI-RIPOLL^PERE*+" << "1" << "PI RIPOLL PERE" << Patient::SamePatients;
    QTest::newRow("same ids, similar names with 1 digit different") << "1" << "LOPEZ CASTAÑO^NIEVES" << "1" << "LOPEZ CASTANO^NIEVES" << Patient::SamePatients;

    QTest::newRow("same ids, diff names") << "1234" << "asdfsadf" << "1234" << "kjhlkhljk" << Patient::SamePatientIDsDifferentPatientNames;

    QTest::newRow("diff ids, exact names") << "1" << "PI RIPOLL PERE" << "2" << "PI RIPOLL PERE" << Patient::SamePatientNamesDifferentPatientIDs;
    QTest::newRow("diff ids, exact names with extra whitespace") << "1" << "PI RIPOLL  PERE" << "2" << "PI RIPOLL PERE" <<  Patient::SamePatientNamesDifferentPatientIDs;
    QTest::newRow("diff ids, 1 digit different") << "1" << "FERNANDEZ GONZALEZ^ROSA MARI" << "2" << "FERNANDEZ GONZALEZ^ROSA MARIA" << Patient::SamePatientNamesDifferentPatientIDs;
    QTest::newRow("diff ids, similar first name") << "1" << "DIAZ LOPEZ^JOAN" << "2" << "DIAZ LOPEZ^JOSE" << Patient::SamePatientNamesDifferentPatientIDs;

    QTest::newRow("twins") << "14906777" << "DUNIA BOUHLASS BENALITI" << "14906779" << "MARIAM BOUHLASS BENALITI" << Patient::DifferentPatients;
}

void test_Patient::compareTo_ShoudReturnExpectedPatientsSimilarity()
{
    QFETCH(QString, idPatient1);
    QFETCH(QString, namePatient1);
    QFETCH(QString, idPatient2);
    QFETCH(QString, namePatient2);
    QFETCH(Patient::PatientsSimilarity, patientsSimilarity);

    QSharedPointer<Patient> patient1 = QSharedPointer<udg::Patient>(PatientTestHelper::createPatientWithIDAndName(idPatient1, namePatient1));
    QSharedPointer<Patient> patient2 = QSharedPointer<udg::Patient>(PatientTestHelper::createPatientWithIDAndName(idPatient2, namePatient2));

    QCOMPARE(patient1->compareTo(patient2.data()), patientsSimilarity);
}

DECLARE_TEST(test_Patient)

#include "test_patient.moc"

