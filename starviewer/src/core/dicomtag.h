/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMTAG_H
#define UDGDICOMTAG_H

#include "dicomdictionary.h"

namespace udg {

/**
   Classe per representar un tag DICOM.
   Un tag està format per enters, el group i l'element. Ex: (0020,0022)

   @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class DICOMTag
{
public:
    DICOMTag();
    DICOMTag( unsigned int group, unsigned int element );
    DICOMTag( DICOMTag * tag );

    ~DICOMTag();

    /// Assignar/Obtenir el valor del group
    void setGroup( unsigned int group );
    unsigned int getGroup();

    /// Assignar/Obtenir el valor de element
    void setElement( unsigned int element );
    unsigned int getElement();

private:
    unsigned int m_group;
    unsigned int m_element;
};

}

#endif // DICOMTAG_H
