#include "autotest.h"
#include "syncactionsconfiguration.h"

#include "zoomfactorsyncaction.h"
#include "voilutsyncaction.h"
#include "anatomicalplanesynccriterion.h"
#include "inputsynccriterion.h"

using namespace udg;

class test_SyncActionsConfiguration : public QObject {
Q_OBJECT

private slots:
    void enableSyncAction_ShouldFunctionAsExpected_data();
    void enableSyncAction_ShouldFunctionAsExpected();

    void isSyncActionEnabled_ShouldReturnExpectedValues_data();
    void isSyncActionEnabled_ShouldReturnExpectedValues();

    void getSyncCriteria_ReturnsExpectedSyncCriteriaList_data();
    void getSyncCriteria_ReturnsExpectedSyncCriteriaList();

    void removeSyncCriteria_RemovesCriteriaCorrectly_data();
    void removeSyncCriteria_RemovesCriteriaCorrectly();

private:
    SyncActionsConfiguration getSyncActionsConfigurationSample();
};

Q_DECLARE_METATYPE(SyncActionMetaData)
Q_DECLARE_METATYPE(SyncAction*)
Q_DECLARE_METATYPE(QList<SyncCriterion*>)

SyncActionsConfiguration test_SyncActionsConfiguration::getSyncActionsConfigurationSample()
{
    SyncActionsConfiguration configuration;

    configuration.enableSyncAction(SyncActionMetaData("SyncAction 1", QString(), QString()));
    configuration.enableSyncAction(SyncActionMetaData("SyncAction 2", QString(), QString()));
    configuration.enableSyncAction(SyncActionMetaData("SyncAction 3", QString(), QString()));

    return configuration;
}

void test_SyncActionsConfiguration::enableSyncAction_ShouldFunctionAsExpected_data()
{
    QTest::addColumn<SyncActionMetaData>("syncActionMetaData");
    QTest::addColumn<bool>("enable");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("Enable non-existing sync action") << SyncActionMetaData("non-existing SyncAction", QString(), QString()) << true << true;
    QTest::newRow("Enable existing sync action") << SyncActionMetaData("SyncAction 1", QString(), QString()) << true << true;
    QTest::newRow("Disabling non-existing sync action") << SyncActionMetaData("non-existing SyncAction", QString(), QString()) << false << false;
    QTest::newRow("Disabling existing sync action") << SyncActionMetaData("SyncAction 1", QString(), QString()) << false << false;
}

void test_SyncActionsConfiguration::enableSyncAction_ShouldFunctionAsExpected()
{
    QFETCH(SyncActionMetaData, syncActionMetaData);
    QFETCH(bool, enable);
    QFETCH(bool, expectedResult);

    SyncActionsConfiguration configuration = getSyncActionsConfigurationSample();
    configuration.enableSyncAction(syncActionMetaData, enable);
    
    QCOMPARE(configuration.isSyncActionEnabled(syncActionMetaData), expectedResult);
}

void test_SyncActionsConfiguration::isSyncActionEnabled_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<SyncActionMetaData>("syncActionMetaData");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("Enabled sync action") << SyncActionMetaData("SyncAction 1", QString(), QString()) << true;
    QTest::newRow("Disabled sync action") << SyncActionMetaData("disabled SyncAction", QString(), QString()) << false;
}

void test_SyncActionsConfiguration::isSyncActionEnabled_ShouldReturnExpectedValues()
{
    QFETCH(SyncActionMetaData, syncActionMetaData);
    QFETCH(bool, expectedResult);

    SyncActionsConfiguration configuration = getSyncActionsConfigurationSample();
    
    QCOMPARE(configuration.isSyncActionEnabled(syncActionMetaData), expectedResult);
}

void test_SyncActionsConfiguration::getSyncCriteria_ReturnsExpectedSyncCriteriaList_data()
{
    QTest::addColumn<SyncAction*>("syncAction");
    QTest::addColumn<QList<SyncCriterion*> >("criteriaListToConfigure");
    QTest::addColumn<QList<SyncCriterion*> >("expectedCriteriaList");

    SyncAction *zoomAction = new ZoomFactorSyncAction();
    QTest::newRow("No criteria added to configuration, default criteria returned") << zoomAction << QList<SyncCriterion*>() << zoomAction->getDefaultSyncCriteria();

    QList<SyncCriterion*> criteria;
    criteria << new AnatomicalPlaneSyncCriterion();

    SyncAction *windowLevelAction = new VoiLutSyncAction();
    QTest::newRow("Criteria added to configuration, configured criteria returned") << windowLevelAction << criteria << criteria;
}

void test_SyncActionsConfiguration::getSyncCriteria_ReturnsExpectedSyncCriteriaList()
{
    QFETCH(SyncAction*, syncAction);
    QFETCH(QList<SyncCriterion*>, criteriaListToConfigure);
    QFETCH(QList<SyncCriterion*>, expectedCriteriaList);

    SyncActionsConfiguration configuration = getSyncActionsConfigurationSample();
    foreach(SyncCriterion *criterion, criteriaListToConfigure)
    {
        configuration.addSyncCriterion(syncAction->getMetaData(), criterion);
    }

    QCOMPARE(configuration.getSyncCriteria(syncAction), expectedCriteriaList);
}

void test_SyncActionsConfiguration::removeSyncCriteria_RemovesCriteriaCorrectly_data()
{
    QTest::addColumn<SyncAction*>("syncAction");
    QTest::addColumn<QList<SyncCriterion*> >("criteriaListToConfigure");

    QList<SyncCriterion*> criteria;
    criteria << new AnatomicalPlaneSyncCriterion() << new InputSyncCriterion();

    SyncAction *windowLevelAction = new VoiLutSyncAction();
    QTest::newRow("Configuration with added criteria for a particular sync action") << windowLevelAction << criteria;
}

void test_SyncActionsConfiguration::removeSyncCriteria_RemovesCriteriaCorrectly()
{
    QFETCH(SyncAction*, syncAction);
    QFETCH(QList<SyncCriterion*>, criteriaListToConfigure);

    SyncActionsConfiguration configuration = getSyncActionsConfigurationSample();
    foreach(SyncCriterion *criterion, criteriaListToConfigure)
    {
        configuration.addSyncCriterion(syncAction->getMetaData(), criterion);
    }

    configuration.removeSyncCriteria(syncAction->getMetaData());

    QCOMPARE(configuration.getSyncCriteria(syncAction), syncAction->getDefaultSyncCriteria());
}

DECLARE_TEST(test_SyncActionsConfiguration)

#include "test_syncactionsconfiguration.moc"
