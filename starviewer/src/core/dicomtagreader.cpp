/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomtagreader.h"

#include "logging.h"
#include "dicomsequenceattribute.h"
#include "dicomattribute.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceitem.h"
// Qt
#include <QStringList>
// dcmtk
#include <dcfilefo.h>
#include <dcdeftag.h>
#include <dcmetinf.h>

namespace udg {

DICOMTagReader::DICOMTagReader() : m_dicomData(0), m_dicomHeader(0), m_hasValidFile(false)
{
}

DICOMTagReader::DICOMTagReader(const QString &filename, DcmDataset *dcmDataset) : m_dicomData(0), m_dicomHeader(0), m_hasValidFile(false)
{
    this->setDcmDataset(filename, dcmDataset);
}

DICOMTagReader::DICOMTagReader(const QString &filename) : m_dicomData(0), m_dicomHeader(0), m_hasValidFile(false)
{
    this->setFile(filename);
}

DICOMTagReader::~DICOMTagReader()
{
    deleteDataLastLoadedFile();
}

void DICOMTagReader::deleteDataLastLoadedFile()
{
    if (m_dicomData)
    {
        delete m_dicomData;
        m_dicomData = NULL;
    }
    if (m_dicomHeader)
    {
        delete m_dicomHeader;
        m_dicomHeader = NULL;
    }
}

bool DICOMTagReader::setFile(const QString &filename)
{
    DcmFileFormat dicomFile;

    m_filename = filename;

    OFCondition status = dicomFile.loadFile(qPrintable(filename));
    if (status.good())
    {
        m_hasValidFile = true;
        // eliminem l'objecte anterior si n'hi hagués
        deleteDataLastLoadedFile();

        m_dicomHeader = new DcmMetaInfo(*dicomFile.getMetaInfo());
        m_dicomData = dicomFile.getAndRemoveDataset();
    }
    else
    {
        m_hasValidFile = false;
        DEBUG_LOG(QString("Error en llegir l'arxiu [%1]\nPossible causa: %2 ").arg(filename).arg(status.text()));
        ERROR_LOG(QString("Error en llegir l'arxiu [%1]\nPossible causa: %2 ").arg(filename).arg(status.text()));
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

    deleteDataLastLoadedFile();

    m_dicomData = dcmDataset;
}

DcmDataset* DICOMTagReader::getDcmDataset() const
{
    return m_dicomData;
}

bool DICOMTagReader::tagExists(const DICOMTag &tag) const
{
    if (m_dicomData)
    {
        return m_dicomData->tagExists(DcmTagKey(tag.getGroup(),tag.getElement()));
    }
    else
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat");
        return false;
    }
}

QString DICOMTagReader::getValueAttributeAsQString(const DICOMTag &tag) const
{
    if (!m_dicomData)
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat. Tornem string buida.");
        return QString();
    }

    // Convertim DICOMTag al format de dcmtk
    DcmTagKey dcmtkTag(tag.getGroup(),tag.getElement());
    QString result;

    OFString value;
    OFCondition status = m_dicomData->findAndGetOFStringArray(dcmtkTag, value);
    if (status.good())
    {
        result = value.c_str();
    }
    else
    {
        if (QString(status.text()) != "Tag Not Found")
        {
            DEBUG_LOG(QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2")
                .arg(dcmtkTag.toString().c_str()).arg(status.text()));
        }
    }

    return result;
}

DICOMSequenceAttribute* DICOMTagReader::getSequenceAttribute(const DICOMTag &sequenceTag, DICOMTagReader::ReturnValueOfTags returnValueOfTags) const
{
    if (!m_dicomData)
    {
        DEBUG_LOG("No hi ha cap m_dicomData (DcmDataset) carregat. Tornem string-list buida.");
        return NULL; 
    }
    // Convertim els DICOMTag al format de dcmtk, que serà qui farà la feina
    DcmTagKey dcmtkSequenceTag(sequenceTag.getGroup(), sequenceTag.getElement());
    
    QStringList result;
    // obtenim els atributs de cada item d'una seqüència de "primer nivell"
    DcmSequenceOfItems *sequence = NULL;

    OFCondition status = m_dicomData->findAndGetSequence(dcmtkSequenceTag, sequence, OFTrue);

    if (status.good())
    {
        DEBUG_LOG(QString("Cerquem sequencia"));
        return convertToDICOMSequenceAttribute(sequence, returnValueOfTags);
    }
    else if (QString(status.text()) != "Tag Not Found")
    {
        DEBUG_LOG(QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg(dcmtkSequenceTag.toString().c_str()).arg(status.text()));
    }

    return NULL;
}

