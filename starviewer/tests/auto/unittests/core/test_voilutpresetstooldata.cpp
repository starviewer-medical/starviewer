#include "autotest.h"
#include "voilutpresetstooldata.h"
#include "windowlevel.h"
#include "mathtools.h"
#include <QDebug>

using namespace udg;

class test_VoiLutPresetsToolData : public QObject {
Q_OBJECT

private slots:
    void addPreset_AddsGivenPreset_data();
    void addPreset_AddsGivenPreset();

    void removePreset_WorksAsExpected_data();
    void removePreset_WorksAsExpected();

    void removePresetsFromGroup_WorksAsExpected_data();
    void removePresetsFromGroup_WorksAsExpected();

    void containsPreset_ReturnsExpectedValue();

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

    void setCustomVoiLut_WorksAsExpected_data();
    void setCustomVoiLut_WorksAsExpected();

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
    VoiLutPresetsToolData* getWindowLevelPresetsSample();
};

Q_DECLARE_METATYPE(WindowLevel)
Q_DECLARE_METATYPE(VoiLut)
Q_DECLARE_METATYPE(VoiLutPresetsToolData::GroupsLabel)

const QString test_VoiLutPresetsToolData::AutoPreset1Name("Preset 1 - Auto");
const QString test_VoiLutPresetsToolData::AutoPreset2Name("Preset 2 - Auto");
const QString test_VoiLutPresetsToolData::FilePreset1Name("Preset 1 - File");
const QString test_VoiLutPresetsToolData::FilePreset2Name("Preset 2 - File");
const QString test_VoiLutPresetsToolData::StandardPreset1Name("CT Lung - Standard");
const QString test_VoiLutPresetsToolData::StandardPreset2Name("CT Bone - Standard");
const QString test_VoiLutPresetsToolData::UserPreset1Name("Preset 1 - User");
const QString test_VoiLutPresetsToolData::CustomPreset1Name("Preset 1 - Custom");
const QString test_VoiLutPresetsToolData::OtherPreset1Name("Preset 1 - Other");
const QString test_VoiLutPresetsToolData::NonExistingPresetName("Dummy name");

const WindowLevel test_VoiLutPresetsToolData::AutoPreset1(128.0, 256.0, AutoPreset1Name);
const WindowLevel test_VoiLutPresetsToolData::AutoPreset2(1024.0, 512.0, AutoPreset2Name);
const WindowLevel test_VoiLutPresetsToolData::FilePreset1(328.5, 678.9, FilePreset1Name);
const WindowLevel test_VoiLutPresetsToolData::FilePreset2(128.3, 583.4, FilePreset2Name);
const WindowLevel test_VoiLutPresetsToolData::StandardPreset1(1500, -650, StandardPreset1Name);
const WindowLevel test_VoiLutPresetsToolData::StandardPreset2(2000, 500, StandardPreset2Name);
const WindowLevel test_VoiLutPresetsToolData::UserPreset1(110, 900, UserPreset1Name);
const WindowLevel test_VoiLutPresetsToolData::CustomPreset1(1231, 412, CustomPreset1Name);
const WindowLevel test_VoiLutPresetsToolData::OtherPreset1(100, 200, OtherPreset1Name);
const WindowLevel test_VoiLutPresetsToolData::NonExistingPreset(0.0, 0.0, NonExistingPresetName);

void test_VoiLutPresetsToolData::addPreset_AddsGivenPreset_data()
{
    QTest::addColumn<QString>("presetDescription");
    QTest::addColumn<WindowLevel>("windowLevel");
    QTest::addColumn<VoiLutPresetsToolData::GroupsLabel>("group");

    QTest::newRow("Preset 1") << "Preset name 1" << WindowLevel(0.0, 0.0, "Preset name 1") << static_cast<VoiLutPresetsToolData::GroupsLabel>(0);
    QTest::newRow("Preset 2") << "Preset name 2" << WindowLevel(128.0, 256.0, "Preset name 2") << static_cast<VoiLutPresetsToolData::GroupsLabel>(100);
    QTest::newRow("Preset 3 in 'FileDefined' group") << "Preset 3" << WindowLevel(1.0, 1.0, "Preset 3") << VoiLutPresetsToolData::FileDefined;
}

void test_VoiLutPresetsToolData::addPreset_AddsGivenPreset()
{
    QFETCH(QString, presetDescription);
    QFETCH(WindowLevel, windowLevel);
    QFETCH(VoiLutPresetsToolData::GroupsLabel, group);

    VoiLutPresetsToolData wlData;
    wlData.addPreset(windowLevel, group);

    QCOMPARE(wlData.getFromDescription(presetDescription), VoiLut(windowLevel));
}

