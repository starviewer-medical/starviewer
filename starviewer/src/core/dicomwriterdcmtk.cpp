/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomwriterdcmtk.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceattribute.h"
#include "dicomsequenceitem.h"
#include "dicomtag.h"

#include <dcmdata/dctag.h>
#include <dcmdata/dcdatset.h>
#include <dcmdata/dcfilefo.h>
#include <dcmpstat/dvpshlp.h>
#include <dcmdata/dcsequen.h>
#include <dcmdata/dcitem.h>

#include "logging.h"

namespace udg {

DICOMWriterDCMTK::DICOMWriterDCMTK()
{
    m_fileFormat = new DcmFileFormat();
}

DICOMWriterDCMTK::~DICOMWriterDCMTK()
{
    delete m_fileFormat;
}


void DICOMWriterDCMTK::addValueAttribute(DICOMValueAttribute * attribute)
{
    DcmDataset * dataset = m_fileFormat->getDataset();
    DcmTag tag( attribute->getTag()->getGroup() , attribute->getTag()->getElement() );

    if ( attribute->getValueRepresentation() == DICOMValueAttribute::ByteArray )
    {
        dataset->putAndInsertUint8Array( tag , static_cast<Uint8 *>( (void *) attribute->getValueAsByteArray().data() ) , static_cast<unsigned long>( attribute->getValueAsByteArray().length() ) , true );
    }
    else
    {
        dataset->putAndInsertString( tag , qPrintable( attribute->getValueAsQString() ) , true );
        
        /*Uint16 valor;
        if( dataset->findAndGetUint16(DCM_Rows, valor).good() )
            std::cout << valor << std::endl;*/
    }

}

bool DICOMWriterDCMTK::addSequenceAttribute(DICOMSequenceAttribute * attribute)
{
    DcmDataset * dataset = m_fileFormat->getDataset();
    DcmSequenceOfItems * sequence = generateDcmSequenceOfItems( attribute );

    if( sequence == NULL )
        return false;

    return dataset->insert(sequence,true).good();
}

DcmSequenceOfItems * DICOMWriterDCMTK::generateDcmSequenceOfItems( DICOMSequenceAttribute * sequenceAttribute )
{
    DcmTag tag( sequenceAttribute->getTag()->getGroup() , sequenceAttribute->getTag()->getElement() );
    DcmSequenceOfItems * sequence = new DcmSequenceOfItems(tag);

    if( sequence == NULL )
        return NULL;

    foreach( DICOMSequenceItem * dicomItem, sequenceAttribute->getItems() )
    {
        DcmItem * item = new DcmItem( DCM_Item );
        
        
        foreach( DICOMAttribute * attribute, dicomItem->getAttributes() )
        {
            if( attribute->isValueAttribute() )
            {
                DICOMValueAttribute * valueAttribute = dynamic_cast< DICOMValueAttribute * >(attribute);
                DcmTag itemTag( valueAttribute->getTag()->getGroup() , valueAttribute->getTag()->getElement() );

                if ( valueAttribute->getValueRepresentation() == DICOMValueAttribute::ByteArray )
                {
                    OFCondition cond = item->putAndInsertUint8Array( itemTag , static_cast<Uint8 *>( (void *) valueAttribute->getValueAsByteArray().data() ) , static_cast<unsigned long>( valueAttribute->getValueAsByteArray().length() ) , true );
                    
                    if( cond.bad() )
                        return NULL;
                }
                else
                {
                    OFCondition cond = item->putAndInsertString( itemTag , qPrintable( valueAttribute->getValueAsQString() ) , true );
                    
                    if( cond.bad() )
                        return NULL;
                }
            }
            else if( attribute->isSequenceAttribute() )
            {
                DICOMSequenceAttribute * sequenceAttribute2 = dynamic_cast<DICOMSequenceAttribute*> (attribute);
                DcmSequenceOfItems * sequence2 = generateDcmSequenceOfItems( sequenceAttribute2 );

                if( sequence2 == NULL )
                    return NULL;

                OFCondition cond = item->insert( sequence2 , true );
                if( cond.bad() )
                    return NULL;
            }
            else
            {
                DEBUG_LOG( QString("Tipus d'element no suportat al generar la sequencia.") );
                return NULL;
            }
        }

        OFCondition insertCondition = sequence->append( item );
        
        if( insertCondition.bad() )
            return NULL;
    }

    return sequence;
}

bool DICOMWriterDCMTK::write()
{
    DcmElement * element;
    m_fileFormat->getDataset()->findAndGetElement(DCM_Rows,element);
    element->print(std::cout);

    DcmElement * element_col;
    m_fileFormat->getDataset()->findAndGetElement(DCM_Columns,element_col);
    element_col->print(std::cout);

    //Guardem la imatge
    OFCondition saveFileCondition = DVPSHelper::saveFileFormat( qPrintable( this->getPath() ) , m_fileFormat , true);

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
