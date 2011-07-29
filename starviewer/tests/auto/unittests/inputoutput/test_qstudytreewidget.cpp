#include "autotest.h"

#include <QList>
#include <QTreeWidgetItem>

#include "qstudytreewidget.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "dicomsource.h"
#include "patienttesthelper.h"
#include "seriestesthelper.h"
#include "dicomsourcetesthelper.h"

using namespace udg;
using namespace testing;

class test_QStudyTreeWidget : public QObject {
Q_OBJECT

private slots:

    void getStudy_ShouldReturnNull_data();
    void getStudy_ShouldReturnNull();

    void getStudy_ShouldReturnStudy_data();
    void getStudy_ShouldReturnStudy();

    void getSeries_ShouldReturnNull_data();
    void getSeries_ShouldReturnNull();

    void getSeries_ShouldReturnSeries_data();
    void getSeries_ShouldReturnSeries();

    void removeStudy_ShouldRemoveStudy_data();
    void removeStudy_ShouldRemoveStudy();

    void removeSeries_ShouldRemoveSeries_data();
    void removeSeries_ShouldRemoveSeries();

    void removeSeries_ShouldRemoveStudy_data();
    void removeSeries_ShouldRemoveStudy();

    void init();
    void cleanupTestCase();

private:

    QStudyTreeWidget *m_qstudyTreeWidget;
};

Q_DECLARE_METATYPE(Patient*)
Q_DECLARE_METATYPE(QList<Patient*>)
Q_DECLARE_METATYPE(Series*)
Q_DECLARE_METATYPE(QList<Series*>)


void test_QStudyTreeWidget::init()
{
    m_qstudyTreeWidget = new QStudyTreeWidget();
}

void test_QStudyTreeWidget::cleanupTestCase()
{
    m_qstudyTreeWidget->clear();
    delete m_qstudyTreeWidget;
}

void test_QStudyTreeWidget::getStudy_ShouldReturnNull_data()
{
    QTest::addColumn<Patient*>("inputPatient");

    Patient *patient = PatientTestHelper::create(1);
    patient->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));

    QTest::newRow("Valid patient") << patient;
}

void test_QStudyTreeWidget::getStudy_ShouldReturnNull()
{
    QFETCH(Patient*, inputPatient);

    m_qstudyTreeWidget->insertPatient(inputPatient);
    QCOMPARE(m_qstudyTreeWidget->getStudy("INVENTENTED UID", DICOMSource()) == NULL, true);
}

void test_QStudyTreeWidget::getStudy_ShouldReturnStudy_data()
{
    QTest::addColumn<QList<Patient*> >("inputPatients");

    Patient *patientOne = PatientTestHelper::create(1);
    patientOne->setID("1");
    patientOne->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));
    patientOne->getStudies().at(0)->setInstanceUID("1");

    Patient *patientTwo = PatientTestHelper::create(1);
    patientTwo->setID("2");
    patientTwo->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("2"));
    patientTwo->getStudies().at(0)->setInstanceUID("2");

    QTest::newRow("Adding two patients to QStudyTreeWidget") << (QList<Patient*>() << patientOne << patientTwo);
}

void test_QStudyTreeWidget::getStudy_ShouldReturnStudy()
{
    QFETCH(QList<Patient*>, inputPatients);

    m_qstudyTreeWidget->insertPatientList(inputPatients);

    QCOMPARE(m_qstudyTreeWidget->getQTreeWidget()->topLevelItemCount(), inputPatients.count());

    foreach(Patient* patient, inputPatients)
    {
        Study *studyToCompare = patient->getStudies().at(0);
        QCOMPARE(m_qstudyTreeWidget->getStudy(studyToCompare->getInstanceUID(), studyToCompare->getDICOMSource()) == studyToCompare, true);
    }
}

void test_QStudyTreeWidget::getSeries_ShouldReturnNull_data()
{
    QTest::addColumn<Patient*>("inputPatient");
    QTest::addColumn<QList<Series*> >("inputSeries");

    Patient *patient = PatientTestHelper::create(1);
    patient->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));
    Series *series = SeriesTestHelper::createSeriesByUID("1");

    QTest::newRow("Valid patient and series") << patient << (QList<Series*>() << series);
}