void test_VoiLutPresetsToolData::removePreset_WorksAsExpected_data()
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

void test_VoiLutPresetsToolData::removePreset_WorksAsExpected()
{
    QFETCH(WindowLevel, presetToRemove);

    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->removePreset(presetToRemove);

    QVERIFY(!wlData->containsPreset(presetToRemove.getName()));

    delete wlData;
}

void test_VoiLutPresetsToolData::removePresetsFromGroup_WorksAsExpected_data()
{
    QTest::addColumn<VoiLutPresetsToolData::GroupsLabel>("groupToDelete");

    QTest::newRow("Remove 'auto' presets") << VoiLutPresetsToolData::AutomaticPreset;
    QTest::newRow("Remove 'file defined' presets") << VoiLutPresetsToolData::FileDefined;
    QTest::newRow("Remove 'standard' presets") << VoiLutPresetsToolData::StandardPresets;
    QTest::newRow("Remove 'user defined' presets") << VoiLutPresetsToolData::UserDefined;
    QTest::newRow("Remove 'custom' presets") << VoiLutPresetsToolData::CustomPreset;
    QTest::newRow("Remove 'other' presets") << VoiLutPresetsToolData::Other;
    QTest::newRow("Remove non-existing presets group") << static_cast<VoiLutPresetsToolData::GroupsLabel>(-1);
}

void test_VoiLutPresetsToolData::removePresetsFromGroup_WorksAsExpected()
{
    QFETCH(VoiLutPresetsToolData::GroupsLabel, groupToDelete);

    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->removePresetsFromGroup(groupToDelete);

    QVERIFY(wlData->getDescriptionsFromGroup(groupToDelete).isEmpty());

    delete wlData;
}

void test_VoiLutPresetsToolData::containsPreset_ReturnsExpectedValue()
{
    VoiLut voiLut;
    voiLut.setExplanation("foo");
    VoiLutPresetsToolData voiLutData;
    QVERIFY(!voiLutData.containsPreset(voiLut.getExplanation()));
    voiLutData.addPreset(voiLut);
    QVERIFY(voiLutData.containsPreset(voiLut.getExplanation()));
    voiLutData.removePreset(voiLut);
    QVERIFY(!voiLutData.containsPreset(voiLut.getExplanation()));
}

void test_VoiLutPresetsToolData::getFromDescription_ReturnsExpectedValues_data()
{
    QTest::addColumn<QString>("presetName");
    QTest::addColumn<WindowLevel>("preset");

    QTest::newRow("Existing auto preset") << AutoPreset2Name << AutoPreset2;
    QTest::newRow("Existing file preset") << FilePreset1Name << FilePreset1;
    QTest::newRow("Existing standard preset") << StandardPreset1Name << StandardPreset1;
    QTest::newRow("Existing user preset") << UserPreset1Name << UserPreset1;
    QTest::newRow("Existing custom preset") << CustomPreset1Name << CustomPreset1;
    QTest::newRow("Existing other preset") << OtherPreset1Name << OtherPreset1;
    QTest::newRow("NON-Existing preset") << NonExistingPresetName << WindowLevel();
}

void test_VoiLutPresetsToolData::getFromDescription_ReturnsExpectedValues()
{
    QFETCH(QString, presetName);
    QFETCH(WindowLevel, preset);

    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();
    
    QCOMPARE(wlData->getFromDescription(presetName), VoiLut(preset));

    delete wlData;
}

void test_VoiLutPresetsToolData::getGroup_ReturnsExpectedValuesFromExistingPresets_data()
{
    QTest::addColumn<WindowLevel>("preset");
    QTest::addColumn<VoiLutPresetsToolData::GroupsLabel>("expectedGroup");

    QTest::newRow("Auto preset") << AutoPreset1 << VoiLutPresetsToolData::AutomaticPreset;
    QTest::newRow("File preset") << FilePreset1 << VoiLutPresetsToolData::FileDefined;
    QTest::newRow("Standard preset") << StandardPreset1 << VoiLutPresetsToolData::StandardPresets;
    QTest::newRow("Custom preset") << CustomPreset1 << VoiLutPresetsToolData::CustomPreset;
    QTest::newRow("User preset") << UserPreset1 << VoiLutPresetsToolData::UserDefined;
    QTest::newRow("Other preset") << OtherPreset1 << VoiLutPresetsToolData::Other;
}

void test_VoiLutPresetsToolData::getGroup_ReturnsExpectedValuesFromExistingPresets()
{
    QFETCH(WindowLevel, preset);
    QFETCH(VoiLutPresetsToolData::GroupsLabel, expectedGroup);

    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();

    QCOMPARE(wlData->getGroup(preset.getName()), expectedGroup);

    delete wlData;
}

