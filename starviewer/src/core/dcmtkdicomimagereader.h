/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDCMTKDICOMIMAGEREADER_H
#define UDGDCMTKDICOMIMAGEREADER_H

#include "dicomimagereader.h"

namespace udg {

/**
Lector d'imatges DICOM que fa servir les llibreries DCMTK per llegir

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DCMTKDICOMImageReader : public DICOMImageReader
{
Q_OBJECT
public:
    DCMTKDICOMImageReader(QObject *parent = 0);

    ~DCMTKDICOMImageReader();

    virtual bool load();
};

}

#endif