void test_QStudyTreeWidget::getSeries_ShouldReturnNull()
{
    QFETCH(Patient*, inputPatient);
    QFETCH(QList<Series*>, inputSeries);

    m_qstudyTreeWidget->insertPatient(inputPatient);
    m_qstudyTreeWidget->insertSeriesList(inputPatient->getStudies().at(0)->getInstanceUID(), inputSeries);
    QCOMPARE(m_qstudyTreeWidget->getSeries("INVENTENTED UID","INVENTENTED UID", DICOMSource()) == NULL, true);
}

void test_QStudyTreeWidget::getSeries_ShouldReturnSeries_data()
{
    QTest::addColumn<Patient*>("inputPatient");
    QTest::addColumn<QList<Series*> >("inputSeries");

    Patient *patient = PatientTestHelper::create(1);
    patient->setID("1");
    patient->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));
    patient->getStudies().at(0)->setInstanceUID("1");

    Series *seriesOne = SeriesTestHelper::createSeriesByUID("1");
    seriesOne->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));

    Series *seriesTwo = SeriesTestHelper::createSeriesByUID("2");
    seriesTwo->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));

    QTest::newRow("Adding two series to study") << patient << (QList<Series*>() << seriesOne << seriesTwo);
}

void test_QStudyTreeWidget::getSeries_ShouldReturnSeries()
{
    QFETCH(Patient*, inputPatient);
    QFETCH(QList<Series*>, inputSeries);

    Study *parentStudySeries = inputPatient->getStudies().at(0);
    m_qstudyTreeWidget->insertPatient(inputPatient);
    //Hack: Quan inserim un pacient/estudi, en el QTreeWidget se li insereix un fill, perquè l'estudi es pugui desplegar i emetre signal requestedSeriesOfStudy
    //hem d'eliminar aquest fill, ja que sinó quan eliminem la sèrie l'estudi continua tenint un fill.
    delete m_qstudyTreeWidget->getQTreeWidget()->topLevelItem(0)->child(0);

    m_qstudyTreeWidget->insertSeriesList(parentStudySeries->getInstanceUID(), inputSeries);

    QCOMPARE(m_qstudyTreeWidget->getQTreeWidget()->topLevelItem(0)->childCount(), inputSeries.count());

    foreach(Series* seriesToCompare, inputSeries)
    {
        QCOMPARE(m_qstudyTreeWidget->getSeries(parentStudySeries->getInstanceUID(), seriesToCompare->getInstanceUID(), parentStudySeries->getDICOMSource()) == seriesToCompare, true);
    }
}

void test_QStudyTreeWidget::removeStudy_ShouldRemoveStudy_data()
{
    QTest::addColumn<Patient*>("inputPatient");

    Patient *patient = PatientTestHelper::create(1);
    patient->setID("1");
    patient->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));
    patient->getStudies().at(0)->setInstanceUID("1");

    QTest::newRow("Adding patient to remove study") << patient;
}

void test_QStudyTreeWidget::removeStudy_ShouldRemoveStudy()
{
    QFETCH(Patient*, inputPatient);

    m_qstudyTreeWidget->insertPatient(inputPatient);

    Study *studyToRemove = inputPatient->getStudies().at(0);

    //Comprovem que l'estudi a esborrar existeix al  QStudyTreeWidget
    QCOMPARE(m_qstudyTreeWidget->getStudy(studyToRemove->getInstanceUID(), studyToRemove->getDICOMSource()) == studyToRemove, true);

    m_qstudyTreeWidget->removeStudy(studyToRemove->getInstanceUID(), studyToRemove->getDICOMSource());
    QCOMPARE(m_qstudyTreeWidget->getStudy(studyToRemove->getInstanceUID(), studyToRemove->getDICOMSource()) == NULL, true);
}

void test_QStudyTreeWidget::removeSeries_ShouldRemoveSeries_data()
{
    QTest::addColumn<Patient*>("inputPatient");
    QTest::addColumn<QList<Series*> >("inputSeries");
    QTest::addColumn<Series*>("inputSeriesToRemove");

    Patient *patient = PatientTestHelper::create(1);
    patient->setID("1");
    patient->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));
    patient->getStudies().at(0)->setInstanceUID("1");

    Series *seriesOne = SeriesTestHelper::createSeriesByUID("1");
    seriesOne->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));

    Series *seriesTwo = SeriesTestHelper::createSeriesByUID("2");
    seriesTwo->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));

    QTest::newRow("Adding two series to study") << patient << (QList<Series*>() << seriesOne << seriesTwo) << seriesOne;
}