DICOMSequenceAttribute* DICOMTagReader::convertToDICOMSequenceAttribute(DcmSequenceOfItems *dcmtkSequence, DICOMTagReader::ReturnValueOfTags returnValueOfTags) const
{
    DICOMSequenceAttribute *sequenceAttribute = new DICOMSequenceAttribute();

    sequenceAttribute->setTag(DICOMTag(dcmtkSequence->getGTag(),dcmtkSequence->getETag()));

    for (unsigned int i = 0; i < dcmtkSequence->card(); i++)
    {
        DICOMSequenceItem *dicomItem = new DICOMSequenceItem();
        DcmItem *dcmtkItem = dcmtkSequence->getItem(i);
        for (unsigned int j = 0; j < dcmtkItem->card(); j++)
        {
            DcmElement *element = dcmtkItem->getElement(j);
            
            if (!element->isLeaf()) // És una Sequence of Items
            {
                dicomItem->addAttribute(convertToDICOMSequenceAttribute(OFstatic_cast(DcmSequenceOfItems*,element), returnValueOfTags));
            }
            else 
            {
                DICOMValueAttribute *dicomValueAttribute = convertToDICOMValueAttribute(element, returnValueOfTags);
            
                if (dicomValueAttribute != NULL)
                {
                    dicomItem->addAttribute(dicomValueAttribute);
                }
            }
        }
        sequenceAttribute->addItem(dicomItem);
    }

    return sequenceAttribute;
}

DICOMValueAttribute* DICOMTagReader::convertToDICOMValueAttribute(DcmElement *dcmtkDICOMElement, DICOMTagReader::ReturnValueOfTags returnValueOfTags) const
{
    DICOMValueAttribute *dicomValueAttribute = new DICOMValueAttribute();
    dicomValueAttribute->setTag(DICOMTag(dcmtkDICOMElement->getGTag(), dcmtkDICOMElement->getETag()));


    if (returnValueOfTags != DICOMTagReader::ExcludeHeavyTags ||  
        (dcmtkDICOMElement->getTag() != DcmTag(DCM_PixelData) && dcmtkDICOMElement->getTag() != DcmTag(DCM_OverlayData)))
    {
        OFString value;
        OFCondition status = dcmtkDICOMElement->getOFStringArray(value);

        if (status.good())
        {
            dicomValueAttribute->setValue(QString(value.c_str()));
        }
        else if (QString(status.text()) != "Tag Not Found")
        {
            dicomValueAttribute->setValue(QString("Unreadable tag value: %1").arg(status.text()));
            INFO_LOG(QString("S'ha produit el seguent problema a l'intentar obtenir el tag %1 :: %2").arg(dcmtkDICOMElement->getTag().toString().c_str() ).arg(status.text()));
        }
    }

    return dicomValueAttribute;
}

QList<DICOMAttribute*> DICOMTagReader::convertToDICOMAttributeQList(DcmItem *dcmItem, DICOMTagReader::ReturnValueOfTags returnValueOfTags) const
{
    QList<DICOMAttribute*> attributeList;
    DcmElement *currentElement = NULL;

    for (unsigned int i = 0; i < dcmItem->card(); i++)
    {
        currentElement = OFstatic_cast(DcmElement*, dcmItem->nextInContainer(currentElement));

        // Es tracta d'una seqüència
        if (!currentElement->isLeaf())
        {
            DICOMSequenceAttribute *dicomSequenceAttribute = convertToDICOMSequenceAttribute(OFstatic_cast(DcmSequenceOfItems*, currentElement), returnValueOfTags);
            attributeList.append(dicomSequenceAttribute);
        }
        else
        {
            DICOMValueAttribute *dicomValueAttribute = convertToDICOMValueAttribute(currentElement, returnValueOfTags);
            
            if (dicomValueAttribute != NULL)
            {
                attributeList.append(dicomValueAttribute);
            }
        }
    }

    return attributeList;
}
QList<DICOMAttribute*> DICOMTagReader::getDICOMDataSet(DICOMTagReader::ReturnValueOfTags returnValueOfTags) const
{
    return convertToDICOMAttributeQList(m_dicomData, returnValueOfTags);
}

QList<DICOMAttribute*> DICOMTagReader::getDICOMHeader() const
{
    if (m_dicomHeader)
    {
        return convertToDICOMAttributeQList(m_dicomHeader, DICOMTagReader::AllTags);
    }
    else
    {
        return QList<DICOMAttribute*>();
    }
}

}