/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMTAGREADER_H
#define UDGDICOMTAGREADER_H

#include <QString>
#include <QList>

#include <dcmtk/dcmdata/dctagkey.h>
#include <dcmtk/dcmdata/dcdeftag.h>

class DcmDataset;

namespace udg {

/**
Classe encarregada de la lectura d'informació de fitxers DICOM.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMTagReader
{
public:
    DICOMTagReader();
    /// Constructor per nom de fitxer.
    DICOMTagReader(QString filename);
    /// Constructor per nom de fitxer per si es té un DcmDataset ja llegit. D'aquesta forma no cal tornar-lo
    /// a llegir.
    DICOMTagReader(QString filename, DcmDataset *dcmDataset);

    /// Constructor

    ~DICOMTagReader();

    /// Nom de l'arxiu DICOM que es vol llegir. Torna cert si l'arxiu s'ha pogut carregar correctament, fals altrament.
    bool setFile( QString filename );

    /// Retorna el nom del fitxer que tracta el tag reader.
    QString getFileName() const;

    /// Mètode de conveniència per aprofitar un DcmDataset ja obert. Es presuposa que dcmDataset no és null i pertany al fitxer passat
    void setDcmDataset(QString filename, DcmDataset *dcmDataset);

    /// Ens diu si el tag és present al fitxer o no. Cal haver fet un ús correcte de l'objecte m_dicomData.
    bool tagExists( DcmTagKey tag );
    bool tagExists( unsigned int group, unsigned int element );

    /// Obtenim el valor d'un atribut en un string fent el query pel seu tag en hexadecimal.Cal haver fet un ús correcte de l'objecte m_dicomData.
    /// Exemple: getAttributeByTag(0x0020, 0x0020);
    /// Retornarà QString buit si no es troba el tag o el tag estigui buit.
    QString getAttributeByTag( unsigned int group, unsigned int element );

    /// Mètode igual que l'anterior però es cerca pel nom a través dels define's de dcmtk. Aquests defines
    /// es poden trobar a /usr/include/dcmtk/dcmdata/dcdeftag.h. Cal haver fet un ús correcte de l'objecte m_dicomData.
    /// Exemple: getAttributeByName(DCM_PatientOrientation);  //Seria equivalent a l'anterior
    /// Retornarà QString buit si no es troba el tag o el tag estigui buit.
    QString getAttributeByName( DcmTagKey tag );

    /// Retorna la llista de valors que pot tenir un atribut dins d'una seqüència. Tindrem tants valors d'aquell atribut com
    /// items tingui la seqüència. El primer paràmetre és el tag de la seqüència i el segon és el tag de l'atribut
    QStringList getSequenceAttributeByTag( unsigned int sequenceGroup, unsigned int sequenceElement, unsigned int group, unsigned int element );
    QStringList getSequenceAttributeByName( DcmTagKey sequenceTag, DcmTagKey attributeTag );

    /// Retorna la llista de valors que pot tenir un atribut dins d'una seqüència aniuada. Donem la llista aniuada de
    /// seqüències i l'atribut que volem de la última seqüència. Tindrem tants valors d'aquell atribut com items tingui la
    /// seqüència. El primer paràmetre és el tag de la seqüència i el segon és el tag de l'atribut
    QStringList getSequenceAttributeByTag( QList<unsigned int *> embeddedSequencesTags, unsigned int group, unsigned int element );
    QStringList getSequenceAttributeByName( QList<DcmTagKey> embeddedSequencesTags, DcmTagKey attributeTag );

private:
    /// Path absolut on es troba l'arxiu del qual extraiem la informació
    QString m_filename;

    /// Objecte dcmtk a través del qual obtenim la informació DICOM
    DcmDataset *m_dicomData;
};

}

#endif