void test_VoiLutPresetsToolData::getGroup_ReturnsExpectedValuesFromNonExistingPresets_data()
{
    QTest::addColumn<WindowLevel>("preset");

    QTest::newRow("Non-existing preset 0") << NonExistingPreset;
    QTest::newRow("Non-existing preset 1") << WindowLevel(MathTools::randomDouble(1, 6000), MathTools::randomDouble(-6000, 6000), "non-existing preset");
    QTest::newRow("Non-existing preset 2") << WindowLevel(MathTools::randomDouble(1, 6000), MathTools::randomDouble(-6000, 6000), "ABDCJS-##@|@@##-123456");
    QTest::newRow("Non-existing preset 3") << WindowLevel(MathTools::randomDouble(1, 6000), MathTools::randomDouble(-6000, 6000), "[" + AutoPreset1Name + "]");
}

void test_VoiLutPresetsToolData::getGroup_ReturnsExpectedValuesFromNonExistingPresets()
{
    QFETCH(WindowLevel, preset);
    
    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();

    QCOMPARE(wlData->getGroup(preset.getName()), VoiLutPresetsToolData::AutomaticPreset);

    delete wlData;
}

void test_VoiLutPresetsToolData::getDescriptionsFromGroup_ReturnsExpectedValues_data()
{
    QTest::addColumn<VoiLutPresetsToolData::GroupsLabel>("group");
    QTest::addColumn<QStringList>("expectedPresetsNames");

    QStringList presetList;

    presetList << AutoPreset1Name << AutoPreset2Name;
    QTest::newRow("Auto presets") << VoiLutPresetsToolData::AutomaticPreset << presetList;
    
    presetList.clear();
    presetList << FilePreset1Name << FilePreset2Name;
    QTest::newRow("File presets") << VoiLutPresetsToolData::FileDefined << presetList;
    
    presetList.clear();
    presetList << StandardPreset1Name << StandardPreset2Name;
    QTest::newRow("Standard presets") << VoiLutPresetsToolData::StandardPresets << presetList;
    
    presetList.clear();
    presetList << CustomPreset1Name;
    QTest::newRow("Custom presets") << VoiLutPresetsToolData::CustomPreset << presetList;
    
    presetList.clear();
    presetList << UserPreset1Name;
    QTest::newRow("User presets") << VoiLutPresetsToolData::UserDefined << presetList;
    
    presetList.clear();
    presetList << OtherPreset1Name;
    QTest::newRow("Other presets") << VoiLutPresetsToolData::Other << presetList;
}

void test_VoiLutPresetsToolData::getDescriptionsFromGroup_ReturnsExpectedValues()
{
    QFETCH(VoiLutPresetsToolData::GroupsLabel, group);
    QFETCH(QStringList, expectedPresetsNames);

    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();

    QStringList namesList = wlData->getDescriptionsFromGroup(group);
    QCOMPARE(namesList.count(), expectedPresetsNames.count());
    
    foreach (const QString &presetName, expectedPresetsNames)
    {
        QVERIFY(namesList.contains(presetName));
    }

    delete wlData;
}

void test_VoiLutPresetsToolData::getCurrentPreset_IsEmptyAfterCreation()
{
    VoiLutPresetsToolData wlData;

    QCOMPARE(wlData.getCurrentPreset(), VoiLut());
}

void test_VoiLutPresetsToolData::getCurrentPreset_ReturnsExpectedPresetViaActivatePreset_data()
{
    QTest::addColumn<QString>("presetNameToActivate");
    QTest::addColumn<QString>("expectedCurrentPresetName");

    QTest::newRow("Existing activated preset") << StandardPreset1Name << StandardPreset1Name;
    QTest::newRow("Non-existing activated preset") << NonExistingPresetName << QString();
}

void test_VoiLutPresetsToolData::getCurrentPreset_ReturnsExpectedPresetViaActivatePreset()
{
    QFETCH(QString, presetNameToActivate);
    QFETCH(QString, expectedCurrentPresetName);

    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->selectPreset(presetNameToActivate);
    
    QCOMPARE(wlData->getCurrentPreset().getExplanation(), expectedCurrentPresetName);

    delete wlData;
}

void test_VoiLutPresetsToolData::getCurrentPreset_ReturnsExpectedPresetViaSetCustomWindowLevel()
{
    VoiLutPresetsToolData wlData;
    wlData.setCustomVoiLut(WindowLevel(1024.0, 512.0));
    VoiLut expectedPreset(WindowLevel(1024.0, 512.0));
    expectedPreset.setExplanation(VoiLutPresetsToolData::getCustomPresetName());

    QCOMPARE(wlData.getCurrentPreset(), expectedPreset);
}

