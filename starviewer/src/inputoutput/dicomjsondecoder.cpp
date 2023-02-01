/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "dicomjsondecoder.h"

#include "dicomdictionary.h"
#include "image.h"
#include "patient.h"
#include "series.h"
#include "study.h"

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QRegularExpression>

namespace udg {

// The key must be "The eight character uppercase hexadecimal representation of a DICOM Tag" [PS3.18§F.2.2]
static const QRegularExpression KeyRegularExpression("^[0-9A-F]{8}$");

void DicomJsonDecoder::setPacsDevice(const PacsDevice &pacs, bool overrideInstitutionName)
{
    if (overrideInstitutionName)
    {
        m_institutionName = pacs.getInstitution();
    }

    m_dicomSource.addRetrievePACS(pacs);
}

const QStringList& DicomJsonDecoder::getErrors() const
{
    return m_errors;
}

QList<Patient*> DicomJsonDecoder::getStudies(const QByteArray &jsonText)
{
    const QJsonArray &studiesJsonArray = getRootArray(jsonText);    // in case of error will return an empty array, thus the for loop works
    QList<Patient*> studies;

    for (int i = 0; i < studiesJsonArray.size(); i++)
    {
        Patient *patientStudy = getStudy(studiesJsonArray[i], QString("Root array, index %1").arg(i));

        if (patientStudy)
        {
            studies.append(patientStudy);
        }
    }

    return studies;
}

QList<Series*> DicomJsonDecoder::getSeries(const QByteArray &jsonText)
{
    const QJsonArray &seriesJsonArray = getRootArray(jsonText);     // in case of error will return an empty array, thus the for loop works
    QList<Series*> seriesList;

    for (int i = 0; i < seriesJsonArray.size(); i++)
    {
        Series *series = getSeries(seriesJsonArray[i], QString("Root array, index %1").arg(i));

        if (series)
        {
            seriesList.append(series);
        }
    }

    return seriesList;
}

QList<Image*> DicomJsonDecoder::getInstances(const QByteArray &jsonText)
{
    const QJsonArray &instancesJsonArray = getRootArray(jsonText);  // in case of error will return an empty array, thus the for loop works
    QList<Image*> instances;

    for (int i = 0; i < instancesJsonArray.size(); i++)
    {
        Image *instance = getInstance(instancesJsonArray[i], QString("Root array, index %1").arg(i));

        if (instance)
        {
            instances.append(instance);
        }
    }

    return instances;
}

QJsonArray DicomJsonDecoder::getRootArray(const QByteArray &jsonText)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonText, &jsonError);

    if (jsonDocument.isNull())
    {
        m_errors.append(jsonError.errorString());
        return {};
    }

    if (!jsonDocument.isArray())
    {
        m_errors.append("JSON document is not an array.");
        return {};
    }

    return jsonDocument.array();
}

