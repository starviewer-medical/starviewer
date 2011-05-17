#include "hangingprotocolxmlreader.h"

#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include "logging.h"
// Qt's
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QFileInfoList>

namespace udg {

HangingProtocolXMLReader::HangingProtocolXMLReader(QObject *parent)
 : QObject(parent)
{
}

HangingProtocolXMLReader::~HangingProtocolXMLReader()
{
}

HangingProtocol* HangingProtocolXMLReader::readFile(const QString &path)
{
    QFile file(path);
    HangingProtocol *hangingProtocolLoaded = NULL;

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(0, tr("Hanging protocol XML File"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return NULL;
    }

    QXmlStreamReader *reader = new QXmlStreamReader(&file);

    if (reader->readNextStartElement())
    {
        if (reader->name() == "hangingProtocol")
        {
            HangingProtocol *hangingProtocol = new HangingProtocol();
            QList<QString> protocols;
            QList<HangingProtocolImageSet::Restriction> restrictionList;

            while (reader->readNextStartElement())
            {
                if (reader->name() == "hangingProtocolName")
                {
                    hangingProtocol->setName(reader->readElementText());
                }
                else if (reader->name() == "numberScreens")
                {
                    hangingProtocol->setNumberOfScreens(reader->readElementText().toInt());
                }
                else if (reader->name() == "protocol")
                {
                    protocols << reader->readElementText();
                }
                else if (reader->name() == "restriction")
                {
                    restrictionList << readRestriction(reader);
                }
                else if (reader->name() == "imageSet")
                {
                    HangingProtocolImageSet *imageSet = readImageSet(reader, restrictionList);
                    hangingProtocol->addImageSet(imageSet);
                }
                else if (reader->name() == "displaySet")
                {
                    HangingProtocolDisplaySet *displaySet = readDisplaySet(reader, hangingProtocol);
                    hangingProtocol->addDisplaySet(displaySet);
                }
                else if (reader->name() == "strictness")
                {
                    hangingProtocol->setStrictness(reader->readElementText().contains("yes"));
                }
                else if (reader->name() == "allDifferent")
                {
                    hangingProtocol->setAllDiferent(reader->readElementText().contains("yes"));
                }
                else if (reader->name() == "iconType")
                {
                    hangingProtocol->setIconType(reader->readElementText());
                }
                else if (reader->name() == "hasPrevious")
                {
                    hangingProtocol->setPrevious(reader->readElementText().contains("yes"));
                }
                else if (reader->name() == "priority")
                {
                    hangingProtocol->setPriority(reader->readElementText().toDouble());
                }
                else
                {
                    reader->skipCurrentElement();
                }
            }

            if (!reader->hasError())
            {
                hangingProtocol->setProtocolsList(protocols);
                hangingProtocolLoaded = hangingProtocol;
            }
            else
            {
                delete hangingProtocol;
            }
        }
    }

    if (reader->hasError())
    {
        DEBUG_LOG(QString("[Line: %1, Column:%2] Error in hanging protocol file %3: %4, error: %5").arg(reader->lineNumber()).arg(reader->columnNumber()).arg(path).arg(reader->errorString()).arg(reader->error()));
        ERROR_LOG(QString("[Line: %1, Column:%2] Error in hanging protocol file %3: %4, error: %5").arg(reader->lineNumber()).arg(reader->columnNumber()).arg(path).arg(reader->errorString()).arg(reader->error()));
    }

    delete reader;

    return hangingProtocolLoaded;
}

HangingProtocolImageSet::Restriction HangingProtocolXMLReader::readRestriction(QXmlStreamReader *reader)
{
    HangingProtocolImageSet::Restriction restriction;

    while (reader->readNextStartElement())
    {
        if (reader->name() == "usageFlag")
        {
            QString text = reader->readElementText();
            if (text == "MATCH")
            {
                restriction.usageFlag = HangingProtocolImageSet::Match;
            }
            else if (text == "NO_MATCH")
            {
                restriction.usageFlag = HangingProtocolImageSet::NoMatch;
            }
        }
        else if (reader->name() == "selectorAttribute")
        {
            restriction.selectorAttribute = reader->readElementText();
        }
        else if (reader->name() == "valueRepresentation")
        {
            restriction.valueRepresentation = reader->readElementText();
        }
        else
        {
            // Saltem l'element perquè no és conegut.
            reader->skipCurrentElement();
        }

    }
    return restriction;
}

HangingProtocolImageSet* HangingProtocolXMLReader::readImageSet(QXmlStreamReader *reader, const QList<HangingProtocolImageSet::Restriction> &restrictionList)
{
    HangingProtocolImageSet *imageSet = new HangingProtocolImageSet();
    imageSet->setIdentifier(reader->attributes().value("identifier").toString().toInt());

    while (reader->readNextStartElement())
    {
        if (reader->name() == "restriction")
        {
            HangingProtocolImageSet::Restriction restriction = restrictionList.value(reader->readElementText().toInt()-1);
            imageSet->addRestriction(restriction);
        }
        else if (reader->name() == "type")
        {
            imageSet->setTypeOfItem(reader->readElementText());
        }
        else if (reader->name() == "previous")
        {
            QString previousText = reader->readElementText();
            bool isPrevious = !(previousText.contains("no"));
            imageSet->setIsPreviousStudy(isPrevious);

            if (isPrevious)
            {
                imageSet->setPreviousImageSetReference(previousText.toInt());
            }
        }
        else if (reader->name() == "imageNumberInPatientModality")
        {
            imageSet->setImageNumberInPatientModality(reader->readElementText().toInt());
        }
        else
        {
            // Saltem l'element perquè no és conegut.
            reader->skipCurrentElement();
        }
    }
    return imageSet;
}

HangingProtocolDisplaySet* HangingProtocolXMLReader::readDisplaySet(QXmlStreamReader *reader, HangingProtocol *hangingProtocol)
{

    HangingProtocolDisplaySet *displaySet = new HangingProtocolDisplaySet();
    displaySet->setIdentifier(reader->attributes().value("identifier").toString().toInt());

    while (reader->readNextStartElement())
    {
        if (reader->name() == "imageSetNumber")
        {
            HangingProtocolImageSet *imageSet = hangingProtocol->getImageSet(reader->readElementText().toInt());
            if (imageSet)
            {
                displaySet->setImageSet(imageSet);
            }
            else
            {
                ERROR_LOG(QString("No s'ha trobat l'image set requerit pel display set %1.").arg(displaySet->getIdentifier()));
            }
        }
        else if (reader->name() == "position")
        {
            displaySet->setPosition(reader->readElementText());
        }
        else if (reader->name() == "patientOrientation")
        {
            displaySet->setPatientOrientation(reader->readElementText());
        }
        else if (reader->name() == "reconstruction")
        {
            displaySet->setReconstruction(reader->readElementText());
        }
        else if (reader->name() == "phase")
        {
            displaySet->setPhase(reader->readElementText().toInt());
        }
        else if (reader->name() == "imageNumber")
        {
            displaySet->setSlice(reader->readElementText().toInt());
        }
        else if (reader->name() == "iconType")
        {
            displaySet->setIconType(reader->readElementText());
        }
        else if (reader->name() == "alignment")
        {
            displaySet->setAlignment(reader->readElementText());
        }
        else if (reader->name() == "toolActivation")
        {
            displaySet->setToolActivation(reader->readElementText());
        }
        else if (reader->name() == "windowLevel")
        {
            while (reader->readNextStartElement())
            {
                if (reader->name() == "width")
                {
                    displaySet->setWindowWidth(reader->readElementText().toDouble());
                }
                else if (reader->name() == "center")
                {
                    displaySet->setWindowCenter(reader->readElementText().toDouble());
                }
            }
        }
        else
        {
            // Saltem l'element perquè no és conegut.
            reader->skipCurrentElement();
        }
    }
    return displaySet;
}
}
