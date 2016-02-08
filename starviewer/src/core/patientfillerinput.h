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

#ifndef UDGPATIENTFILLERINPUT_H
#define UDGPATIENTFILLERINPUT_H

#include "dicomsource.h"

#include <QHash>

namespace udg {

class DICOMTagReader;
class Image;
class Patient;
class Series;

/**
 * @brief The PatientFillerInput class encapsulates input data used by PatientFiller and its steps.
 */
class PatientFillerInput {

public:
    PatientFillerInput();
    ~PatientFillerInput();

    /// Returns the DICOM source of the images.
    const DICOMSource& getDICOMSource() const;
    /// Sets the DICOM source of the images.
    void setDICOMSource(DICOMSource imagesDICOMSource);

    /// Returns the DICOM file that is currently being processed.
    const DICOMTagReader* getDICOMFile() const;
    /// Sets the DICOM file that will be processed. PatientFillerInput takes ownership of the DICOMTagReader.
    void setDICOMFile(const DICOMTagReader *dicomTagReader);

    /// Returns the non-DICOM file that is currently being processed.
    const QString& getFile() const;
    /// Sets the non-DICOM file that will be processed.
    void setFile(QString file);

    /// Adds a patient to the list.
    void addPatient(Patient *patient);
    /// Returns the patient at the given position in the list. If the index is out of range, returns null.
    Patient* getPatient(int index = 0) const;
    /// Returns the patient with the given id in the list. If no such patient exists, returns null.
    Patient* getPatientByID(const QString &id) const;
    /// Returns the number of patients in the list.
    int getNumberOfPatients() const;
    /// Returns the list of generated patients.
    const QList<Patient*>& getPatientList() const;

    /// Returns the series that is currently being processed.
    Series* getCurrentSeries() const;
    /// Sets the series that is currently being processed.
    void setCurrentSeries(Series *series);

    /// Returns the images generated from the current file.
    const QList<Image*>& getCurrentImages() const;
    /// Sets the images generated from the current file. Adds this images to the history except if \a addToHistory is false.
    void setCurrentImages(const QList<Image*> &images, bool addToHistory = true);
    /// Returns the history of current images of the current series.
    QList<QList<Image*>> getCurrentImagesHistory() const;

    /// Returns true if the current series contains at least one multiframe volume.
    bool currentSeriesContainsMultiframeImages() const;

    /// Returns the current volume number.
    int getCurrentVolumeNumber() const;
    /// Sets the current volume number.
    void setCurrentVolumeNumber(int volumeNumber);

    // Disable copy
    PatientFillerInput(const PatientFillerInput&) = delete;
    PatientFillerInput& operator =(const PatientFillerInput&) = delete;

private:
    /// The DICOM source of the images.
    DICOMSource m_imagesDICOMSource;

    /// The DICOM file that is currently being processed.
    const DICOMTagReader *m_dicomFile;

    /// The non-DICOM file that is currently being processed.
    QString m_file;

    /// List of generated patients.
    QList<Patient*> m_patientList;

    /// The series that is currently being processed.
    Series *m_currentSeries;

    /// The images generated from the current file.
    QList<Image*> m_currentImages;
    /// History of current images for each series.
    QHash<Series*, QList<QList<Image*>>> m_perSeriesCurrentImagesHistory;

    /// Hash that stores for each series whether it contains or not at least one file with more than one frame.
    QHash<Series*, bool> m_seriesContainsMultiframeImages;

    /// Hash that stores the current volume number for each series.
    QHash<Series*, int> m_perSeriesCurrentVolumeNumber;

};

}

#endif
