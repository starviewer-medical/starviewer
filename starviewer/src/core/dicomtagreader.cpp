/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomtagreader.h"

#include "logging.h"
#include "dicomsequenceattribute.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceitem.h"
#include "dicomreferencedimage.h"
// Qt
#include <QStringList>
// dcmtk
#include <dcfilefo.h>
#include <dsrdoc.h>
#include <dsrimgtn.h>

// TODO Cal utilitzar aquest template per problemes de compilacio al utilitzar DSRListOfItems cal descobrir el perque
template<> const Sint32 DSRListOfItems<Sint32>::EmptyItem(0);

namespace udg {

DICOMTagReader::DICOMTagReader() : m_dicomData(0), m_hasValidFile(false), m_DICOMStructuredReportDocument(0)
{
}

DICOMTagReader::DICOMTagReader(const QString &filename, DcmDataset *dcmDataset) : m_dicomData(0), m_hasValidFile(false), m_DICOMStructuredReportDocument(0)
{
    this->setDcmDataset(filename, dcmDataset);
}

DICOMTagReader::DICOMTagReader( const QString &filename ) : m_dicomData(0), m_hasValidFile(false), m_DICOMStructuredReportDocument(0)
{
    this->setFile( filename );
}

DICOMTagReader::~DICOMTagReader()
{
    if( m_dicomData )
        delete m_dicomData;
}

bool DICOMTagReader::setFile( const QString &filename )
{
    DcmFileFormat dicomFile;

    m_filename = filename;

    OFCondition status = dicomFile.loadFile( qPrintable(filename) );
    if( status.good() )
    {
        m_hasValidFile = true;
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
        m_hasValidFile = false;
        DEBUG_LOG( QString( "Error en llegir l'arxiu [%1]\nPossible causa: %2 ").arg( filename ).arg( status.text() ) );
        ERROR_LOG( QString( "Error en llegir l'arxiu [%1]\nPossible causa: %2 ").arg( filename ).arg( status.text() ) );
        return false;
    }

    return true;
}

bool DICOMTagReader::canReadFile() const
{
    return m_hasValidFile;
}

QString DICOMTagReader::getFileName() const
{
    return m_filename;
}

void DICOMTagReader::setDcmDataset(const QString &filename, DcmDataset *dcmDataset)
{
    Q_ASSERT(dcmDataset);

    m_filename = filename;
    // Assumim que sempre serà un fitxer vàlid. 
    /// TODO Cal fer alguna validació en aquests casos? 
    // Potser en aquests casos el filename hauria de ser sempre buit ja que així expressem
    // explícitament que llegim un element de memòria
    m_hasValidFile = true;
    if (m_dicomData)
    {
        delete m_dicomData;
    }

    m_dicomData = dcmDataset;
}

DcmDataset *DICOMTagReader::getDcmDataset() const
{
    return m_dicomData;
}

bool DICOMTagReader::tagExists( const DICOMTag &tag )
{
    if( m_dicomData )
        return m_dicomData->tagExists( DcmTagKey(tag.getGroup(),tag.getElement()) );
    else
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat");
        return false;
    }
}

QString DICOMTagReader::getValueAttributeAsQString( const DICOMTag &tag )
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

DICOMSequenceAttribute * DICOMTagReader::getSequenceAttribute( const DICOMTag &sequenceTag )
{
    
    if( !m_dicomData )
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat. Tornem string-list buida.");
        return NULL; 
    }
    // Convertim els DICOMTag al format de dcmtk, que serà qui farà la feina
    DcmTagKey dcmtkSequenceTag( sequenceTag.getGroup(), sequenceTag.getElement() );
    
    QStringList result;
    // obtenim els atributs de cada item d'una seqüència de "primer nivell"
    DcmSequenceOfItems *sequence = NULL;

    OFCondition status = m_dicomData->findAndGetSequence( dcmtkSequenceTag, sequence, OFTrue );

    if( status.good() )
    {
        DEBUG_LOG( QString("Cerquem sequencia") );
        return convertToDICOMSequenceAttribute( sequence );
    }
    else if( QString(status.text()) != "Tag Not Found" )
        DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg( dcmtkSequenceTag.toString().c_str() ).arg( status.text() ) );
        

    return NULL;
}

DICOMSequenceAttribute * DICOMTagReader::convertToDICOMSequenceAttribute( DcmSequenceOfItems * dcmtkSequence )
{
    DICOMSequenceAttribute * sequenceAttribute = new DICOMSequenceAttribute();
    DcmVR sequenceVR("SQ");

    sequenceAttribute->setTag( DICOMTag(dcmtkSequence->getGTag(),dcmtkSequence->getETag()) );

    for(unsigned int i = 0; i < dcmtkSequence->card(); i++ )
    {
        DICOMSequenceItem * dicomItem = new DICOMSequenceItem();
        DcmItem *dcmtkItem = dcmtkSequence->getItem( i );
        for(unsigned int j = 0; j < dcmtkItem->card() ; j++ )
        {
            DcmElement *element = dcmtkItem->getElement( j );
            
            
            if( sequenceVR.isEquivalent( element->getTag().getVR() ) ) // És una Sequence of Items
            {
                dicomItem->addAttribute( convertToDICOMSequenceAttribute( OFstatic_cast(DcmSequenceOfItems *,element) ) );
            }
            else 
            {
                OFString value;
                OFCondition status = element->getOFStringArray( value );

                if( status.good() )
                {
                    DICOMTag tag(element->getGTag(),element->getETag());
                    
                    DICOMValueAttribute * valueAttribute = new DICOMValueAttribute();
                    valueAttribute->setTag( tag );
                    valueAttribute->setValue( QString( value.c_str() ) );

                    dicomItem->addAttribute( valueAttribute );
                }
                else if( QString(status.text()) != "Tag Not Found" )
                {
                    DEBUG_LOG( QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg( element->getTag().toString().c_str()  ).arg( status.text() ) );
                }
            }
        }
        sequenceAttribute->addItem( dicomItem );
    }

    return sequenceAttribute;
}

