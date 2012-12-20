#include "studylayoutconfigsettingsconverter.h"

#include <QStringList>

namespace udg {

const QString StudyLayoutConfigSettingsConverter::ModalityKey("Modality");
const QString StudyLayoutConfigSettingsConverter::MaxNumberOfViewersKey("MaxNumberOfViewers");
const QString StudyLayoutConfigSettingsConverter::ExclusionCriteriaKey("ExclusionCriteria");
const QString StudyLayoutConfigSettingsConverter::UnfoldByKey("UnfoldBy");
const QString StudyLayoutConfigSettingsConverter::UnfoldDirectionKey("Direction");
const QString StudyLayoutConfigSettingsConverter::StringListSeparator(";");
const QString StudyLayoutConfigSettingsConverter::Survey("Survey");
const QString StudyLayoutConfigSettingsConverter::Localizer("Localizer");
const QString StudyLayoutConfigSettingsConverter::LeftToRight("LeftToRight");
const QString StudyLayoutConfigSettingsConverter::TopToBottom("TopToBottom");
const QString StudyLayoutConfigSettingsConverter::Images("Images");
const QString StudyLayoutConfigSettingsConverter::Series("Series");

StudyLayoutConfigSettingsConverter::StudyLayoutConfigSettingsConverter()
{
}

StudyLayoutConfigSettingsConverter::~StudyLayoutConfigSettingsConverter()
{
}

StudyLayoutConfig StudyLayoutConfigSettingsConverter::fromSettingsListItem(const Settings::SettingsListItemType &item) const
{
    StudyLayoutConfig config;
    
    config.setMaximumNumberOfViewers(item.value(MaxNumberOfViewersKey).toInt());
    config.setUnfoldDirection(getUnfoldDirectionFromSettingsValue(item.value(UnfoldDirectionKey).toString()));
    config.setUnfoldType(getUnfoldTypeFromSettingsValue(item.value(UnfoldByKey).toString()));
    foreach (StudyLayoutConfig::ExclusionCriteriaType criteria, getExclusionCriteriaFromSettingsValue(item.value(ExclusionCriteriaKey).toString()))
    {
        config.addExclusionCriteria(criteria);
    }
    config.setModality(item.value(ModalityKey).toString());

    return config;
}

Settings::SettingsListItemType StudyLayoutConfigSettingsConverter::toSettingsListItem(const StudyLayoutConfig &config) const
{
    Settings::SettingsListItemType item;

    QStringList exclusionCriteria;
    foreach (StudyLayoutConfig::ExclusionCriteriaType criteria, config.getExclusionCriteria())
    {
        exclusionCriteria << getAsSettingsStringValue(criteria);
    }
    exclusionCriteria.removeDuplicates();
    exclusionCriteria.removeAll(QString());
    
    QString direction = getAsSettingsStringValue(config.getUnfoldDirection());
    
    QString unfoldBy = getAsSettingsStringValue(config.getUnfoldType());

    item[ModalityKey] = config.getModality();
    item[ExclusionCriteriaKey] = exclusionCriteria.join(StringListSeparator);
    item[MaxNumberOfViewersKey] = QString::number(config.getMaximumNumberOfViewers());
    item[UnfoldDirectionKey] = direction;
    item[UnfoldByKey] = unfoldBy;

    return item;
}

QString StudyLayoutConfigSettingsConverter::getAsSettingsStringValue(const StudyLayoutConfig::ExclusionCriteriaType &criteria) const
{
    QString criteriaString;
    switch (criteria)
    {
        case StudyLayoutConfig::Localizer:
            criteriaString = Localizer;
            break;

        case StudyLayoutConfig::Survey:
            criteriaString = Survey;
            break;
    }

    return criteriaString;
}

QString StudyLayoutConfigSettingsConverter::getAsSettingsStringValue(const StudyLayoutConfig::UnfoldDirectionType &direction) const
{
    QString directionString;
    switch (direction)
    {
        case StudyLayoutConfig::LeftToRightFirst:
            directionString = LeftToRight;
            break;

        case StudyLayoutConfig::TopToBottomFirst:
            directionString = TopToBottom;
            break;
    }

    return directionString;
}

QString StudyLayoutConfigSettingsConverter::getAsSettingsStringValue(const StudyLayoutConfig::UnfoldType &unfoldBy) const
{
    QString unfoldByString;
    switch (unfoldBy)
    {
        case StudyLayoutConfig::UnfoldImages:
            unfoldByString = Images;
            break;

        case StudyLayoutConfig::UnfoldSeries:
            unfoldByString = Series;
            break;
    }

    return unfoldByString;
}

QList<StudyLayoutConfig::ExclusionCriteriaType> StudyLayoutConfigSettingsConverter::getExclusionCriteriaFromSettingsValue(const QString &value) const
{
    QStringList criteriaStringList = value.split(StringListSeparator, QString::SkipEmptyParts);
    QList<StudyLayoutConfig::ExclusionCriteriaType> criteriaList;
    foreach (const QString &string, criteriaStringList)
    {
        if (string == Localizer)
        {
            criteriaList << StudyLayoutConfig::Localizer;
        }
        else if (string == Survey)
        {
            criteriaList << StudyLayoutConfig::Survey;
        }
    }

    return criteriaList;
}

StudyLayoutConfig::UnfoldDirectionType StudyLayoutConfigSettingsConverter::getUnfoldDirectionFromSettingsValue(const QString &value) const
{
    if (value == LeftToRight)
    {
        return StudyLayoutConfig::LeftToRightFirst;
    }

    if (value == TopToBottom)
    {
        return StudyLayoutConfig::TopToBottomFirst;
    }

    // Si no hi ha cap valor vàlid, retornem el valor per defecte
    return StudyLayoutConfig::DefaultUnfoldDirection;
}

StudyLayoutConfig::UnfoldType StudyLayoutConfigSettingsConverter::getUnfoldTypeFromSettingsValue(const QString &value) const
{
    if (value == Images)
    {
        return StudyLayoutConfig::UnfoldImages;
    }

    if (value == Series)
    {
        return StudyLayoutConfig::UnfoldSeries;
    }

    // Si no hi ha cap valor vàlid, retornem el valor per defecte
    return StudyLayoutConfig::DefaultUnfoldType;
}

}
