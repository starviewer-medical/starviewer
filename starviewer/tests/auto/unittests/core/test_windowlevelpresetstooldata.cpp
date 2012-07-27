#include "autotest.h"
#include "windowlevelpresetstooldata.h"

using namespace udg;

class test_WindowLevelPresetsToolData : public QObject {
Q_OBJECT

private slots:
    void addPreset_AddsGivenPreset_data();
    void addPreset_AddsGivenPreset();

    void removePreset_WorksAsExpected_data();
    void removePreset_WorksAsExpected();

    void removePresetsFromGroup_WorksAsExpected_data();
    void removePresetsFromGroup_WorksAsExpected();

    void getWindowLevelFromDescription_ReturnsExpectedValues_data();
    void getWindowLevelFromDescription_ReturnsExpectedValues();

    void getGroup_ReturnsExpectedValuesFromExistingPresets_data();
    void getGroup_ReturnsExpectedValuesFromExistingPresets();

    void getGroup_ReturnsExpectedValuesFromNonExistingPresets_data();
    void getGroup_ReturnsExpectedValuesFromNonExistingPresets();

    void getDescriptionsFromGroup_ReturnsExpectedValues_data();
    void getDescriptionsFromGroup_ReturnsExpectedValues();

    void getCurrentPreset_IsEmptyAfterCreation();

    void getCurrentPreset_ReturnsExpectedPresetViaActivatePreset_data();
    void getCurrentPreset_ReturnsExpectedPresetViaActivatePreset();

    void getCurrentPreset_ReturnsExpectedPresetViaSetCustomWindowLevel();

    void getFileDefinedPresetIndex_ReturnsExpectedValues_data();
    void getFileDefinedPresetIndex_ReturnsExpectedValues();

    void updateCurrentFileDefinedPreset_WorksAsExpected_data();
    void updateCurrentFileDefinedPreset_WorksAsExpected();

    void setCustomWindowLevel_UpdatesValues_data();
    void setCustomWindowLevel_UpdatesValues();

    void activatePreset_WorksAsExpected_data();
    void activatePreset_WorksAsExpected();

private:
    static const QString AutoPreset1Name;
    static const QString AutoPreset2Name;
    static const QString FilePreset1Name;
    static const QString FilePreset2Name;
    static const QString StandardPreset1Name;
    static const QString StandardPreset2Name;
    static const QString UserPreset1Name;
    static const QString CustomPreset1Name;
    static const QString OtherPreset1Name;
    static const QString NonExistingPresetName;

private:
    WindowLevelPresetsToolData* getWindowLevelPresetsSample();
};

const QString test_WindowLevelPresetsToolData::AutoPreset1Name("Preset 1 - Auto");
const QString test_WindowLevelPresetsToolData::AutoPreset2Name("Preset 2 - Auto");
const QString test_WindowLevelPresetsToolData::FilePreset1Name("Preset 1 - File");
const QString test_WindowLevelPresetsToolData::FilePreset2Name("Preset 2 - File");
const QString test_WindowLevelPresetsToolData::StandardPreset1Name("CT Lung - Standard");
const QString test_WindowLevelPresetsToolData::StandardPreset2Name("CT Bone - Standard");
const QString test_WindowLevelPresetsToolData::UserPreset1Name("Preset 1 - User");
const QString test_WindowLevelPresetsToolData::CustomPreset1Name("Preset 1 - Custom");
const QString test_WindowLevelPresetsToolData::OtherPreset1Name("Preset 1 - Other");
const QString test_WindowLevelPresetsToolData::NonExistingPresetName("Dummy name");

void test_WindowLevelPresetsToolData::addPreset_AddsGivenPreset_data()
{
    QTest::addColumn<QString>("presetDescription");
    QTest::addColumn<double>("window");
    QTest::addColumn<double>("level");
    QTest::addColumn<int>("group");

    QTest::newRow("Preset 1") << "Preset name 1" << 0.0 << 0.0 << 0;
    QTest::newRow("Preset 2") << "Preset name 2" << 128.0 << 256.0 << 100;
    QTest::newRow("Preset 3 in 'FileDefined' group") << "Preset 3" << 1.0 << 1.0 << static_cast<int>(WindowLevelPresetsToolData::FileDefined);
}