bool DICOMTagReader::existStructuredReportNode(const QString &codeValue, const QString &codeMeaning, const QString &schemeDesignator)
{
    DSRDocumentTree &tree = getStructuredReportDocument()->getTree();
    return tree.gotoNamedNode(DSRCodedEntryValue(qPrintable(codeValue), qPrintable(schemeDesignator), qPrintable(codeMeaning)));
}

QString DICOMTagReader::getStructuredReportNodeContent(const QString &codeValue, const QString &codeMeaning, const QString &schemeDesignator)
{
    DSRDocumentTree &tree = getStructuredReportDocument()->getTree();
    tree.gotoNamedNode(DSRCodedEntryValue(qPrintable(codeValue), qPrintable(schemeDesignator), qPrintable(codeMeaning)));

    return tree.getCurrentContentItem().getStringValue().c_str();
}

DSRDocument* DICOMTagReader::getStructuredReportDocument()
{
    if (!m_DICOMStructuredReportDocument)
    {
        m_DICOMStructuredReportDocument = new DSRDocument();
        OFCondition status = m_DICOMStructuredReportDocument->read(*getDcmDataset(), DSRTypes::RF_verboseDebugMode);

        if (status.bad())
        {
            ERROR_LOG(QString("S'ha produit un error al llegir el DSRDocument: " + QString(status.text())) );
        }
    }

    return m_DICOMStructuredReportDocument;
}

QList<DICOMReferencedImage*> DICOMTagReader::getDICOMReferencedImagesOfStructedReport()
{
    // TODO Aquest metode no esta al lloc mes correcte, caldria posar-lo a una altra classe (pendent implementacio)
    QList<DICOMReferencedImage*> referencedImagesOfStructuredReport;

    DSRDocumentTree &structuredReportTree = getStructuredReportDocument()->getTree();
    structuredReportTree.gotoRoot();
    DSRTreeNodeCursor cursor(structuredReportTree.getNode());

    if (cursor.isValid())
    {
        while (cursor.iterate())
        {
            DSRDocumentTreeNode *structuredReportTreeNode = OFstatic_cast(DSRDocumentTreeNode*, cursor.getNode());
            
            if (structuredReportTreeNode && structuredReportTreeNode->getValueType() == DSRTypes::VT_Image)
            {
                foreach (DICOMReferencedImage *referencedImage, getDicomReferencedImagesFromTreeNode(structuredReportTreeNode))
                {
                    referencedImagesOfStructuredReport.append(referencedImage);
                }
            }
        }
    }

    return referencedImagesOfStructuredReport;
}

QList<DICOMReferencedImage*> DICOMTagReader:: getDicomReferencedImagesFromTreeNode(DSRDocumentTreeNode *structuredReportTreeNode)
{
     QList <DICOMReferencedImage*> referencedImagesOfTreeNode;

    if (structuredReportTreeNode->getValueType() == DSRTypes::VT_Image)
    {
        DSRImageTreeNode *imageNode = OFstatic_cast(DSRImageTreeNode*, structuredReportTreeNode);
        OFString SOPInstance = imageNode->getSOPInstanceUID();
        
        DSRImageFrameList &framesList  = imageNode->getFrameList();
        
        if (!framesList.isEmpty()) // Cas multiframe
        {
            for (size_t i = 0; i < framesList.getNumberOfItems(); i++)
            {
                DICOMReferencedImage *referencedImage = new DICOMReferencedImage();
                int frameNumber = static_cast<int>(framesList.getItem(i));
                referencedImage->setDICOMReferencedImageSOPInstanceUID(SOPInstance.c_str());
                referencedImage->setFrameNumber(frameNumber);
                referencedImagesOfTreeNode.append(referencedImage);
            }
        }
        else
        {
            DICOMReferencedImage *referencedimage = new DICOMReferencedImage();
            referencedimage->setDICOMReferencedImageSOPInstanceUID(SOPInstance.c_str());
            referencedImagesOfTreeNode.append(referencedimage);
        }
    }

    return referencedImagesOfTreeNode;
}

QString DICOMTagReader::getStructuredReportCodeValueOfContentItem(const QString &codeValue, const QString &codeMeaning, const QString &schemeDesignator)
{
    DSRDocumentTree &structuredReportTree = getStructuredReportDocument()->getTree();
    structuredReportTree.gotoNamedNode(DSRCodedEntryValue(qPrintable(codeValue), qPrintable(schemeDesignator), qPrintable(codeMeaning)));

    DSRCodedEntryValue code = structuredReportTree.getCurrentContentItem().getCodeValue();

    return code.getCodeValue().c_str();
}

}