Patient* DicomJsonDecoder::getStudy(const QJsonValue &jsonValue, const QString &errorContext)
{
    if (!jsonValue.isObject())
    {
        m_errors.append(QString("%1: not an object.").arg(errorContext));
        return nullptr;
    }

    const QJsonObject &studyJsonObject = jsonValue.toObject();
    Patient *patient = new Patient();
    Study *study = new Study();

    for (const QString &key : studyJsonObject.keys())
    {
        if (!KeyRegularExpression.match(key).hasMatch())
        {
            m_errors.append(QString("%1: invalid key format: \"%2\".").arg(errorContext).arg(key));
            continue;
        }

        uint group = key.left(4).toUInt(nullptr, 16);
        uint element = key.right(4).toUInt(nullptr, 16);
        DICOMTag tag(group, element);
        QString valueErrorContext = QString("%1, key %2").arg(errorContext).arg(key);

        if (tag == DICOMPatientName)
        {
            patient->setFullName(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMPatientID)
        {
            patient->setID(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMPatientBirthDate)
        {
            patient->setBirthDate(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMPatientAge)
        {
            study->setPatientAge(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMStudyInstanceUID)
        {
            study->setInstanceUID(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMStudyID)
        {
            study->setID(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMStudyDate)
        {
            study->setDate(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMStudyTime)
        {
            study->setTime(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMStudyDescription)
        {
            study->setDescription(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMModalitiesInStudy)
        {
            const QStringList &modalities = getTagValueAsStringList(studyJsonObject[key], valueErrorContext);

            for (const QString &modality : modalities)
            {
                study->addModality(modality);
            }
        }
        else if (tag == DICOMAccessionNumber)
        {
            study->setAccessionNumber(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMReferringPhysicianName)
        {
            study->setReferringPhysiciansName(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMInstitutionName)
        {
            study->setInstitutionName(getTagValueAsString(studyJsonObject[key], valueErrorContext));
        }
    }

    if (!m_institutionName.isNull())
    {
        study->setInstitutionName(m_institutionName);
    }

    study->setDICOMSource(m_dicomSource);

    if (!patient->addStudy(study))
    {
        m_errors.append(QString("%1: study has not been added to the patient.").arg(errorContext));
        delete study;
    }

    return patient;
}

Series* DicomJsonDecoder::getSeries(const QJsonValue &jsonValue, const QString &errorContext)
{
    if (!jsonValue.isObject())
    {
        m_errors.append(QString("%1: not an object.").arg(errorContext));
        return nullptr;
    }

    const QJsonObject &seriesJsonObject = jsonValue.toObject();
    Series *series = new Series();

    for (const QString &key : seriesJsonObject.keys())
    {
        if (!KeyRegularExpression.match(key).hasMatch())
        {
            m_errors.append(QString("%1: invalid key format: \"%2\".").arg(errorContext).arg(key));
            continue;
        }

        uint group = key.left(4).toUInt(nullptr, 16);
        uint element = key.right(4).toUInt(nullptr, 16);
        DICOMTag tag(group, element);
        QString valueErrorContext = QString("%1, key %2").arg(errorContext).arg(key);

        if (tag == DICOMSeriesInstanceUID)
        {
            series->setInstanceUID(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMSeriesNumber)
        {
            series->setSeriesNumber(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMSeriesDescription)
        {
            series->setDescription(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMProtocolName)
        {
            series->setProtocolName(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMSeriesDate)
        {
            series->setDate(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMSeriesTime)
        {
            series->setTime(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMModality)
        {
            series->setModality(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMPerformedProcedureStepStartDate)
        {
            series->setPerformedProcedureStepStartDate(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMPerformedProcedureStepStartTime)
        {
            series->setPerformedProcedureStepStartTime(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMRequestedProcedureID)
        {
            series->setRequestedProcedureID(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMScheduledProcedureStepID)
        {
            series->setScheduledProcedureStepID(getTagValueAsString(seriesJsonObject[key], valueErrorContext));
        }
    }

    series->setDICOMSource(m_dicomSource);

    return series;
}

Image* DicomJsonDecoder::getInstance(const QJsonValue &jsonValue, const QString &errorContext)
{
    if (!jsonValue.isObject())
    {
        m_errors.append(QString("%1: not an object.").arg(errorContext));
        return nullptr;
    }

    const QJsonObject &instanceJsonObject = jsonValue.toObject();
    Image *instance = new Image();

    for (const QString &key : instanceJsonObject.keys())
    {
        if (!KeyRegularExpression.match(key).hasMatch())
        {
            m_errors.append(QString("%1: invalid key format: \"%2\".").arg(errorContext).arg(key));
            continue;
        }

        uint group = key.left(4).toUInt(nullptr, 16);
        uint element = key.right(4).toUInt(nullptr, 16);
        DICOMTag tag(group, element);
        QString valueErrorContext = QString("%1, key %2").arg(errorContext).arg(key);

        if (tag == DICOMSOPInstanceUID)
        {
            instance->setSOPInstanceUID(getTagValueAsString(instanceJsonObject[key], valueErrorContext));
        }
        else if (tag == DICOMInstanceNumber)
        {
            instance->setInstanceNumber(getTagValueAsString(instanceJsonObject[key], valueErrorContext));
        }
    }

    instance->setDICOMSource(m_dicomSource);

    return instance;
}

QString DicomJsonDecoder::getTagValueAsString(const QJsonValue &jsonValue, const QString &errorContext)
{
    const JsonDataType expectedType = getExpectedType(jsonValue, errorContext);

    if (expectedType == JsonDataType::Undefined)
    {
        return {};
    }

    const QJsonArray &valueArray = getValueArray(jsonValue, errorContext);

    if (valueArray.isEmpty())
    {
        return {};
    }

    if (valueArray.size() > 1)
    {
        m_errors.append(QString("%1: Value array has more than one element.").arg(errorContext));
    }

    const QStringList &values = getArrayAsStringList(valueArray, expectedType, QString("%1, Value array").arg(errorContext));

    return values.first();
}

QStringList DicomJsonDecoder::getTagValueAsStringList(const QJsonValue &jsonValue, const QString &errorContext)
{
    const JsonDataType expectedType = getExpectedType(jsonValue, errorContext);

    if (expectedType == JsonDataType::Undefined)
    {
        return {};
    }

    const QJsonArray &valueArray = getValueArray(jsonValue, errorContext);

    if (valueArray.isEmpty())
    {
        return {};
    }

    return getArrayAsStringList(valueArray, expectedType, QString("%1, Value array").arg(errorContext));
}

DicomJsonDecoder::JsonDataType DicomJsonDecoder::getExpectedType(const QJsonValue &jsonValue, const QString &errorContext)
{
    /*
     * Expected structure of jsonValue:
     * {
     *     "vr": "CS",
     *     "Value": [...]
     * }
     */

    static const QString VrKey("vr");

    if (!jsonValue.isObject())
    {
        m_errors.append(QString("%1: value is not an object.").arg(errorContext));
        return JsonDataType::Undefined;
    }

    const QJsonObject &jsonObject = jsonValue.toObject();

    if (!jsonObject.contains(VrKey))
    {
        m_errors.append(QString("%1: value does not specify VR.").arg(errorContext));
        return JsonDataType::Undefined;
    }

    const QJsonValue &vrJsonValue = jsonObject[VrKey];

    if (!vrJsonValue.isString())
    {
        m_errors.append(QString("%1, key %2: value is not a string.").arg(errorContext).arg(VrKey));
        return JsonDataType::Undefined;
    }

    const QString &vrString = vrJsonValue.toString();

    static const QHash<QString, JsonDataType> VrMap{
        {"AE", JsonDataType::String},           // Application Entity
        {"AS", JsonDataType::String},           // Age String
        {"AT", JsonDataType::String},           // Attribute Tag
        {"CS", JsonDataType::String},           // Code String
        {"DA", JsonDataType::String},           // Date
        {"DS", JsonDataType::DoubleOrString},   // Decimal String (rational number represented as a string)
        {"DT", JsonDataType::String},           // Date Time
        {"FL", JsonDataType::Double},           // Floating Point Single (float)
        {"FD", JsonDataType::Double},           // Floating Point Double (double)
        {"IS", JsonDataType::IntegerOrString},  // Integer String (int32_t represented as a string)
        {"LO", JsonDataType::String},           // Long String
        {"LT", JsonDataType::String},           // Long Text
        {"PN", JsonDataType::PersonNameObject}, // Person Name
        {"SH", JsonDataType::String},           // Short String
        {"SL", JsonDataType::Integer},          // Signed Long (int32_t)
        {"SQ", JsonDataType::Array},            // Sequence of Items
        {"SS", JsonDataType::Integer},          // Signed Short (int16_t)
        {"ST", JsonDataType::String},           // Short Text
        {"SV", JsonDataType::DoubleOrString},   // Signed 64-bit Very Long (int64_t)
        {"TM", JsonDataType::String},           // Time
        {"UC", JsonDataType::String},           // Unlimited Characters
        {"UI", JsonDataType::String},           // Unique Identifier (UID)
        {"UL", JsonDataType::Double},           // Unsigned Long (uint32_t)
        {"UR", JsonDataType::String},           // Universal Resource Identifier or Universal Resource Locator (URI/URL)
        {"US", JsonDataType::Integer},          // Unsigned Short (uint16_t)
        {"UT", JsonDataType::String},           // Unlimited Text
        {"UV", JsonDataType::DoubleOrString},   // Unsigned 64-bit Very Long (uint64_t)
        // The following VRs don't store the value under the key "Value" but are included for completeness sake
        {"OB", JsonDataType::Base64String},     // Other Byte
        {"OD", JsonDataType::Base64String},     // Other Double
        {"OF", JsonDataType::Base64String},     // Other Float
        {"OL", JsonDataType::Base64String},     // Other Long
        {"OV", JsonDataType::Base64String},     // Other 64-bit Very Long
        {"OW", JsonDataType::Base64String},     // Other Word
        {"UN", JsonDataType::Base64String}      // Unknown
    };

    if (!VrMap.contains(vrString))
    {
        m_errors.append(QString("%1, key %2: unsupported VR: %3.").arg(errorContext).arg(VrKey).arg(vrString));
        return JsonDataType::Undefined;
    }

    return VrMap[vrString];
}

QJsonArray DicomJsonDecoder::getValueArray(const QJsonValue &jsonValue, const QString &errorContext)
{
    /*
     * Expected structure of jsonValue:
     * {
     *     "vr": "CS",
     *     "Value": [...]
     * }
     */

    static const QString ValueKey("Value");

    if (!jsonValue.isObject())
    {
        m_errors.append(QString("%1: value is not an object.").arg(errorContext));
        return {};
    }

    const QJsonObject &jsonObject = jsonValue.toObject();

    if (!jsonObject.contains(ValueKey))
    {
        // This is valid to represent an empty tag
        return {};
    }

    const QJsonValue &valueJsonValue = jsonObject[ValueKey];

    if (!valueJsonValue.isArray())
    {
        m_errors.append(QString("%1, key %2: value is not an array.").arg(errorContext).arg(ValueKey));
        return {};
    }

    const QJsonArray &valueJsonArray = valueJsonValue.toArray();

    if (valueJsonArray.size() == 0)
    {
        m_errors.append(QString("%1, key %2: array is empty.").arg(errorContext).arg(ValueKey));
    }

    return valueJsonArray;
}

QStringList DicomJsonDecoder::getArrayAsStringList(const QJsonArray &jsonArray, JsonDataType expectedType, const QString &errorContext)
{
    QStringList stringList;

    for (int i = 0; i < jsonArray.size(); i++)
    {
        const QJsonValue &jsonValue = jsonArray[i];
        QString valueErrorContext = QString("%1, index %2").arg(errorContext).arg(i);

        if (jsonValue.isNull()) // If a multi-valued attribute has one or more empty values these are represented as "null" array elements
        {
            stringList.append("null");
        }
        else if (expectedType == JsonDataType::String)
        {
            if (jsonValue.isString())
            {
                stringList.append(jsonValue.toString());
            }
            else
            {
                m_errors.append(QString("%1: not a string.").arg(valueErrorContext));
            }
        }
        else if (expectedType == JsonDataType::Integer)
        {
            if (jsonValue.isDouble())
            {
                stringList.append(QString::number(jsonValue.toInt()));
            }
            else
            {
                m_errors.append(QString("%1: not a number.").arg(valueErrorContext));
            }
        }
        else if (expectedType == JsonDataType::Double)
        {
            if (jsonValue.isDouble())
            {
                stringList.append(QString::number(jsonValue.toDouble()));
            }
            else
            {
                m_errors.append(QString("%1: not a number.").arg(valueErrorContext));
            }
        }
        else if (expectedType == JsonDataType::IntegerOrString)
        {
            if (jsonValue.isDouble())
            {
                stringList.append(QString::number(jsonValue.toInt()));
            }
            else if (jsonValue.isString())
            {
                stringList.append(jsonValue.toString());
            }
            else
            {
                m_errors.append(QString("%1: not a number nor string.").arg(valueErrorContext));
            }
        }
        else if (expectedType == JsonDataType::DoubleOrString)
        {
            if (jsonValue.isDouble())
            {
                stringList.append(QString::number(jsonValue.toDouble()));
            }
            else if (jsonValue.isString())
            {
                stringList.append(jsonValue.toString());
            }
            else
            {
                m_errors.append(QString("%1: not a number nor string.").arg(valueErrorContext));
            }
        }
        else if (expectedType == JsonDataType::PersonNameObject)
        {
            if (jsonValue.isObject())
            {
                const QJsonObject &pnJsonObject = jsonValue.toObject();
                /*
                 * Expected structure of pnJsonObject (all elements optional):
                 * {
                 *     "Alphabetic": "Yamada^Tarou",
                 *     "Ideographic": "山田^太郎",
                 *     "Phonetic": "やまだ^たろう"
                 * }
                 */

                // Goal: produce a string with groups like this "Alphabetic=Ideographic=Phonetic". Missing groups at the end can be discarded but not in the
                // middle or beginning, e.g. if there is only Ideographic then the result must be "=Ideographic".

                static const QStringList GroupKeys{"Alphabetic", "Ideographic", "Phonetic"};
                static const QString GroupSeparator{"="};

                QStringList groupList;

                for (const QString &key : GroupKeys)
                {
                    if (pnJsonObject.contains(key))
                    {
                        const QJsonValue &groupValue = pnJsonObject[key];

                        if (groupValue.isString())
                        {
                            groupList.append(groupValue.toString());
                        }
                        else
                        {
                            m_errors.append(QString("%1, key %2: not a string.").arg(valueErrorContext).arg(key));
                            groupList.append(QString());
                        }
                    }
                    else
                    {
                        groupList.append(QString());
                    }
                }

                // Remove trailing null QStrings
                while (!groupList.isEmpty() && groupList.last().isNull())
                {
                    groupList.removeLast();
                }

                QString fullName = groupList.join(GroupSeparator);
                stringList.append(fullName);
            }
            else
            {
                m_errors.append(QString("%1: not an object.").arg(valueErrorContext));
            }
        }
        else
        {
            m_errors.append(QString("%1: unsupported type.").arg(valueErrorContext));
        }

        if (stringList.size() == i) // if nothing has been appended in this iteration (because of an error), append a null QString
        {
            stringList.append(QString());
        }
    }

    return stringList;
}

} // namespace udg