void test_WindowLevelPresetsToolData::addPreset_AddsGivenPreset()
{
    QFETCH(QString, presetDescription);
    QFETCH(double, window);
    QFETCH(double, level);
    QFETCH(int, group);

    WindowLevelPresetsToolData wlData;
    wlData.addPreset(presetDescription, window, level, group);

    double dummyWindow, dummyLevel;
    QVERIFY(wlData.getWindowLevelFromDescription(presetDescription, dummyWindow, dummyLevel));
}

void test_WindowLevelPresetsToolData::removePreset_WorksAsExpected_data()
{
    QTest::addColumn<QString>("presetToRemove");

    QTest::newRow("Remove existing preset [Auto]") << AutoPreset1Name;
    QTest::newRow("Remove existing preset [File]") << FilePreset1Name;
    QTest::newRow("Remove existing preset [Standard]") << StandardPreset2Name;
    QTest::newRow("Remove existing preset [User]") << UserPreset1Name;
    QTest::newRow("Remove existing preset [Custom]") << CustomPreset1Name;
    QTest::newRow("Remove existing preset [Other]") << OtherPreset1Name;
    QTest::newRow("Remove non-existing preset") << NonExistingPresetName;
}

void test_WindowLevelPresetsToolData::removePreset_WorksAsExpected()
{
    QFETCH(QString, presetToRemove);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->removePreset(presetToRemove);

    double dummyWindow, dummyLevel;
    QVERIFY(!wlData->getWindowLevelFromDescription(presetToRemove, dummyWindow, dummyLevel));

    delete wlData;
}

void test_WindowLevelPresetsToolData::removePresetsFromGroup_WorksAsExpected_data()
{
    QTest::addColumn<int>("groupToDelete");

    QTest::newRow("Remove 'auto' presets") << static_cast<int>(WindowLevelPresetsToolData::AutomaticPreset);
    QTest::newRow("Remove 'file defined' presets") << static_cast<int>(WindowLevelPresetsToolData::FileDefined);
    QTest::newRow("Remove 'standard' presets") << static_cast<int>(WindowLevelPresetsToolData::StandardPresets);
    QTest::newRow("Remove 'user defined' presets") << static_cast<int>(WindowLevelPresetsToolData::UserDefined);
    QTest::newRow("Remove 'custom' presets") << static_cast<int>(WindowLevelPresetsToolData::CustomPreset);
    QTest::newRow("Remove 'other' presets") << static_cast<int>(WindowLevelPresetsToolData::Other);
    QTest::newRow("Remove non-existing presets group") << -1;
}

void test_WindowLevelPresetsToolData::removePresetsFromGroup_WorksAsExpected()
{
    QFETCH(int, groupToDelete);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->removePresetsFromGroup(groupToDelete);

    QVERIFY(wlData->getDescriptionsFromGroup(groupToDelete).isEmpty());

    delete wlData;
}

void test_WindowLevelPresetsToolData::getWindowLevelFromDescription_ReturnsExpectedValues_data()
{
    QTest::addColumn<QString>("presetName");
    QTest::addColumn<double>("window");
    QTest::addColumn<double>("level");
    QTest::addColumn<bool>("returnValue");

    QTest::newRow("Existing auto preset") << AutoPreset2Name << 1024.0 << 512.0 << true;
    QTest::newRow("Existing file preset") << FilePreset1Name << 328.5 << 678.9 << true;
    QTest::newRow("Existing standard preset") << StandardPreset1Name << 1500.0 << -650.0 << true;
    QTest::newRow("Existing user preset") << UserPreset1Name << 110.0 << 900.0 << true;
    QTest::newRow("Existing custom preset") << CustomPreset1Name << 1231.0 << 412.0 << true;
    QTest::newRow("Existing other preset") << OtherPreset1Name << 100.0 << 200.0 << true;
    QTest::newRow("NON-Existing preset") << NonExistingPresetName << 0.0 << 0.0 << false;
}

