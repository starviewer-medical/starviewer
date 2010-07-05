/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMREFERENCEDIMAGE_H
#define UDGDICOMREFERENCEDIMAGE_H

#include <QObject>

namespace udg {
/**
Classe generica que serveix per a guardar-nos UID d'una imatge, pot esser multiframe, en aquest cas tindra el numero de
frame al qual fa referencia

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMReferencedImage : public QObject
{
Q_OBJECT
public:
    DICOMReferencedImage();

    ~DICOMReferencedImage();

    /// Obte/assigna SOP Instance UID de la imatge referenciada
    QString getDICOMReferencedImageSOPInstanceUID() const;
    void setDICOMReferencedImageSOPInstanceUID(const QString &referencedImageSOPInstanceUID);
    
    /// Obte/assigna Numero de frame al que fem referencia, si no es imatge multiframe tindra valor NULL
    int getFrameNumber() const;
    void setFrameNumber(int frameNumber);

    /// Obte/assigna SOP Instance UID de l'objecte DICOM pare de la referencia
    QString getReferenceParentSOPInstanceUID() const;
    void setReferenceParentSOPInstanceUID(const QString &referenceParentSOPInstanceUID);

private:
    /// UID de la imatge referenciada
    QString m_DICOMReferencedImageSOPInstanceUID;

    /// Numero de frame de la imatge referenciada
    int m_frameNumber;

    /// UID de l'objecte DICOM pare de la referencia
    QString m_referenceParentSOPInstanceUID;
};

}

#endif