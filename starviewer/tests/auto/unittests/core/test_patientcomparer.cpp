#include "autotest.h"
#include "patientcomparer.h"

#include "patient.h"

using namespace udg;

class TestingPatientComparer : public PatientComparer {

public:

    TestingPatientComparer() : m_askedUser(false)
    {
    }

public:

    bool m_userAnswer;
    mutable bool m_askedUser;

private:

    virtual bool askUserIfAreSamePatient(const Patient*, const Patient*) const
    {
        m_askedUser = true;
        return m_userAnswer;
    }

};

class test_PatientComparer : public QObject {

    Q_OBJECT

private slots:

    void areSamePatient_ShouldReturnExpectedValueWithoutAskingUser_data();
    void areSamePatient_ShouldReturnExpectedValueWithoutAskingUser();

    void areSamePatient_ShouldReturnExpectedValueAskingUserButOnlyTheFirstTime_data();
    void areSamePatient_ShouldReturnExpectedValueAskingUserButOnlyTheFirstTime();

};

Q_DECLARE_METATYPE(Patient*)

void test_PatientComparer::areSamePatient_ShouldReturnExpectedValueWithoutAskingUser_data()
{
    QTest::addColumn<Patient*>("patient1");
    QTest::addColumn<Patient*>("patient2");
    QTest::addColumn<bool>("samePatient");

    Patient *patient1 = new Patient(this);
    patient1->setID("1");
    patient1->setFullName("John Smith");
    Patient *patient1Bis = new Patient(*patient1, this);
    Patient *patient2 = new Patient(this);
    patient2->setID("2");
    patient2->setFullName("Foo Bar");

    QTest::newRow("null, null") << static_cast<Patient*>(NULL) << static_cast<Patient*>(NULL) << false;
    QTest::newRow("null, non-null") << static_cast<Patient*>(NULL) << new Patient(this) << false;
    QTest::newRow("non-null, null") << new Patient(this) << static_cast<Patient*>(NULL) << false;
    QTest::newRow("same patient") << patient1 << patient1 << true;
    QTest::newRow("same id and same name") << patient1 << patient1Bis << true;
    QTest::newRow("different id and different name") << patient1 << patient2 << false;
}

void test_PatientComparer::areSamePatient_ShouldReturnExpectedValueWithoutAskingUser()
{
    QFETCH(Patient*, patient1);
    QFETCH(Patient*, patient2);
    QFETCH(bool, samePatient);

    TestingPatientComparer patientComparer;

    QCOMPARE(patientComparer.areSamePatient(patient1, patient2), samePatient);
    QVERIFY(!patientComparer.m_askedUser);
}

void test_PatientComparer::areSamePatient_ShouldReturnExpectedValueAskingUserButOnlyTheFirstTime_data()
{
    QTest::addColumn<Patient*>("patient1");
    QTest::addColumn<Patient*>("patient2");
    QTest::addColumn<bool>("samePatient");

    Patient *patient1John = new Patient(this);
    patient1John->setID("1");
    patient1John->setFullName("John Smith");
    Patient *patient1Foo = new Patient(this);
    patient1Foo->setID("1");
    patient1Foo->setFullName("Foo Bar");
    Patient *patient2Foo = new Patient(this);
    patient2Foo->setID("2");
    patient2Foo->setFullName("Foo Bar");

    QTest::newRow("same id and different name, user says same") << patient1John << patient1Foo << true;
    QTest::newRow("same id and different name, user says different") << patient1John << patient1Foo << false;
    QTest::newRow("different id and same name, user says same") << patient1Foo << patient2Foo << true;
    QTest::newRow("different id and same name, user says different") << patient1Foo << patient2Foo << false;
}

void test_PatientComparer::areSamePatient_ShouldReturnExpectedValueAskingUserButOnlyTheFirstTime()
{
    QFETCH(Patient*, patient1);
    QFETCH(Patient*, patient2);
    QFETCH(bool, samePatient);

    TestingPatientComparer patientComparer;
    patientComparer.m_userAnswer = samePatient;

    QCOMPARE(patientComparer.areSamePatient(patient1, patient2), samePatient);
    QVERIFY(patientComparer.m_askedUser);

    patientComparer.m_askedUser = false;

    QCOMPARE(patientComparer.areSamePatient(patient1, patient2), samePatient);
    QVERIFY(!patientComparer.m_askedUser);
    QCOMPARE(patientComparer.areSamePatient(patient2, patient1), samePatient);
    QVERIFY(!patientComparer.m_askedUser);
}

DECLARE_TEST(test_PatientComparer)

#include "test_patientcomparer.moc"
