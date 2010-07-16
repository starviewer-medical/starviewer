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

namespace udg {

DICOMTagReader::DICOMTagReader() : m_dicomData(0), m_hasValidFile(false)
{
}

DICOMTagReader::DICOMTagReader(const QString &filename, DcmDataset *dcmDataset) : m_dicomData(0), m_hasValidFile(false)
{
    this->setDcmDataset(filename, dcmDataset);
}

DICOMTagReader::DICOMTagReader(const QString &filename) : m_dicomData(0), m_hasValidFile(false)
{
    this->setFile(filename);
}

DICOMTagReader::~DICOMTagReader()
{
    if (m_dicomData)
    {
        delete m_dicomData;
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
        if (m_dicomData)
        {
            delete m_dicomData;
            m_dicomData = NULL;
        }

        m_dicomData =  dicomFile.getAndRemoveDataset();
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
    if (m_dicomData)
    {
        delete m_dicomData;
    }

    m_dicomData = dcmDataset;
}

DcmDataset* DICOMTagReader::getDcmDataset() const
{
    return m_dicomData;
}

bool DICOMTagReader::tagExists(const DICOMTag &tag)
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

QString DICOMTagReader::getValueAttributeAsQString(const DICOMTag &tag)
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

DICOMSequenceAttribute* DICOMTagReader::getSequenceAttribute(const DICOMTag &sequenceTag)
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
        return convertToDICOMSequenceAttribute(sequence);
    }
    else if (QString(status.text()) != "Tag Not Found")
    {
        DEBUG_LOG(QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg(dcmtkSequenceTag.toString().c_str()).arg(status.text()));
    }

    return NULL;
}

DICOMSequenceAttribute* DICOMTagReader::convertToDICOMSequenceAttribute(DcmSequenceOfItems *dcmtkSequence)
{
    DICOMSequenceAttribute *sequenceAttribute = new DICOMSequenceAttribute();
    DcmVR sequenceVR("SQ");

    sequenceAttribute->setTag(DICOMTag(dcmtkSequence->getGTag(),dcmtkSequence->getETag()));

    for (unsigned int i = 0; i < dcmtkSequence->card(); i++)
    {
        DICOMSequenceItem *dicomItem = new DICOMSequenceItem();
        DcmItem *dcmtkItem = dcmtkSequence->getItem(i);
        for (unsigned int j = 0; j < dcmtkItem->card(); j++)
        {
            DcmElement *element = dcmtkItem->getElement(j);
            
            
            if (sequenceVR.isEquivalent(element->getTag().getVR())) // És una Sequence of Items
            {
                dicomItem->addAttribute(convertToDICOMSequenceAttribute(OFstatic_cast(DcmSequenceOfItems*,element)));
            }
            else 
            {
                OFString value;
                OFCondition status = element->getOFStringArray(value);

                if (status.good())
                {
                    DICOMTag tag(element->getGTag(),element->getETag());
                    
                    DICOMValueAttribute *valueAttribute = new DICOMValueAttribute();
                    valueAttribute->setTag(tag);
                    valueAttribute->setValue(QString(value.c_str()));

                    dicomItem->addAttribute(valueAttribute);
                }
                else if (QString(status.text()) != "Tag Not Found")
                {
                    DEBUG_LOG(QString("S'ha produit el següent problema a l'intentar obtenir el tag %1 :: %2").arg(element->getTag().toString().c_str() ).arg(status.text()));
                }
            }
        }
        sequenceAttribute->addItem(dicomItem);
    }

    return sequenceAttribute;
}

QList<DICOMAttribute*> DICOMTagReader::getDICOMAttributes()
{
    QList<DICOMAttribute*> attributeList;
    DcmElement *currentElement = NULL;

    for (unsigned int i = 0; i < m_dicomData->card(); i++)
    {
        currentElement = OFstatic_cast(DcmElement*, m_dicomData->nextInContainer(currentElement));

        // Es tracta d'una seqüència
        if (!currentElement->isLeaf())
        {
            DICOMSequenceAttribute *dicomSequenceAttribute = convertToDICOMSequenceAttribute(OFstatic_cast(DcmSequenceOfItems*, currentElement));
            attributeList.append(dicomSequenceAttribute);
        }
        else
        {
            OFString value;
            currentElement->getOFStringArray(value);

            DICOMTag tag(currentElement->getGTag(), currentElement->getETag());

            DICOMValueAttribute *dicomValueAttribute = new DICOMValueAttribute();
            dicomValueAttribute->setTag(tag);
            dicomValueAttribute->setValue(QString(value.c_str()));

            attributeList.append(dicomValueAttribute);
        }
    }
    return attributeList;
}

}

