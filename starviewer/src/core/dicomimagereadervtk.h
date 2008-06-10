/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMIMAGEREADERVTK_H
#define UDGDICOMIMAGEREADERVTK_H

#include "dicomimagereader.h"

namespace udg {

/**
Lector d'imatges DICOM que fa servir els lectors DICOM de vtk

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMImageReaderVTK : public DICOMImageReader
{
Q_OBJECT
public:
    DICOMImageReaderVTK(QObject *parent = 0);

    ~DICOMImageReaderVTK();

    virtual bool load();
};

}

#endif
