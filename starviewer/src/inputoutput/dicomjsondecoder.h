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

#ifndef UDG_DICOMJSONDECODER_H
#define UDG_DICOMJSONDECODER_H

#include "dicomsource.h"

class QJsonArray;
class QJsonValue;

namespace udg {

class Image;
class Patient;
class Series;

/**
 * @brief The DicomJsonDecoder class has methods to create Patient, Study, Series and Image instances from the information contained in a given JSON, according
 * to the DICOM JSON model explained in PS3.18§F.
 *
 * Mapping from DICOM VR to JSON types is explained in PS3.18§F.2.3. An example JSON is shown in PS3.18§F.4.
 */
class DicomJsonDecoder
{
public:
    /// Sets the given PACS as the DICOM source.
    /// If \a overrideInstitutionName is true the institution of the PACS will be set to studies instead of their own institution.
    void setPacsDevice(const PacsDevice &pacs, bool overrideInstitutionName = false);
    /// Returns all the errors and warnings encountered during processing.
    const QStringList& getErrors() const;

    /// Processes the given text as a DICOM JSON representing studies metadata, and returns the corresponding Patient and Study instances.
    QList<Patient*> getStudies(const QByteArray &jsonText);
    /// Processes the given text as a DICOM JSON representing series metadata, and returns the corresponding Series instances.
    QList<Series*> getSeries(const QByteArray &jsonText);
    /// Processes the given text as a DICOM JSON representing instances metadata, and returns the corresponding Image instances.
    QList<Image*> getInstances(const QByteArray &jsonText);

public: // All the following methods are internal. They should be private but are public to allow testing.
    /// Processes \a jsonText as a full JSON document with an array at the root, and returns this array.
    QJsonArray getRootArray(const QByteArray &jsonText);

    /// Processes \a jsonValue as a JSON object representing a study, and returns the corresponding Patient and Study instance.
    Patient* getStudy(const QJsonValue &jsonValue, const QString &errorContext);
    /// Processes \a jsonValue as a JSON object representing a series, and returns the corresponding Series instance.
    Series* getSeries(const QJsonValue &jsonValue, const QString &errorContext);
    /// Processes \a jsonValue as a JSON object representing an instance, and returns the corresponding Image instance.
    Image* getInstance(const QJsonValue &jsonValue, const QString &errorContext);

    /// Processes \a jsonValue as a JSON object representing a single-valued tag value, and returns this value as a string.
    QString getTagValueAsString(const QJsonValue &jsonValue, const QString &errorContext);
    /// Processes \a jsonValue as a JSON object representing a multi-valued tag value, and returns these values as a string list.
    QStringList getTagValueAsStringList(const QJsonValue &jsonValue, const QString &errorContext);

    /// JSON data types that can be expected depending on the DICOM VR.
    enum class JsonDataType { Undefined, String, Integer, Double, IntegerOrString, DoubleOrString, PersonNameObject, Array, Base64String };

    /// Reads property "vr" in \a jsonValue and returns the expected JsonDataType according to the standard.
    JsonDataType getExpectedType(const QJsonValue &jsonValue, const QString &errorContext);
    /// Processes \a jsonValue as a JSON object representing a tag value, and returns the array at its "Value" property.
    QJsonArray getValueArray(const QJsonValue &jsonValue, const QString &errorContext);
    /// Processes \a jsonArray as a JSON array with a list of values of \a expectedType, converts them to strings and returns the resulting string list.
    QStringList getArrayAsStringList(const QJsonArray &jsonArray, JsonDataType expectedType, const QString &errorContext);

private:
    /// Institution name that will be set to studies, overriding their actual institution name. If not set, the studies' actual institution name will remain.
    QString m_institutionName;
    /// DICOM source that will be set to studies, series and instances.
    DICOMSource m_dicomSource;
    /// List of all errors encountered during processing.
    QStringList m_errors;
};

} // namespace udg

#endif // UDG_DICOMJSONDECODER_H
