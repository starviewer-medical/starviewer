/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGITKGDCMDICOMIMAGEREADER_H
#define UDGITKGDCMDICOMIMAGEREADER_H

#include "dicomimagereader.h"

namespace udg {

/**
Lector d'imatges DICOM que fa servir la interfície d'itk sobre Gdcm

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class itkGdcmDICOMImageReader : public DICOMImageReader
{
Q_OBJECT
public:
    itkGdcmDICOMImageReader(QObject *parent = 0);

    ~itkGdcmDICOMImageReader();

    virtual bool load();
};

}

#endif
