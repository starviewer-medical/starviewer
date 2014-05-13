#include "autotest.h"
#include "windowlevelpresetstooldata.h"
#include "windowlevel.h"
#include "mathtools.h"
#include <QDebug>

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

    void getFromDescription_ReturnsExpectedValues_data();
    void getFromDescription_ReturnsExpectedValues();

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

    void updatePreset_WorksAsExpected_data();
    void updatePreset_WorksAsExpected();

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

    static const WindowLevel AutoPreset1;
    static const WindowLevel AutoPreset2;
    static const WindowLevel FilePreset1;
    static const WindowLevel FilePreset2;
    static const WindowLevel StandardPreset1;
    static const WindowLevel StandardPreset2;
    static const WindowLevel UserPreset1;
    static const WindowLevel CustomPreset1;
    static const WindowLevel OtherPreset1;
    static const WindowLevel NonExistingPreset;

private:
    WindowLevelPresetsToolData* getWindowLevelPresetsSample();
};

Q_DECLARE_METATYPE(WindowLevel)

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

const WindowLevel test_WindowLevelPresetsToolData::AutoPreset1(128.0, 256.0, AutoPreset1Name);
const WindowLevel test_WindowLevelPresetsToolData::AutoPreset2(1024.0, 512.0, AutoPreset2Name);
const WindowLevel test_WindowLevelPresetsToolData::FilePreset1(328.5, 678.9, FilePreset1Name);
const WindowLevel test_WindowLevelPresetsToolData::FilePreset2(128.3, 583.4, FilePreset2Name);
const WindowLevel test_WindowLevelPresetsToolData::StandardPreset1(1500, -650, StandardPreset1Name);
const WindowLevel test_WindowLevelPresetsToolData::StandardPreset2(2000, 500, StandardPreset2Name);
const WindowLevel test_WindowLevelPresetsToolData::UserPreset1(110, 900, UserPreset1Name);
const WindowLevel test_WindowLevelPresetsToolData::CustomPreset1(1231, 412, CustomPreset1Name);
const WindowLevel test_WindowLevelPresetsToolData::OtherPreset1(100, 200, OtherPreset1Name);
const WindowLevel test_WindowLevelPresetsToolData::NonExistingPreset(0.0, 0.0, NonExistingPresetName);

void test_WindowLevelPresetsToolData::addPreset_AddsGivenPreset_data()
{
    QTest::addColumn<QString>("presetDescription");
    QTest::addColumn<WindowLevel>("windowLevel");
    QTest::addColumn<int>("group");

    QTest::newRow("Preset 1") << "Preset name 1" << WindowLevel(0.0, 0.0, "Preset name 1") << 0;
    QTest::newRow("Preset 2") << "Preset name 2" << WindowLevel(128.0, 256.0, "Preset name 2") << 100;
    QTest::newRow("Preset 3 in 'FileDefined' group") << "Preset 3" << WindowLevel(1.0, 1.0, "Preset 3") << static_cast<int>(WindowLevelPresetsToolData::FileDefined);
}

void test_WindowLevelPresetsToolData::addPreset_AddsGivenPreset()
{
    QFETCH(QString, presetDescription);
    QFETCH(WindowLevel, windowLevel);
    QFETCH(int, group);

    WindowLevelPresetsToolData wlData;
    wlData.addPreset(windowLevel, group);

    WindowLevel dummyWindowLevel;
    QVERIFY(wlData.getFromDescription(presetDescription, dummyWindowLevel));
}

void test_WindowLevelPresetsToolData::removePreset_WorksAsExpected_data()
{
    QTest::addColumn<WindowLevel>("presetToRemove");

    QTest::newRow("Remove existing preset [Auto]") << AutoPreset1;
    QTest::newRow("Remove existing preset [File]") << FilePreset1;
    QTest::newRow("Remove existing preset [Standard]") << StandardPreset2;
    QTest::newRow("Remove existing preset [User]") << UserPreset1;
    QTest::newRow("Remove existing preset [Custom]") << CustomPreset1;
    QTest::newRow("Remove existing preset [Other]") << OtherPreset1;
    QTest::newRow("Remove non-existing preset") << NonExistingPreset;
}

