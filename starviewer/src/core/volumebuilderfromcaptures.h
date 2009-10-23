/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMEBUILDERFROMCAPTURES_H
#define UDGVOLUMEBUILDERFROMCAPTURES_H

#include "volumebuilder.h"

// FWD declarations
class vtkImageData;
class vtkImageAppend;

namespace udg {

class Study;

/**
Classe encarregada de generar un nou objecte Volume a partir de captures de pantalla representades amb un vtkImageData.
Per anar afegint captures cal utilitzar el mètode \sa addCapture i l'algorisme està preparat perquè funcioni si els objectes
vtkImageData conten més d'una llesca. L'única restricció és que tots els inputs han de tenir el mateix extent (exceptuant
l'eix de les z) i que tots els inputs han de tenir el mateix número de components per cada pixel.

És imprescindible indicar l'estudi al que volem que pertanyi la nova sèrie. També es dóna la possibilitat d'assignar la
modalitat a la que volem que pertanyi (per defecte 'OT' (Other)) tenint en compte que el SOP Class UID sempre
serà Secondary Capture Image Storage (PS 3.4 I.4).

IMPORTANT: El Volume generat NOMÉS té el pixel data en memòria representat amb un vtkImageData (o itk). Per tant, els objectes
Image associat al Volume no estan relacionats amb cap fitxer ( path == "" ). Si es necessita els fitxers caldria exportar el
Volume.

@author Grup de Gràfics de Girona  ( GGG )
*/

class VolumeBuilderFromCaptures : public VolumeBuilder
{
public:

    VolumeBuilderFromCaptures();

    ~VolumeBuilderFromCaptures();

    /**
        Afegeix captures de pantalla en format vtkImageData. Es suporta la possibilitat que el vtkImageData tingui més d'una llesca (z > 1).
        @pre Tots els inputs han de tenir el mateix extent (exceptuant l'eix de les z) i que tots els
        inputs han de tenir el mateix número de components per cada pixel.
    */
    void addCapture( vtkImageData * data );

    /**
        Genera un nou objecte Volume a partir de les captures afegides i la sèrie generada s'assigna a l'estudi introduït.
        @pre S'ha d'haver introduit l'estudi al que volem que s'afegeixi el Volume i s'ha d'haver afegit alguna captura.
    */
    Volume * build();

    /// Afegir l'estudi al que volem que pertanyi la nova sèrie que es crearà.
    void setParentStudy( Study * study );

    /**
        Assignar la modalitat que volem que tingui la sèrie tenint en compte que el SOP Class UID sempre
        serà Secondary Capture Image Storage (PS 3.4 I.4).
        Valor per defecte: 'OT' (Other)
        @return Cert si el valor que es vol assignar pertany a una modalitat vàlida (PS 3.3 C.7.3.1.1.1). Si no és vàlid no hi ha assignació i retorna fals.
    */
    bool setModality( QString modality );

private:

    /// Object encarregat de generar el nou vtkImageData
    vtkImageAppend * m_vtkImageAppend;

    /// Estudi al que s'afegirà la nova sèrie que es generarà
    Study * m_parentStudy;

    /// Modalitat que s'assignarà a la nova sèrie
    QString m_modality;

};

};  // end namespace udg

#endif // VOLUMEBUILDERFROMCAPTURES_H
