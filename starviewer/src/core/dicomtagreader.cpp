/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomtagreader.h"
#include "logging.h"

#include <QStringList>

#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcsequen.h"

namespace udg {

// Inicialitzem l'autoclear a 300 segons.
DcmDatasetCache DICOMTagReader::m_cache(300);

DICOMTagReader::DICOMTagReader() : m_dicomData(0)
{
}

DICOMTagReader::DICOMTagReader( QString filename ) : m_dicomData(0)
{
    this->setFile( filename );
}

DICOMTagReader::~DICOMTagReader()
{
    delete m_dicomData;
}

bool DICOMTagReader::setFile( QString filename )
{
    DcmFileFormat dicomFile;
    DcmDataset *dataset = m_cache.find(filename);
    if (! dataset)
    {
        OFCondition status = dicomFile.loadFile( qPrintable(filename) );
        if( status.good() )
        {
            // eliminem l'objecte anterior si n'hi hagués
            if( m_dicomData )
            {
                delete m_dicomData;
                m_dicomData = NULL;
            }

            dataset =  dicomFile.getAndRemoveDataset();
            m_cache.insert( filename, dynamic_cast<DcmDataset*>(dataset->clone()) );
        }
        else
        {
            DEBUG_LOG( QString( "Error en llegir l'arxiu [%1]\n%2 ").arg( filename ).arg( status.text() ) );
            return false;
        }
    }
    m_dicomData = dataset;
    return true;
}

bool DICOMTagReader::tagExists( DcmTagKey tag )
{
    if( m_dicomData )
    {
        return m_dicomData->tagExists( tag );
    }
    else
    {
        DEBUG_LOG("El m_dicomData no és vàlid");
        return false;
    }
}

bool DICOMTagReader::tagExists( unsigned int group, unsigned int element )
{
    return this->tagExists( DcmTagKey(group,element) );
}

QString DICOMTagReader::getAttributeByTag( unsigned int group, unsigned int element )
{
    return this->getAttributeByName( DcmTagKey(group,element) );
}

QString DICOMTagReader::getAttributeByName( DcmTagKey tag )
{
    QString result;
    if( m_dicomData )
    {
        OFString value;
        OFCondition status = m_dicomData->findAndGetOFStringArray( tag , value );
        if( status.good() )
        {
            result = value.c_str();
        }
        else
        {
            if (QString(status.text()) != "Tag Not Found")
            {
                DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2")
                                    .arg( tag.toString().c_str() ).arg( status.text() ) );
            }
        }
    }
    else
        DEBUG_LOG("El m_dicomData no és vàlid");

    return result;
}

QStringList DICOMTagReader::getSequenceAttributeByTag( unsigned int sequenceGroup, unsigned int sequenceElement, unsigned int group, unsigned int element )
{
    return this->getSequenceAttributeByName( DcmTagKey(sequenceGroup,sequenceElement) , DcmTagKey(group,element) );
}

QStringList DICOMTagReader::getSequenceAttributeByName( DcmTagKey sequenceTag, DcmTagKey attributeTag )
{
    QStringList result;
    // obtenim els atributs de cada item d'una seqüència de "primer nivell"
    if( m_dicomData )
    {
        DcmStack stack;

        OFCondition status = m_dicomData->search( sequenceTag, stack );

        if( status.good() )
        {
            OFString value;
            DcmSequenceOfItems *sequence = NULL;
            sequence = OFstatic_cast( DcmSequenceOfItems *,stack.top() );
            for(unsigned int i = 0; i < sequence->card(); i++ )
            {
                DcmItem *item = sequence->getItem( i );
                status = item->findAndGetOFStringArray( attributeTag , value );
                if( status.good() )
                {
                    result << value.c_str();
                }
                else if( QString(status.text()) != "Tag Not Found" )
                {
                    DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg( attributeTag.toString().c_str() ).arg( status.text() ) );
                }
            }
        }
        else if( QString(status.text()) != "Tag Not Found" )
            DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg( sequenceTag.toString().c_str() ).arg( status.text() ) );
    }
    else
        DEBUG_LOG("El m_dicomData no és vàlid");

    return result;