void test_WindowLevelPresetsToolData::removePreset_WorksAsExpected()
{
    QFETCH(WindowLevel, presetToRemove);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->removePreset(presetToRemove);

    WindowLevel dummyWindowLevel;
    QVERIFY(!wlData->getFromDescription(presetToRemove.getName(), dummyWindowLevel));

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

void test_WindowLevelPresetsToolData::getFromDescription_ReturnsExpectedValues_data()
{
    QTest::addColumn<QString>("presetName");
    QTest::addColumn<WindowLevel>("preset");
    QTest::addColumn<bool>("returnValue");

    QTest::newRow("Existing auto preset") << AutoPreset2Name << AutoPreset2 << true;
    QTest::newRow("Existing file preset") << FilePreset1Name << FilePreset1 << true;
    QTest::newRow("Existing standard preset") << StandardPreset1Name << StandardPreset1 << true;
    QTest::newRow("Existing user preset") << UserPreset1Name << UserPreset1 << true;
    QTest::newRow("Existing custom preset") << CustomPreset1Name << CustomPreset1 << true;
    QTest::newRow("Existing other preset") << OtherPreset1Name << OtherPreset1 << true;
    QTest::newRow("NON-Existing preset") << NonExistingPresetName << WindowLevel() << false;
}

void test_WindowLevelPresetsToolData::getFromDescription_ReturnsExpectedValues()
{
    QFETCH(QString, presetName);
    QFETCH(WindowLevel, preset);
    QFETCH(bool, returnValue);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    
    WindowLevel returnedWindowLevel;
    QCOMPARE(wlData->getFromDescription(presetName, returnedWindowLevel), returnValue);
    QCOMPARE(preset, returnedWindowLevel);

    delete wlData;
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromExistingPresets_data()
{
    QTest::addColumn<WindowLevel>("preset");
    QTest::addColumn<int>("expectedGroup");

    QTest::newRow("Auto preset") << AutoPreset1 << static_cast<int>(WindowLevelPresetsToolData::AutomaticPreset);
    QTest::newRow("File preset") << FilePreset1 << static_cast<int>(WindowLevelPresetsToolData::FileDefined);
    QTest::newRow("Standard preset") << StandardPreset1 << static_cast<int>(WindowLevelPresetsToolData::StandardPresets);
    QTest::newRow("Custom preset") << CustomPreset1 << static_cast<int>(WindowLevelPresetsToolData::CustomPreset);
    QTest::newRow("User preset") << UserPreset1 << static_cast<int>(WindowLevelPresetsToolData::UserDefined);
    QTest::newRow("Other preset") << OtherPreset1 << static_cast<int>(WindowLevelPresetsToolData::Other);
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromExistingPresets()
{
    QFETCH(WindowLevel, preset);
    QFETCH(int, expectedGroup);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();

    int returnedGroup;
    QVERIFY(wlData->getGroup(preset, returnedGroup));
    QCOMPARE(returnedGroup, expectedGroup); 

    delete wlData;
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromNonExistingPresets_data()
{
    QTest::addColumn<WindowLevel>("preset");

    QTest::newRow("Non-existing preset 0") << NonExistingPreset;
    QTest::newRow("Non-existing preset 1") << WindowLevel(MathTools::randomDouble(1, 6000), MathTools::randomDouble(-6000, 6000), "non-existing preset");
    QTest::newRow("Non-existing preset 2") << WindowLevel(MathTools::randomDouble(1, 6000), MathTools::randomDouble(-6000, 6000), "ABDCJS-##@|@@##-123456");
    QTest::newRow("Non-existing preset 3") << WindowLevel(MathTools::randomDouble(1, 6000), MathTools::randomDouble(-6000, 6000), "[" + AutoPreset1Name + "]");
}

void test_WindowLevelPresetsToolData::getGroup_ReturnsExpectedValuesFromNonExistingPresets()
{
    QFETCH(WindowLevel, preset);
    
    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();

    int returnedDummyGroup;
    QVERIFY(!wlData->getGroup(preset, returnedDummyGroup));

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
    
    foreach (const QString &presetName, expectedPresetsNames)
    {
        QVERIFY(namesList.contains(presetName));
    }

    delete wlData;
}

void test_WindowLevelPresetsToolData::getCurrentPreset_IsEmptyAfterCreation()
{
    WindowLevelPresetsToolData wlData;

    QCOMPARE(wlData.getCurrentPreset(), WindowLevel());
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
    wlData->selectCurrentPreset(presetNameToActivate);
    
    QCOMPARE(wlData->getCurrentPreset().getName(), expectedCurrentPresetName);

    delete wlData;
}

void test_WindowLevelPresetsToolData::getCurrentPreset_ReturnsExpectedPresetViaSetCustomWindowLevel()
{
    WindowLevelPresetsToolData wlData;

    wlData.setCustomWindowLevel(1024.0, 512.0);
    QCOMPARE(wlData.getCurrentPreset(), WindowLevel(1024.0, 512.0, tr("Custom")));
}

void test_WindowLevelPresetsToolData::updatePreset_WorksAsExpected_data()
{
    QTest::addColumn<WindowLevel>("windowLevelToUpdate");
    QTest::addColumn<WindowLevel>("windowLevelAfterUpdate");

    QTest::newRow("Activated preset exists and is file defined") << WindowLevel(1.5, 2.5, FilePreset1Name) << WindowLevel(1.5, 2.5, FilePreset1Name);
    QTest::newRow("Activated preset exists but is not file defined") << WindowLevel(128.0, 256.0, AutoPreset1Name) << WindowLevel(128.0, 256.0, AutoPreset1Name);
    QTest::newRow("Activated preset does not exist") << WindowLevel(1.5, 2.5, NonExistingPresetName) << WindowLevel();
}

void test_WindowLevelPresetsToolData::updatePreset_WorksAsExpected()
{
    QFETCH(WindowLevel, windowLevelToUpdate);
    QFETCH(WindowLevel, windowLevelAfterUpdate);
    
    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->updatePreset(windowLevelToUpdate);
    
    WindowLevel obtainedWindowLevel;
    wlData->getFromDescription(windowLevelToUpdate.getName(), obtainedWindowLevel);
    
    QCOMPARE(obtainedWindowLevel, windowLevelAfterUpdate);
    
    delete wlData;
}

void test_WindowLevelPresetsToolData::setCustomWindowLevel_UpdatesValues_data()
{
    QTest::addColumn<WindowLevel>("customWindowLevel");

    QTest::newRow("Some values 1") << WindowLevel(0.0, 0.0, tr("Custom"));
    QTest::newRow("Some values 2") << WindowLevel(1.0, 0.0, tr("Custom"));
    QTest::newRow("Some values 3") << WindowLevel(0.0, 1.0, tr("Custom"));
    QTest::newRow("Some values 4") << WindowLevel(1.0, 1.0, tr("Custom"));
}

void test_WindowLevelPresetsToolData::setCustomWindowLevel_UpdatesValues()
{
    QFETCH(WindowLevel, customWindowLevel);

    WindowLevelPresetsToolData wlData;
    wlData.setCustomWindowLevel(customWindowLevel.getWidth(), customWindowLevel.getCenter());
    WindowLevel returnedWindowLevel;
    wlData.getFromDescription(tr("Custom"), returnedWindowLevel);

    QCOMPARE(returnedWindowLevel, customWindowLevel);
}

void test_WindowLevelPresetsToolData::activatePreset_WorksAsExpected_data()
{
    QTest::addColumn<QString>("presetNameToActivate");
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
    QFETCH(QString, presetNameToActivate);
    QFETCH(QString, currentActivatedPreset);

    WindowLevelPresetsToolData* wlData = getWindowLevelPresetsSample();

    wlData->selectCurrentPreset(presetNameToActivate);
    QCOMPARE(wlData->getCurrentPreset().getName(), currentActivatedPreset);
    
    delete wlData;
}

WindowLevelPresetsToolData* test_WindowLevelPresetsToolData::getWindowLevelPresetsSample()
{
    WindowLevelPresetsToolData* wlData = new WindowLevelPresetsToolData();
    // Eliminem els presets que es posen per defecte al construir l'objecte de cares a tenir més ben controlat el testing del que aquí posem
    wlData->removePresetsFromGroup(WindowLevelPresetsToolData::StandardPresets);
    wlData->removePresetsFromGroup(WindowLevelPresetsToolData::CustomPreset);
    wlData->addPreset(AutoPreset1, WindowLevelPresetsToolData::AutomaticPreset);
    wlData->addPreset(AutoPreset2, WindowLevelPresetsToolData::AutomaticPreset);
    wlData->addPreset(FilePreset1, WindowLevelPresetsToolData::FileDefined);
    wlData->addPreset(FilePreset2, WindowLevelPresetsToolData::FileDefined);
    wlData->addPreset(StandardPreset1, WindowLevelPresetsToolData::StandardPresets);
    wlData->addPreset(StandardPreset2, WindowLevelPresetsToolData::StandardPresets);
    wlData->addPreset(UserPreset1, WindowLevelPresetsToolData::UserDefined);
    wlData->addPreset(CustomPreset1, WindowLevelPresetsToolData::CustomPreset);
    wlData->addPreset(OtherPreset1, WindowLevelPresetsToolData::Other);

    return wlData;
}

DECLARE_TEST(test_WindowLevelPresetsToolData)

#include "test_windowlevelpresetstooldata.moc"
