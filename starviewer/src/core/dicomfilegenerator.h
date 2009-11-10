/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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

@author Grup de Gràfics de Girona  ( GGG )
*/
class DICOMFileGenerator
{
public:
    DICOMFileGenerator();

    ~DICOMFileGenerator();

    /**
        Assignar el directori a on es vol guardar el/s fitxer/s generat/s
        Si el directori no existeix es crearà.
    */
    bool setDirPath( QString path );

    /// Mètode encarregat de generar el/s fitxer/s. Cada generador decideix què genera.
    virtual bool generateDICOMFiles() = 0;

protected:
    /// Omple la informació bàsica de pacient
    void fillPatientInfo( DICOMWriter *writer, Patient *patient );

    /// Omple la informació bàsica de l'estudi
    void fillStudyInfo( DICOMWriter *writer, Study *study );

    /// Omple la informació bàsica de la serie
    void fillSeriesInfo( DICOMWriter *writer, Series *series );

    /// Omple la informació del modul "General Equipment"
    void fillGeneralEquipmentInfo( DICOMWriter *writer, Series *series );

protected:
    QDir m_dir;
};

}

#endif // UDGDICOMFILEGENERATOR_H
