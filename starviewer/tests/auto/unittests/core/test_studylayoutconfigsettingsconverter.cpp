#include "autotest.h"

#include "studylayoutconfigsettingsconverter.h"

using namespace udg;

class test_StudyLayoutConfigSettingsConverter : public QObject {
Q_OBJECT

private slots:
    void fromSettingsListItem_ReturnExpectedValues_data();
    void fromSettingsListItem_ReturnExpectedValues();
    
    void toSettingsListItem_ReturnExpectedValues_data();
    void toSettingsListItem_ReturnExpectedValues();
};

Q_DECLARE_METATYPE(StudyLayoutConfig)
Q_DECLARE_METATYPE(Settings::SettingsListItemType)

void test_StudyLayoutConfigSettingsConverter::fromSettingsListItem_ReturnExpectedValues_data()
{
    QTest::addColumn<Settings::SettingsListItemType>("input");
    QTest::addColumn<StudyLayoutConfig>("expectedValue");
    
    Settings::SettingsListItemType item;
    item["Modality"] = "MR";
    item["ExclusionCriteria"] = "Survey";
    item["MaxNumberOfViewers"] = 22;
    item["Direction"] = "LeftToRight";
    item["UnfoldBy"] = "Series";

    StudyLayoutConfig config("MR", StudyLayoutConfig::UnfoldSeries, StudyLayoutConfig::LeftToRightFirst);
    config.addExclusionCriteria(StudyLayoutConfig::Survey);
    config.setMaximumNumberOfViewers(22);

    QTest::newRow("Full filled item") << item << config;

    item.clear();
    QTest::newRow("Empty Item") << item << StudyLayoutConfig();

    item.clear();
    item["Modality"] = "MR";
    item["Non-existing field#1"] = "Survey";
    item["UnfolderBy"] = "Series";

    QTest::newRow("item with non-existing, mispelled and missing fields") << item << StudyLayoutConfig("MR");

    item.clear();
    item["Modality"] = "MR";
    item["ExclusionCriteria"] = "Surveyloranciancer;Localizator";
    item["MaxNumberOfViewers"] = "Twenty";
    item["Direction"] = "RightFirstThenUp";
    item["UnfoldBy"] = "DICOMImages";

    QTest::newRow("item with invalid values, leds to default values in conversion") << item << StudyLayoutConfig("MR");

}

void test_StudyLayoutConfigSettingsConverter::fromSettingsListItem_ReturnExpectedValues()
{
    QFETCH(Settings::SettingsListItemType, input);
    QFETCH(StudyLayoutConfig, expectedValue);
    
    StudyLayoutConfigSettingsConverter converter;
    QCOMPARE(converter.fromSettingsListItem(input), expectedValue);
}

void test_StudyLayoutConfigSettingsConverter::toSettingsListItem_ReturnExpectedValues_data()
{
    QTest::addColumn<StudyLayoutConfig>("input");
    QTest::addColumn<Settings::SettingsListItemType>("expectedValue");
    
    Settings::SettingsListItemType item;
    item["Modality"] = "";
    item["ExclusionCriteria"] = "";
    item["MaxNumberOfViewers"] = 0;
    item["Direction"] = "LeftToRight";
    item["UnfoldBy"] = "Series";

    QTest::newRow("Empty config") << StudyLayoutConfig() << item;

    StudyLayoutConfig config("CR", StudyLayoutConfig::UnfoldImages, StudyLayoutConfig::TopToBottomFirst);
    config.setMaximumNumberOfViewers(31);

    item.clear();
    item["Modality"] = "CR";
    item["ExclusionCriteria"] = "";
    item["MaxNumberOfViewers"] = 31;
    item["Direction"] = "TopToBottom";
    item["UnfoldBy"] = "Images";

    QTest::newRow("Some sample config with no default values") << config << item;

    config = StudyLayoutConfig("DX", StudyLayoutConfig::UnfoldImages, StudyLayoutConfig::TopToBottomFirst);
    config.addExclusionCriteria(StudyLayoutConfig::Survey);
    config.addExclusionCriteria(StudyLayoutConfig::Localizer);
    config.setMaximumNumberOfViewers(11);

    item.clear();
    item["Modality"] = "DX";
    item["ExclusionCriteria"] = "Survey;Localizer";
    item["MaxNumberOfViewers"] = 11;
    item["Direction"] = "TopToBottom";
    item["UnfoldBy"] = "Images";

    QTest::newRow("Some sample config with no default values and exclusion criteria") << config << item;
}

void test_StudyLayoutConfigSettingsConverter::toSettingsListItem_ReturnExpectedValues()
{
    QFETCH(StudyLayoutConfig, input);
    QFETCH(Settings::SettingsListItemType, expectedValue);

    StudyLayoutConfigSettingsConverter converter;
    QCOMPARE(converter.toSettingsListItem(input), expectedValue);
}

DECLARE_TEST(test_StudyLayoutConfigSettingsConverter)

#include "test_studylayoutconfigsettingsconverter.moc"