// \TODO el que ve a continuació és com hauria de ser quan s'implementi el mètode amb seqüències "embedded"
//     QList<DcmTagKey> embeddedSequenceList;
//     embeddedSequenceList << sequenceTag;
//     return this->getSequenceAttributeByName( embeddedSequenceList, attributeTag );
}

QStringList DICOMTagReader::getSequenceAttributeByTag( QList<unsigned int *> embeddedSequencesTags, unsigned int group, unsigned int element )
{
    QList<DcmTagKey> embeddedSequenceList;
    foreach( unsigned int *tagNumbers, embeddedSequencesTags )
    {
        embeddedSequenceList << DcmTagKey( tagNumbers[0], tagNumbers[1] );
    }
    return this->getSequenceAttributeByName( embeddedSequenceList, DcmTagKey( group, element ) );

}

QStringList DICOMTagReader::getSequenceAttributeByName( QList<DcmTagKey> embeddedSequencesTags, DcmTagKey attributeTag )
{
    QStringList result;
    DEBUG_LOG("DICOMTagReader::getSequenceAttributeByName( QList<DcmTagKey> embeddedSequencesTags, DcmTagKey attributeTag )  == Crida a Mètode no implementat!");
//\TODO per implementar. Aquesta part és una mica més fotuda.
// aquí també es podria fer servir DcmItem::findAndGetElements, que torna un stack amb tot de DcmObjects que continguin l'atribut demanat. Fa un deep search per tant busca dins de les seqüències

// l'altre opció és anar iterant sobre les seqüències i els seus ítems amb findAndGetSequenceItem

// links que ens poden ajudar
// http://forum.dcmtk.org/viewtopic.php?t=881&highlight=sequence
// http://forum.dcmtk.org/viewtopic.php?t=698&highlight=sequence
// http://forum.dcmtk.org/viewtopic.php?t=386&highlight=sequence en aquest expliquen i definexien molt bé com és un seqüència

//     int i = 0;
//     bool ok = true;
//     DcmStack stack;
//     DcmSequenceOfItems *sequence = NULL;
//     while( i < embeddedSequencesTags.size() && ok )
//     {
//         OFCondition status = m_dicomData->search( embeddedSequencesTags.at(i), stack );
//         if( status.good() )
//         {
//             sequence = OFstatic_cast( DcmSequenceOfItems *,stack.top() );
//             //\TODO ara només tenim en compte que la seqüència té un sol ítem. A
//             DcmItem *item = sequence->getItem( 0 );
//         }
//         else
//         {
//         }
//         sequence =
//         i++;
//     }
//
//     DcmStack stack;
//     DcmSequenceOfItems *lutSequence = NULL;
//     lutSequence = OFstatic_cast( DcmSequenceOfItems *,stack.top() );
//     DcmItem *item = lutSequence->getItem( 0 );
//     // obtenim la descripció de la lut que ens especifica el format d'aquesta
//     const Uint16 *lutDescriptor;
//     OFCondition status = item->findAndGetUint16Array( DcmTagKey( DCM_LUTDescriptor ) , lutDescriptor  );
//     if( status.good() )
//     {
//
//         int numberOfEntries;
//         if( lutDescriptor[0] == 0 )
//             numberOfEntries = 65535;
//         else
//             numberOfEntries =  lutDescriptor[0];
//         signed int firstStored;
//         if( signedRepresentation )
//             firstStored = static_cast<signed short>( lutDescriptor[1] );
//         else
//             firstStored = lutDescriptor[1];
//
//         DEBUG_LOG( QString("LUT Descriptor: %1\\%2\\%3")
//         .arg( numberOfEntries )
//         .arg( firstStored )
//         .arg( lutDescriptor[2] )
//         );
//     }

    return result;
}

}
