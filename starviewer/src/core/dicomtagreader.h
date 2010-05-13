/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMTAGREADER_H
#define UDGDICOMTAGREADER_H

#include <QString>
// Pràcticament sempre que volguem fer servir aquesta classe farem ús del diccionari
#include "dicomdictionary.h"

class DcmDataset;
class DcmSequenceOfItems;

namespace udg {

class DICOMTag;
class DICOMSequenceAttribute;

/**
   Classe encarregada de la lectura d'informació de fitxers DICOM.
   Es pot crear a partir d'un fitxer (el cas més habitual) o bé aprofitant un DcmDataset ja existent (per casos d'eficiència).
   En el cas de crear un DICOMTagReader a partir d'un DcmDataset cal tenir en compte que el propietari d'aquest serà el
   mateix DICOMTagReader i, per tant, no es pot destruir l'objecte des de fora. Ho farà el mateix DICOMTagReader.

   @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMTagReader {
public:
    DICOMTagReader();
    /// Constructor per nom de fitxer.
    DICOMTagReader(const QString &filename);
    /// Constructor per nom de fitxer per si es té un DcmDataset ja llegit. 
    /// D'aquesta forma no cal tornar-lo a llegir.
    DICOMTagReader(const QString &filename, DcmDataset *dcmDataset);

    ~DICOMTagReader();

    /// Nom de l'arxiu DICOM que es vol llegir. Torna cert si l'arxiu s'ha pogut carregar correctament, fals altrament.
    bool setFile( const QString &filename );

    /// Ens diu si l'arxiu assignat és vàlid com a arxiu DICOM. Si no tenim arxiu assignat retornarà fals.
    bool canReadFile() const;

    /// Retorna el nom del fitxer que tracta el tag reader.
    QString getFileName() const;

    /// Mètode de conveniència per aprofitar un DcmDataset ja obert. Es presuposa que dcmDataset no és null i pertany al fitxer passat.
    /// En el cas que ja tingués un fitxer obert, el substitueix esborrant el DcmDataset anterior. Un cop passat el propietari
    /// del DcmDataset passa a ser el DICOMTagReader.
    void setDcmDataset(const QString &filename, DcmDataset *dcmDataset);

    /// Retorna el Dataset de dcmtk que es fa servir internament
    DcmDataset *getDcmDataset() const;
    
    /// Ens diu si el tag és present al fitxer o no. Cal haver fet un ús correcte de l'objecte m_dicomData.
    bool tagExists( const DICOMTag &tag );

    /// Obté el valor de l'atribut demanat i ens el retorna com a QString
    /// Si no es troba el tag es retornarà un QString buit
    QString getValueAttributeAsQString( const DICOMTag &tag );

    /// Retorna un objecte nou que inclou tota la seqüència. Si no existeix o el tag no correspon a una seqüència retorna null.
    DICOMSequenceAttribute * getSequenceAttribute( const DICOMTag &sequenceTag );

private:
    /// Converteix una seqüència de DCMTK a una seqüència pròpia.
    DICOMSequenceAttribute * convertToDICOMSequenceAttribute( DcmSequenceOfItems * dcmtkSequence );

private:
    /// Path absolut on es troba l'arxiu del qual extraiem la informació
    QString m_filename;

    /// Objecte dcmtk a través del qual obtenim la informació DICOM
    DcmDataset *m_dicomData;

    /// Ens indica si l'arxiu actual és vàlid
    bool m_hasValidFile;
};

}

#endif
