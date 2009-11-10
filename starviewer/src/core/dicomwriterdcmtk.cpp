/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMWRITERDCMTK_CPP
#define UDGDICOMWRITERDCMTK_CPP

#include "dicomwriterdcmtk.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceattribute.h"
#include "dicomtag.h"

#include <dcmdata/dctag.h>
#include <dcmdata/dcdatset.h>
#include <dcmdata/dcfilefo.h>
#include <dcmpstat/dvpshlp.h>

#include "logging.h"

namespace udg {

DICOMWriterDCMTK::DICOMWriterDCMTK()
{
    m_dataset = new DcmDataset();
}

DICOMWriterDCMTK::~DICOMWriterDCMTK()
{
    delete m_dataset;
}


void DICOMWriterDCMTK::addValueAttribute(DICOMValueAttribute * attribute)
{

    DcmTag tag( attribute->getTag()->getGroup() , attribute->getTag()->getElement() );

    if ( attribute->getValueRepresentation() == DICOMValueAttribute::ByteArray )
    {
        m_dataset->putAndInsertUint8Array( tag , static_cast<Uint8 *>( (void *) attribute->getValueAsByteArray().data() ) , static_cast<unsigned long>( attribute->getValueAsByteArray().length() ) , true );
    }
    else
    {
        m_dataset->putAndInsertString( tag , qPrintable( attribute->getValueAsQString() ) , true );
    }

}

void DICOMWriterDCMTK::addSequenceAttribute(DICOMSequenceAttribute * attribute)
{
    Q_UNUSED( attribute );
}

bool DICOMWriterDCMTK::write()
{
    DcmFileFormat * file = new DcmFileFormat( m_dataset );

    //Guardem la imatge
    OFCondition saveFileCondition = DVPSHelper::saveFileFormat( qPrintable( this->getPath() ) , file , true);
    
    delete file;

    if ( saveFileCondition == EC_Normal )
    {
        DEBUG_LOG( QString("Fitxer DICOM generat correctament: %1").arg( this->getPath() ) );
        return true;
    }
    else
    {
        DEBUG_LOG( QString("No s'ha pogut generar el fitxer DICOM: %1").arg( this->getPath() ) );
        return false;
    }

}

}

#endif
