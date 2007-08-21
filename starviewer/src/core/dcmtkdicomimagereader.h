/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGdcmtkDICOMIMAGEREADER_H
#define UDGdcmtkDICOMIMAGEREADER_H

#include "dicomimagereader.h"

namespace udg {

/**
Lector d'imatges DICOM que fa servir les llibreries dcmtk per llegir

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class dcmtkDICOMImageReader : public DICOMImageReader
{
Q_OBJECT
public:
    dcmtkDICOMImageReader(QObject *parent = 0);

    ~dcmtkDICOMImageReader();

    virtual bool load();
};

}

#endif
