/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMIMAGEREADERITKGDCM_H
#define UDGDICOMIMAGEREADERITKGDCM_H

#include "dicomimagereader.h"

namespace udg {

/**
Lector d'imatges DICOM que fa servir la interfície d'itk sobre Gdcm

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMImageReaderITKGdcm : public DICOMImageReader
{
Q_OBJECT
public:
    DICOMImageReaderITKGdcm(QObject *parent = 0);

    ~DICOMImageReaderITKGdcm();

    virtual bool load();
};

}

#endif
