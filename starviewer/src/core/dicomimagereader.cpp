/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomimagereader.h"

namespace udg {

DICOMImageReader::DICOMImageReader(QObject *parent)
 : QObject(parent), m_imageBuffer(0)
{
}


DICOMImageReader::~DICOMImageReader()
{
}

void DICOMImageReader::setInputImages( const QList<Image *> &imageList )
{
    m_inputImageList = imageList;
}

void DICOMImageReader::setBufferPointer( const void *buffer )
{
    m_imageBuffer = (void *)buffer;
}

}
