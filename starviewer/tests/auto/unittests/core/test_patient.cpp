#include "autotest.h"
#include "patient.h"
#include "patienttesthelper.h"

using namespace udg;
using namespace testing;

class test_Patient : public QObject {
Q_OBJECT

private slots:
    void compareTo_ShoudReturnPatientsAreTheSame_data();
    void compareTo_ShoudReturnPatientsAreTheSame();
    void compareTo_ShoudReturnPatientsAreDifferent_data();
    void compareTo_ShoudReturnPatientsAreDifferent();

};

void test_Patient::compareTo_ShoudReturnPatientsAreTheSame_data()
{
    QTest::addColumn<QString>("idPatient1");
    QTest::addColumn<QString>("namePatient1");
    QTest::addColumn<QString>("idPatient2");
    QTest::addColumn<QString>("namePatient2");

    QTest::newRow("same ids") << "1234" << "asdfsadf" << "1234" << "kjhlkhljk";
    QTest::newRow("diff ids, exact names") << "1" << "PI RIPOLL PERE" << "2" << "PI RIPOLL PERE";
    QTest::newRow("diff ids, exact names with small letters") << "1" << "pI rIPOLl Pere" << "2" << "PI RIPOLL PERE";
    QTest::newRow("diff ids, exact names with number") << "1" << "PI RIPOLL1 PERE" << "2" << "PI RIPOLL1 PERE";
    QTest::newRow("diff ids, exact names with strage symbol and whitespaces") << "1" << "   PI-RIPOLL^PERE*+" << "2" << "PI RIPOLL PERE";
    QTest::newRow("diff ids, exact names with extra whitespace") << "1" << "PI RIPOLL  PERE" << "2" << "PI RIPOLL PERE";
    QTest::newRow("diff ids, 1 digit different") << "1" << "FERNANDEZ GONZALEZ^ROSA MARI" << "2" << "FERNANDEZ GONZALEZ^ROSA MARIA";

    // Falsos positius que s'haurien de mirar de solucionar i passar a "ShoudReturnPatientsAreDifferent"
    QTest::newRow("diff ids, similar first name") << "1" << "DIAZ LOPEZ^JOAN" << "2" << "DIAZ LOPEZ^JOSE";
}

void test_Patient::compareTo_ShoudReturnPatientsAreTheSame()
{
    QFETCH(QString, idPatient1);
    QFETCH(QString, namePatient1);
    QFETCH(QString, idPatient2);
    QFETCH(QString, namePatient2);

    QSharedPointer<Patient> patient1 = QSharedPointer<udg::Patient>(PatientTestHelper::createPatientWithIDAndName(idPatient1, namePatient1));
    QSharedPointer<Patient> patient2 = QSharedPointer<udg::Patient>(PatientTestHelper::createPatientWithIDAndName(idPatient2, namePatient2));

    QCOMPARE(patient1->compareTo(patient2.data()), Patient::SamePatients);
}

void test_Patient::compareTo_ShoudReturnPatientsAreDifferent_data()
{
    QTest::addColumn<QString>("idPatient1");
    QTest::addColumn<QString>("namePatient1");
    QTest::addColumn<QString>("idPatient2");
    QTest::addColumn<QString>("namePatient2");

    QTest::newRow("twins") << "14906777" << "DUNIA BOUHLASS BENALITI" << "14906779" << "MARIAM BOUHLASS BENALITI";
    QTest::newRow("composed names") << "1" << "FERNANDEZ GONZALEZ^ROSA M" << "2" << "FERNANDEZ GONZALEZ^ROSA MARIA";
}

void test_Patient::compareTo_ShoudReturnPatientsAreDifferent()
{
    QFETCH(QString, idPatient1);
    QFETCH(QString, namePatient1);
    QFETCH(QString, idPatient2);
    QFETCH(QString, namePatient2);

    QSharedPointer<Patient> patient1 = QSharedPointer<udg::Patient>(PatientTestHelper::createPatientWithIDAndName(idPatient1, namePatient1));
    QSharedPointer<Patient> patient2 = QSharedPointer<udg::Patient>(PatientTestHelper::createPatientWithIDAndName(idPatient2, namePatient2));

    QCOMPARE(patient1->compareTo(patient2.data()), Patient::DifferentPatients);
}

DECLARE_TEST(test_Patient)

#include "test_patient.moc"

