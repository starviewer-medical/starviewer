/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMWRITER_CPP
#define UDGDICOMWRITER_CPP

#include "dicomwriter.h"
#include "dicomwriterdcmtk.h"

namespace udg {

DICOMWriter::~DICOMWriter()
{

}

DICOMWriter * DICOMWriter::newInstance() {
    return new DICOMWriterDCMTK();
}

void DICOMWriter::setPath(QString path)
{
    m_path = path;
}

QString DICOMWriter::getPath()
{
    return m_path;
}

}

#endif