void test_VoiLutPresetsToolData::updatePreset_WorksAsExpected_data()
{
    QTest::addColumn<WindowLevel>("windowLevelToUpdate");
    QTest::addColumn<WindowLevel>("windowLevelAfterUpdate");

    QTest::newRow("Activated preset exists and is file defined") << WindowLevel(1.5, 2.5, FilePreset1Name) << WindowLevel(1.5, 2.5, FilePreset1Name);
    QTest::newRow("Activated preset exists but is not file defined") << WindowLevel(128.0, 256.0, AutoPreset1Name) << WindowLevel(128.0, 256.0, AutoPreset1Name);
    QTest::newRow("Activated preset does not exist") << WindowLevel(1.5, 2.5, NonExistingPresetName) << WindowLevel();
}

void test_VoiLutPresetsToolData::updatePreset_WorksAsExpected()
{
    QFETCH(WindowLevel, windowLevelToUpdate);
    QFETCH(WindowLevel, windowLevelAfterUpdate);
    
    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();
    wlData->updatePreset(windowLevelToUpdate);
    
    QCOMPARE(wlData->getFromDescription(windowLevelToUpdate.getName()), VoiLut(windowLevelAfterUpdate));
    
    delete wlData;
}

void test_VoiLutPresetsToolData::setCustomVoiLut_WorksAsExpected_data()
{
    QTest::addColumn<VoiLut>("customVoiLut");

    QTest::newRow("default voi lut") << VoiLut();
    QTest::newRow("window level") << VoiLut(WindowLevel(35, 11, "wl"));
    TransferFunction transferFunction;
    transferFunction.setName("tf");
    transferFunction.setColor(0.0, 0.0, 0.0, 0.0);
    transferFunction.setColor(50.0, 0.2, 0.2, 0.2);
    transferFunction.setColor(80.0, 0.5, 0.5, 0.5);
    transferFunction.setColor(200.0, 1.0, 1.0, 1.0);
    transferFunction.setOpacity(0.0, 1.0);
    QTest::newRow("transfer function") << VoiLut(transferFunction);
}

void test_VoiLutPresetsToolData::setCustomVoiLut_WorksAsExpected()
{
    QFETCH(VoiLut, customVoiLut);

    VoiLutPresetsToolData voiLutData;
    voiLutData.setCustomVoiLut(customVoiLut);
    VoiLut expectedVoiLut = customVoiLut;
    expectedVoiLut.setExplanation(VoiLutPresetsToolData::getCustomPresetName());

    QCOMPARE(voiLutData.getCurrentPreset(), expectedVoiLut);
    QCOMPARE(voiLutData.getGroup(voiLutData.getCurrentPresetName()), VoiLutPresetsToolData::CustomPreset);
}

void test_VoiLutPresetsToolData::activatePreset_WorksAsExpected_data()
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

void test_VoiLutPresetsToolData::activatePreset_WorksAsExpected()
{
    QFETCH(QString, presetNameToActivate);
    QFETCH(QString, currentActivatedPreset);

    VoiLutPresetsToolData* wlData = getWindowLevelPresetsSample();

    wlData->selectPreset(presetNameToActivate);
    QCOMPARE(wlData->getCurrentPreset().getExplanation(), currentActivatedPreset);
    
    delete wlData;
}

VoiLutPresetsToolData* test_VoiLutPresetsToolData::getWindowLevelPresetsSample()
{
    VoiLutPresetsToolData* wlData = new VoiLutPresetsToolData();
    // Eliminem els presets que es posen per defecte al construir l'objecte de cares a tenir més ben controlat el testing del que aquí posem
    wlData->removePresetsFromGroup(VoiLutPresetsToolData::StandardPresets);
    wlData->removePresetsFromGroup(VoiLutPresetsToolData::CustomPreset);
    wlData->addPreset(AutoPreset1, VoiLutPresetsToolData::AutomaticPreset);
    wlData->addPreset(AutoPreset2, VoiLutPresetsToolData::AutomaticPreset);
    wlData->addPreset(FilePreset1, VoiLutPresetsToolData::FileDefined);
    wlData->addPreset(FilePreset2, VoiLutPresetsToolData::FileDefined);
    wlData->addPreset(StandardPreset1, VoiLutPresetsToolData::StandardPresets);
    wlData->addPreset(StandardPreset2, VoiLutPresetsToolData::StandardPresets);
    wlData->addPreset(UserPreset1, VoiLutPresetsToolData::UserDefined);
    wlData->addPreset(CustomPreset1, VoiLutPresetsToolData::CustomPreset);
    wlData->addPreset(OtherPreset1, VoiLutPresetsToolData::Other);

    return wlData;
}

DECLARE_TEST(test_VoiLutPresetsToolData)

#include "test_voilutpresetstooldata.moc"