void test_WindowLevelPresetsToolData::getWindowLevelFromDescription_ReturnsExpectedValues()
{
    QFETCH(QString, presetName);
    QFETCH(double, window);
    QFETCH(double, level);
    QFETCH(bool, returnValue);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    
    double returnedWindow, returnedLevel;
    QCOMPARE(wlData->getWindowLevelFromDescription(presetName, returnedWindow, returnedLevel), returnValue);
    QCOMPARE(returnedWindow, window);
    QCOMPARE(returnedLevel, level);

    delete wlData;
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromExistingPresets_data()
{
    QTest::addColumn<QString>("presetName");
    QTest::addColumn<int>("expectedGroup");

    QTest::newRow("Auto preset") << AutoPreset1Name << static_cast<int>(WindowLevelPresetsToolData::AutomaticPreset);
    QTest::newRow("File preset") << FilePreset1Name << static_cast<int>(WindowLevelPresetsToolData::FileDefined);
    QTest::newRow("Standard preset") << StandardPreset1Name << static_cast<int>(WindowLevelPresetsToolData::StandardPresets);
    QTest::newRow("Custom preset") << CustomPreset1Name << static_cast<int>(WindowLevelPresetsToolData::CustomPreset);
    QTest::newRow("User preset") << UserPreset1Name << static_cast<int>(WindowLevelPresetsToolData::UserDefined);
    QTest::newRow("Other preset") << OtherPreset1Name << static_cast<int>(WindowLevelPresetsToolData::Other);
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromExistingPresets()
{
    QFETCH(QString, presetName);
    QFETCH(int, expectedGroup);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();

    int returnedGroup;
    QVERIFY(wlData->getGroup(presetName, returnedGroup));
    QCOMPARE(returnedGroup, expectedGroup); 

    delete wlData;
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromNonExistingPresets_data()
{
    QTest::addColumn<QString>("presetName");

    QTest::newRow("Non-existing preset 0") << NonExistingPresetName;
    QTest::newRow("Non-existing preset 1") << "non-existing preset";
    QTest::newRow("Non-existing preset 2") << "ABDCJS-##@|@@##-123456";
    QTest::newRow("Non-existing preset 3") << "[" + AutoPreset1Name + "]";
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromNonExistingPresets()
{
    QFETCH(QString, presetName);
    
    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();

    int returnedDummyGroup;
    QVERIFY(!wlData->getGroup(presetName, returnedDummyGroup));

    delete wlData;
}

void test_WindowLevelPresetsToolData::getDescriptionsFromGroup_ReturnsExpectedValues_data()
{
    QTest::addColumn<int>("group");
    QTest::addColumn<QStringList>("expectedPresetsNames");

    QStringList presetList;

    presetList << AutoPreset1Name << AutoPreset2Name;
    QTest::newRow("Auto presets") << static_cast<int>(WindowLevelPresetsToolData::AutomaticPreset) << presetList;
    
    presetList.clear();
    presetList << FilePreset1Name << FilePreset2Name;
    QTest::newRow("File presets") << static_cast<int>(WindowLevelPresetsToolData::FileDefined) << presetList;
    
    presetList.clear();
    presetList << StandardPreset1Name << StandardPreset2Name;
    QTest::newRow("Standard presets") << static_cast<int>(WindowLevelPresetsToolData::StandardPresets) << presetList;
    
    presetList.clear();
    presetList << CustomPreset1Name;
    QTest::newRow("Custom presets") << static_cast<int>(WindowLevelPresetsToolData::CustomPreset) << presetList;
    
    presetList.clear();
    presetList << UserPreset1Name;
    QTest::newRow("User presets") << static_cast<int>(WindowLevelPresetsToolData::UserDefined) << presetList;
    
    presetList.clear();
    presetList << OtherPreset1Name;
    QTest::newRow("Other presets") << static_cast<int>(WindowLevelPresetsToolData::Other) << presetList;
}

void test_WindowLevelPresetsToolData::getDescriptionsFromGroup_ReturnsExpectedValues()
{
    QFETCH(int, group);
    QFETCH(QStringList, expectedPresetsNames);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();

    QStringList namesList = wlData->getDescriptionsFromGroup(group);
    QCOMPARE(namesList.count(), expectedPresetsNames.count());
    
    foreach (QString presetName, expectedPresetsNames)
    {
        QVERIFY(namesList.contains(presetName));
    }

    delete wlData;
}

void test_WindowLevelPresetsToolData::getCurrentPreset_IsEmptyAfterCreation()
{
    WindowLevelPresetsToolData wlData;

    QVERIFY(wlData.getCurrentPreset().isEmpty());
}

void test_WindowLevelPresetsToolData::getCurrentPreset_ReturnsExpectedPresetViaActivatePreset_data()
{
    QTest::addColumn<QString>("presetNameToActivate");
    QTest::addColumn<QString>("expectedCurrentPresetName");

    QTest::newRow("Existing activated preset") << StandardPreset1Name << StandardPreset1Name;
    QTest::newRow("Non-existing activated preset") << NonExistingPresetName << QString();
}

void test_WindowLevelPresetsToolData::getCurrentPreset_ReturnsExpectedPresetViaActivatePreset()
{
    QFETCH(QString, presetNameToActivate);
    QFETCH(QString, expectedCurrentPresetName);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->activatePreset(presetNameToActivate);
    
    QCOMPARE(wlData->getCurrentPreset(), expectedCurrentPresetName);

    delete wlData;
}

void test_WindowLevelPresetsToolData::getCurrentPreset_ReturnsExpectedPresetViaSetCustomWindowLevel()
{
    WindowLevelPresetsToolData wlData;

    wlData.setCustomWindowLevel(1024.0, 512.0);
    QCOMPARE(wlData.getCurrentPreset(), tr("Custom"));
}

void test_WindowLevelPresetsToolData::getFileDefinedPresetIndex_ReturnsExpectedValues_data()
{
    QTest::addColumn<QString>("presetName");
    QTest::addColumn<int>("expectedIndex");

    QTest::newRow("Existing file defined preset 1") << FilePreset1Name << 0;
    QTest::newRow("Existing file defined preset 2") << FilePreset2Name << 1;
    QTest::newRow("Existing preset, not in file defined group") << AutoPreset1Name << -1;
    QTest::newRow("Non-Existing preset") << NonExistingPresetName << -1;
}

void test_WindowLevelPresetsToolData::getFileDefinedPresetIndex_ReturnsExpectedValues()
{
    QFETCH(QString, presetName);
    QFETCH(int, expectedIndex);

    WindowLevelPresetsToolData *wlData = getWindowLevelPresetsSample();

    QCOMPARE(wlData->getFileDefinedPresetIndex(presetName), expectedIndex);
    
    delete wlData;
}

void test_WindowLevelPresetsToolData::updateCurrentFileDefinedPreset_WorksAsExpected_data()
{
    QTest::addColumn<QString>("presetToActivate");
    QTest::addColumn<double>("windowToUpdate");
    QTest::addColumn<double>("levelToUpdate");
    QTest::addColumn<double>("windowAfterUpdate");
    QTest::addColumn<double>("levelAfterUpdate");

    QTest::newRow("Activated preset exists and is file defined") << FilePreset1Name << 1.5 << 2.5 << 1.5 << 2.5;
    QTest::newRow("Activated preset exists but is not file defined") << AutoPreset1Name << 1.5 << 2.5 << 128.0 << 256.0;
    QTest::newRow("Activated preset does not exist") << NonExistingPresetName << 1.5 << 2.5 << 0.0 << 0.0;
}

void test_WindowLevelPresetsToolData::updateCurrentFileDefinedPreset_WorksAsExpected()
{
    QFETCH(QString, presetToActivate);
    QFETCH(double, windowToUpdate);
    QFETCH(double, levelToUpdate);
    QFETCH(double, windowAfterUpdate);
    QFETCH(double, levelAfterUpdate);
    
    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->activatePreset(presetToActivate);
    wlData->updateCurrentFileDefinedPreset(windowToUpdate, levelToUpdate);
    
    double window, level;
    wlData->getWindowLevelFromDescription(wlData->getCurrentPreset(), window, level);
    
    QCOMPARE(window, windowAfterUpdate);
    QCOMPARE(level,levelAfterUpdate);
    
    delete wlData;
}

void test_WindowLevelPresetsToolData::setCustomWindowLevel_UpdatesValues_data()
{
    QTest::addColumn<double>("window");
    QTest::addColumn<double>("level");

    QTest::newRow("Some values 1") << 0.0 << 0.0;
    QTest::newRow("Some values 2") << 1.0 << 0.0;
    QTest::newRow("Some values 3") << 0.0 << 1.0;
    QTest::newRow("Some values 4") << 1.0 << 1.0;
}

void test_WindowLevelPresetsToolData::setCustomWindowLevel_UpdatesValues()
{
    QFETCH(double, window);
    QFETCH(double, level);

    WindowLevelPresetsToolData wlData;
    wlData.setCustomWindowLevel(window, level);

    double returnedWindow, returnedLevel;
    wlData.getWindowLevelFromDescription(tr("Custom"), returnedWindow, returnedLevel);

    QCOMPARE(returnedWindow, window);
    QCOMPARE(returnedLevel, level);
}

void test_WindowLevelPresetsToolData::activatePreset_WorksAsExpected_data()
{
    QTest::addColumn<QString>("presetToActivate");
    QTest::addColumn<QString>("currentActivatedPreset");

    QTest::newRow("Activate existing preset [Auto]") << AutoPreset1Name << AutoPreset1Name;
    QTest::newRow("Activate existing preset [File]") << FilePreset1Name << FilePreset1Name;
    QTest::newRow("Activate existing preset [Standard]") << StandardPreset1Name << StandardPreset1Name;
    QTest::newRow("Activate existing preset [User]") << UserPreset1Name << UserPreset1Name;
    QTest::newRow("Activate existing preset [Custom]") << CustomPreset1Name << CustomPreset1Name;
    QTest::newRow("Activate existing preset [Other]") << OtherPreset1Name << OtherPreset1Name;
    QTest::newRow("Activate non-existing preset") << NonExistingPresetName << QString();
}

void test_WindowLevelPresetsToolData::activatePreset_WorksAsExpected()
{
    QFETCH(QString, presetToActivate);
    QFETCH(QString, currentActivatedPreset);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();

    wlData->activatePreset(presetToActivate);
    QCOMPARE(wlData->getCurrentPreset(), currentActivatedPreset);
    
    delete wlData;
}

WindowLevelPresetsToolData* test_WindowLevelPresetsToolData::getWindowLevelPresetsSample()
{
    WindowLevelPresetsToolData* wlData = new WindowLevelPresetsToolData();
    // Eliminem els presets que es posen per defecte al construir l'objecte de cares a tenir més ben controlat el testing del que aquí posem
    wlData->removePresetsFromGroup(WindowLevelPresetsToolData::StandardPresets);
    wlData->removePresetsFromGroup(WindowLevelPresetsToolData::CustomPreset);
    wlData->addPreset(AutoPreset1Name, 128.0, 256.0, WindowLevelPresetsToolData::AutomaticPreset);
    wlData->addPreset(AutoPreset2Name, 1024.0, 512.0, WindowLevelPresetsToolData::AutomaticPreset);
    wlData->addPreset(FilePreset1Name, 328.5, 678.9, WindowLevelPresetsToolData::FileDefined);
    wlData->addPreset(FilePreset2Name, 128.3, 583.4, WindowLevelPresetsToolData::FileDefined);
    wlData->addPreset(StandardPreset1Name, 1500, -650, WindowLevelPresetsToolData::StandardPresets);
    wlData->addPreset(StandardPreset2Name, 2000, 500, WindowLevelPresetsToolData::StandardPresets);
    wlData->addPreset(UserPreset1Name, 110, 900, WindowLevelPresetsToolData::UserDefined);
    wlData->addPreset(CustomPreset1Name, 1231, 412, WindowLevelPresetsToolData::CustomPreset);
    wlData->addPreset(OtherPreset1Name, 100, 200, WindowLevelPresetsToolData::Other);

    return wlData;
}

DECLARE_TEST(test_WindowLevelPresetsToolData)

#include "test_windowlevelpresetstooldata.moc"
