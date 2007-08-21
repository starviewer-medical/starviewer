/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomimagereader.h"

#include "logging.h"

namespace udg {

DICOMImageReader::DICOMImageReader(QObject *parent)
 : QObject(parent), m_imageBuffer(0), m_sliceByteIncrement(0)
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
    m_imageBuffer = (unsigned char *)buffer;
}

bool DICOMImageReader::readyToLoad()
{
    bool ok = true;
    if( m_inputImageList.isEmpty() )
    {
        DEBUG_LOG("No podem carregar cap imatge, la llista és buida");
        ok = false;
    }
    if( m_imageBuffer == NULL )
    {
        DEBUG_LOG("No podem carregar cap imatge, el buffer és NUL");
        ok = false;
    }
    if( m_sliceByteIncrement == 0 )
    {
        DEBUG_LOG("No podem carregar cap imatge, l'increment de bytes és = 0 ");
        ok = false;
    }
    return ok;
}

}
