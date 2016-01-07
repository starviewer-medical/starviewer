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

#ifndef UDGDICOMFILEGENERATOR_H
#define UDGDICOMFILEGENERATOR_H

#include <QDir>
#include <QString>

namespace udg {

class DICOMWriter;
class Patient;
class Study;
class Series;

/**
    Classe base per a la generació de nous fitxers DICOM. Cada generador té la capacitat de generar un tipus de fitxers.
*/
class DICOMFileGenerator {
public:
    DICOMFileGenerator();

    ~DICOMFileGenerator();

    /// Assignar el directori a on es vol guardar el/s fitxer/s generat/s
    /// Si el directori no existeix es crearà.
    bool setDirPath(QString path);

    /// Mètode encarregat de generar el/s fitxer/s. Cada generador decideix què genera.
    virtual bool generateDICOMFiles() = 0;

protected:
    /// Omple la informació bàsica de pacient
    void fillPatientInfo(DICOMWriter *writer, Patient *patient);

    /// Omple la informació bàsica de l'estudi
    void fillStudyInfo(DICOMWriter *writer, Study *study);

    /// Omple la informació bàsica de la serie
    void fillSeriesInfo(DICOMWriter *writer, Series *series);

    /// Omple la informació del modul "General Equipment"
    void fillGeneralEquipmentInfo(DICOMWriter *writer, Series *series);

protected:
    QDir m_dir;
};

}

#endif // UDGDICOMFILEGENERATOR_H
