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

#include <QStringList>
#include <QMultiMap>
#include <QHash>

#include "dicomsource.h"

namespace udg {

class Patient;
class Series;
class Image;
class DICOMTagReader;

/**
    Classe que encapsula els paràmetres d'input que es faran servir a PatientFiller
  */
class PatientFillerInput {
public:
    PatientFillerInput();

    ~PatientFillerInput();

    /// Afegeix un pacient a la llista
    void addPatient(Patient *patient);

    /// Obté un pacient de la llista indexat. Si l'índex supera el nombre de membres de la llista es retorna NUL
    Patient* getPatient(int index = 0);

    /// Obté un pacient identificat pel seu nom
    Patient* getPatientByName(QString name);

    /// Obté un pacient identificat pel seu ID
    Patient* getPatientByID(QString id);

    /// Retorna el nombre de pacients que tenim a la llista
    unsigned int getNumberOfPatients();

    /// Retorna la llista de Patients que tenim.
    QList<Patient*> getPatientsList();

    /// Assigna/Obté l'arxiu a tractar. TODO: De moment és independent del setDICOMFile i només té sentit per fitxers
    /// no DICOM, com els fitxers MHD. Per DICOM cal utilitzar setDICOMFile(DICOMTagReader*).
    void setFile(QString file);
    QString getFile() const;

    /// S'indica/obté quin serà el DICOMTagReader a processar. Aquest mètode esborrarà l'objecte que es tenia guardat
    /// anteriorment fent que no es pugui utilitzar més: es pren el control absolut de l'objecte.
    /// Per objectes no dicom, cal utilitzar set/getFile(QString)
    void setDICOMFile(const DICOMTagReader *dicomTagReader);
    const DICOMTagReader* getDICOMFile();

    /// Assignar/Obtenir la llista d'imatges que s'han de processar.
    void setCurrentImages(const QList<Image*> &images, bool addToHistory = true);
    QList<Image*> getCurrentImages();
    QList<QList<Image*>> getCurrentImagesList() const;

    /// Afegir / Obtenir la sèrie del fitxer que s'ha de processar.
    void setCurrentSeries(Series *series);
    Series* getCurrentSeries();

    /// Returns true if the current series already contains at least one multiframe volume.
    bool currentSeriesContainsMultiframeImages() const;

    /// Assigna/Retorna el número de volum actual que estem tractant, necessari pels
    /// passos posteriors a l'ImageFillerStep
    void setCurrentVolumeNumber(int volumeNumber);
    int getCurrentVolumeNumber() const;

    /// Assigna/Retorna el DICOMSource de les imatges
    void setDICOMSource(const DICOMSource &imagesDICOMSource);
    DICOMSource getDICOMSource() const;

private:
    /// Llista de pacients a omplir
    QList<Patient*> m_patientList;

    /// Arxius que cal tractar per omplir la llista de pacients
    QString m_file;

    /// Atribut que s'utilitza per executar els fillers individualment.
    const DICOMTagReader *m_dicomFile;

    /// Guarda les imatges que els fillers han de processar.
    QList<Image*> m_currentImages;
    /// Hash that contains the list of "current images" that have been set for each series.
    QHash<Series*, QList<QList<Image*>>> m_perSeriesCurrentImagesList;
    /// Hash that stores for each series whether it contains or not at least one file with more than one frame.
    QHash<Series*, bool> m_seriesContainsMultiframeImages;

    /// Guardem la sèrie del fitxer que els fillers han de processar. S'utilitza si es vol exectuar els fillers individualment per fitxers.
    Series *m_currentSeries;

    /// Hash that stores the current volume number for each series.
    QHash<Series*, int> m_perSeriesCurrentVolumeNumber;

    /// Conté el DICOMSource del qual provenen les imatges
    DICOMSource m_imagesDICOMSource;
};

}

#endif
