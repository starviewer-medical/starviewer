/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMIMAGEREADERDCMTK_H
#define UDGDICOMIMAGEREADERDCMTK_H

#include "dicomimagereader.h"

namespace udg {

/**
Lector d'imatges DICOM que fa servir les llibreries dcmtk per llegir

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMImageReaderDCMTK : public DICOMImageReader
{
Q_OBJECT
public:
    DICOMImageReaderDCMTK(QObject *parent = 0);

    ~DICOMImageReaderDCMTK();

    virtual bool load();
};

}

#endif
