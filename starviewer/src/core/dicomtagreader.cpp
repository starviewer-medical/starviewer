/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomtagreader.h"
#include "logging.h"
#include "singleton.h"
#include "dicomtag.h"
#include <QStringList>

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcsequen.h>

namespace udg {

DICOMTagReader::DICOMTagReader() : m_dicomData(0)
{
}

DICOMTagReader::DICOMTagReader(QString filename, DcmDataset *dcmDataset) : m_dicomData(0)
{
    this->setDcmDataset(filename, dcmDataset);
}

DICOMTagReader::DICOMTagReader( QString filename ) : m_dicomData(0)
{
    this->setFile( filename );
}

DICOMTagReader::~DICOMTagReader()
{
    if( m_dicomData )
        delete m_dicomData;
}

bool DICOMTagReader::setFile( QString filename )
{
    DcmFileFormat dicomFile;

    m_filename = filename;

    OFCondition status = dicomFile.loadFile( qPrintable(filename) );
    if( status.good() )
    {
        // eliminem l'objecte anterior si n'hi hagués
        if( m_dicomData )
        {
            delete m_dicomData;
            m_dicomData = NULL;
        }

        m_dicomData =  dicomFile.getAndRemoveDataset();
    }
    else
    {
        DEBUG_LOG( QString( "Error en llegir l'arxiu [%1]\nPossible causa: %2 ").arg( filename ).arg( status.text() ) );
        ERROR_LOG( QString( "Error en llegir l'arxiu [%1]\nPossible causa: %2 ").arg( filename ).arg( status.text() ) );
        return false;
    }

    return true;
}

QString DICOMTagReader::getFileName() const
{
    return m_filename;
}

void DICOMTagReader::setDcmDataset(QString filename, DcmDataset *dcmDataset)
{
    Q_ASSERT(dcmDataset);

    m_filename = filename;

    if (m_dicomData)
    {
        delete m_dicomData;
    }

    m_dicomData = dcmDataset;
}

bool DICOMTagReader::tagExists( DICOMTag tag )
{
    if( m_dicomData )
        return m_dicomData->tagExists( DcmTagKey(tag.getGroup(),tag.getElement()) );
    else
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat");
        return false;
    }
}

bool DICOMTagReader::tagExists( DcmTagKey tag )
{
    DEBUG_LOG("Aquest mètode està deprecated. S'eliminarà en breu, fer servir DICOMTag, no DcmTagKey");
    return tagExists( DICOMTag(tag.getGroup(),tag.getElement()) );
}

bool DICOMTagReader::tagExists( unsigned int group, unsigned int element )
{
    return this->tagExists( DcmTagKey(group,element) );
}

QString DICOMTagReader::getAttributeByTag( unsigned int group, unsigned int element )
{
    return getAttributeByName( DICOMTag(group,element) );
}

QString DICOMTagReader::getAttributeByName( DICOMTag tag )
{
    if( !m_dicomData )
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat. Tornem string buida.");
        return QString();
    }

    // Convertim DICOMTag al format de dcmtk
    DcmTagKey dcmtkTag(tag.getGroup(),tag.getElement());
    QString result;

    OFString value;
    OFCondition status = m_dicomData->findAndGetOFStringArray( dcmtkTag, value );
    if( status.good() )
    {
        result = value.c_str();
    }
    else
    {
        if (QString(status.text()) != "Tag Not Found")
        {
            DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2")
                .arg( dcmtkTag.toString().c_str() ).arg( status.text() ) );
        }
    }

    return result;
}

QString DICOMTagReader::getAttributeByName( DcmTagKey tag )
{
    DEBUG_LOG("Aquest mètode està deprecated. S'eliminarà en breu, fer servir DICOMTag, no DcmTagKey");
    return getAttributeByName( DICOMTag(tag.getGroup(),tag.getElement()) );
}

QStringList DICOMTagReader::getSequenceAttributeByTag( unsigned int sequenceGroup, unsigned int sequenceElement, unsigned int group, unsigned int element )
{
    return this->getSequenceAttributeByName( DcmTagKey(sequenceGroup,sequenceElement) , DcmTagKey(group,element) );
}

QStringList DICOMTagReader::getSequenceAttributeByName( DICOMTag sequenceTag, DICOMTag attributeTag )
{
    if( !m_dicomData )
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat. Tornem string-list buida.");
        return QStringList(); 
    }
    // Convertim els DICOMTag al format de dcmtk, que serà qui farà la feina
    DcmTagKey dcmtkSequenceTag( sequenceTag.getGroup(), sequenceTag.getElement() );
    DcmTagKey dcmtkAttributeTag( attributeTag.getGroup(), attributeTag.getElement() );
    QStringList result;
    // obtenim els atributs de cada item d'una seqüència de "primer nivell"
    DcmStack stack;

    OFCondition status = m_dicomData->search( dcmtkSequenceTag, stack );

    if( status.good() )
    {
        OFString value;
        DcmSequenceOfItems *sequence = NULL;
        sequence = OFstatic_cast( DcmSequenceOfItems *,stack.top() );
        for(unsigned int i = 0; i < sequence->card(); i++ )
        {
            DcmItem *item = sequence->getItem( i );
            status = item->findAndGetOFStringArray( dcmtkAttributeTag , value );
            if( status.good() )
            {
                result << value.c_str();
            }
            else if( QString(status.text()) != "Tag Not Found" )
            {
                DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg( dcmtkAttributeTag.toString().c_str() ).arg( status.text() ) );
            }
        }
    }
    else if( QString(status.text()) != "Tag Not Found" )
        DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg( dcmtkSequenceTag.toString().c_str() ).arg( status.text() ) );

    return result;
// \TODO el que ve a continuació és com hauria de ser quan s'implementi el mètode amb seqüències "embedded"
//     QList<DcmTagKey> embeddedSequenceList;
//     embeddedSequenceList << sequenceTag;
//     return this->getSequenceAttributeByName( embeddedSequenceList, attributeTag );
}

QStringList DICOMTagReader::getSequenceAttributeByName( DcmTagKey sequenceTag, DcmTagKey attributeTag )
{
    DEBUG_LOG("Aquest mètode està deprecated. S'eliminarà en breu, fer servir DICOMTag, no DcmTagKey");
    return getSequenceAttributeByName( DICOMTag(sequenceTag.getGroup(), sequenceTag.getElement()), DICOMTag(attributeTag.getGroup(),attributeTag.getElement()) );
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

QStringList DICOMTagReader::getSequenceAttributeByName( QList<DICOMTag> embeddedSequencesTags, DICOMTag attributeTag )
{
    Q_UNUSED(embeddedSequencesTags);
    Q_UNUSED(attributeTag);
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

QStringList DICOMTagReader::getSequenceAttributeByName( QList<DcmTagKey> embeddedSequencesTags, DcmTagKey attributeTag )
{
    DEBUG_LOG("Aquest mètode està deprecated. S'eliminarà en breu, fer servir DICOMTag, no DcmTagKey");
    // Convertim els paràmetres a DICOMTag
    QList<DICOMTag>  tagList;
    foreach( DcmTagKey tag, embeddedSequencesTags )
    {
        tagList << DICOMTag(tag.getGroup(),tag.getElement());
    }
    
    return getSequenceAttributeByName( tagList, DICOMTag(attributeTag.getGroup(),attributeTag.getElement()) );
}

}
