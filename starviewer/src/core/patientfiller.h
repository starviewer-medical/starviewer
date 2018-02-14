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

#ifndef UDGPATIENTFILLER_H
#define UDGPATIENTFILLER_H

#include <QObject>

#include "dicomsource.h"

namespace udg {

class DICOMTagReader;
class Patient;
class PatientFillerInput;
class PatientFillerStep;

/**
 * @brief The PatientFiller class generates patients, studies, series, images and volumes from a list of DICOM or non-DICOM files.
 *
 * Files can be given to it one by one (e.g. as they arrive from PACS) in processDICOMFile() and then call finishDICOMFilesProcess() after the last file.
 * Alternatively, files can be given to it all at once (e.g. when reading fils from a directory) in processFiles().
 *
 * The files are processed by several steps that share a common PatientFillerInput.
 */
class PatientFiller : public QObject {

    Q_OBJECT

public:
    PatientFiller(DICOMSource dicomSource = DICOMSource(), QObject *parent = 0);
    virtual ~PatientFiller();

public slots:
    /// Processes the given DICOM file. Executes the first stage steps with the file. Emits the progress() signal at the end.
    void processDICOMFile(const DICOMTagReader *dicomTagReader);

    /// Executes the second stage steps with all the images generated in the first stage and then executes the post-processing.
    /// Emits the patientProcessed() signal at the end.
    void finishFilesProcessing();

    /// Processes the given files executing both stages and post-processing. Returns the generated patients.
    QList<Patient*> processFiles(const QStringList &files);

signals:
    /// This signal is emitted each time a file is processed.
    void progress(int numberOfProcessedFiles);

    /// This signal is emitted when finishDICOMFilesProcess() has finished, with the first generated patient.
    /// \todo If more than one patient was generated, all but the first will be ignored.
    void patientProcessed(Patient *patient);

private:
    /// Creates the steps of the patient filler. Different steps are created for DICOM vs non-DICOM files.
    void createSteps();

    /// Applies the first stage steps to the current file in the patient filler input.
    void processCurrentFile();

private:
    /// If true, all files will be considered as DICOM, otherwise as non-DICOM.
    bool m_dicomMode;

    /// Steps that are executed in the first stage of processing.
    QList<PatientFillerStep*> m_firstStageSteps;
    /// Steps that are executed in the second stage of processing.
    QList<PatientFillerStep*> m_secondStageSteps;

    /// S'encarrega de guardar l'input durant tota l'execucció dels mòduls. S'utilitza
    /// en cas que es processin fitxer individualment.
    PatientFillerInput *m_patientFillerInput;

    /// Counts the number of processed files.
    int m_numberOfProcessedFiles;

};

}

#endif