void test_QStudyTreeWidget::removeSeries_ShouldRemoveSeries()
{
    QFETCH(Patient*, inputPatient);
    QFETCH(QList<Series*>, inputSeries);
    QFETCH(Series*, inputSeriesToRemove);

    m_qstudyTreeWidget->insertPatient(inputPatient);

    Study *parentStudySeries = inputPatient->getStudies().at(0);
    m_qstudyTreeWidget->insertSeriesList(parentStudySeries->getInstanceUID(), inputSeries);

    //Comprovem que la sèrie a esborrar existeix al QStudyTreeWidget
    QCOMPARE(m_qstudyTreeWidget->getSeries(parentStudySeries->getInstanceUID(), inputSeriesToRemove->getInstanceUID(), inputSeriesToRemove->getDICOMSource()) == inputSeriesToRemove, true);

    m_qstudyTreeWidget->removeSeries(parentStudySeries->getInstanceUID(), inputSeriesToRemove->getInstanceUID(), inputSeriesToRemove->getDICOMSource());
    QCOMPARE(m_qstudyTreeWidget->getSeries(parentStudySeries->getInstanceUID(), inputSeriesToRemove->getInstanceUID(), inputSeriesToRemove->getDICOMSource()) == NULL, true);
}

void test_QStudyTreeWidget::removeSeries_ShouldRemoveStudy_data()
{
    QTest::addColumn<Patient*>("inputPatient");
    QTest::addColumn<Series*>("inputSeries");

    Patient *patient = PatientTestHelper::create(1);
    patient->setID("1");
    patient->getStudies().at(0)->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));
    patient->getStudies().at(0)->setInstanceUID("90");

    Series *series = SeriesTestHelper::createSeriesByUID("1");
    series->setDICOMSource(DICOMSourceTestHelper::createAndAddPACSByID("1"));

    QTest::newRow("Adding one series") << patient << series;
}

void test_QStudyTreeWidget::removeSeries_ShouldRemoveStudy()
{
    //Si esborrem una sèrie d'un estudi, i aquesta n'és la única sèrie també s'esborra l'estudi, no té sentit deixar estudi sense sèries, al eliminar-les
    QFETCH(Patient*, inputPatient);
    QFETCH(Series*, inputSeries);

    m_qstudyTreeWidget->insertPatient(inputPatient);
    //Hack: Quan inserim un pacient/estudi, en el QTreeWidget se li insereix un fill, perquè l'estudi es pugui desplegar i emetre signal requestedSeriesOfStudy
    //hem d'eliminar aquest fill, ja que sinó quan eliminem la sèrie l'estudi continua tenint un fill.
    delete m_qstudyTreeWidget->getQTreeWidget()->topLevelItem(0)->child(0);

    Study *parentStudySeries = inputPatient->getStudies().at(0);
    m_qstudyTreeWidget->insertSeriesList(parentStudySeries->getInstanceUID(), QList<Series*>() << inputSeries);

    //Comprovem que l'estudi i la sèrie a esborrar existeix al QStudyTreeWidget
    QCOMPARE(m_qstudyTreeWidget->getStudy(parentStudySeries->getInstanceUID(), parentStudySeries->getDICOMSource()) == parentStudySeries, true);
    QCOMPARE(m_qstudyTreeWidget->getSeries(parentStudySeries->getInstanceUID(), inputSeries->getInstanceUID(), inputSeries->getDICOMSource()) == inputSeries, true);

    m_qstudyTreeWidget->removeSeries(parentStudySeries->getInstanceUID(), inputSeries->getInstanceUID(), inputSeries->getDICOMSource());
    QCOMPARE(m_qstudyTreeWidget->getSeries(parentStudySeries->getInstanceUID(), inputSeries->getInstanceUID(), inputSeries->getDICOMSource()) == NULL, true);
    QCOMPARE(m_qstudyTreeWidget->getStudy(parentStudySeries->getInstanceUID(), parentStudySeries->getDICOMSource()) == NULL, true);
}

DECLARE_TEST(test_QStudyTreeWidget)

#include "test_qstudytreewidget.moc"


