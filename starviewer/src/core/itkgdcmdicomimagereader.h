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
Lector d'imatges DICOM que fa servir la interfície d'itk sobre GDCM

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class itkGDCMDICOMImageReader : public DICOMImageReader
{
Q_OBJECT
public:
    itkGDCMDICOMImageReader(QObject *parent = 0);

    ~itkGDCMDICOMImageReader();

    virtual bool load();
};

}

#endif
