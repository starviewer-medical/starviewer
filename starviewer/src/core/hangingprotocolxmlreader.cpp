/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "hangingprotocolxmlreader.h"

#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocolimagesetrestriction.h"
#include "logging.h"
#include "patientorientation.h"
// Qt's
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <QFileInfoList>
#include <QRegExp>

namespace udg {

HangingProtocolXMLReader::HangingProtocolXMLReader()
{
}

HangingProtocolXMLReader::~HangingProtocolXMLReader()
{
}

HangingProtocol* HangingProtocolXMLReader::readFile(const QString &path)
{
    QFile file(path);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(0, QObject::tr("Hanging Protocol XML File"),
                             QObject::tr("Unable to read file %1:\n%2.")
                             .arg(path)
                             .arg(file.errorString()));
        return NULL;
    }

    return read(&file);
}

HangingProtocol* HangingProtocolXMLReader::read(QIODevice *device)
{
    m_xmlReader.setDevice(device);
    HangingProtocol *hangingProtocolLoaded = NULL;

    if (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == "hangingProtocol" && m_xmlReader.attributes().value("version") == "2")
        {
            hangingProtocolLoaded = readHangingProtocol();
        }
        else
        {
            m_xmlReader.raiseError(QObject::tr("Unsupported hanging protocol version: %1").arg(m_xmlReader.attributes().value("version").toString()));
        }
    }

    if (m_xmlReader.hasError())
    {
        QFileDevice *fileDevice = qobject_cast<QFileDevice*>(device);

        if (fileDevice)
        {
            DEBUG_LOG(QString("[Line: %1, Column:%2] Error in hanging protocol file %3: %4, error: %5").arg(m_xmlReader.lineNumber())
                      .arg(m_xmlReader.columnNumber()).arg(fileDevice->fileName()).arg(m_xmlReader.errorString()).arg(m_xmlReader.error()));
            ERROR_LOG(QString("[Line: %1, Column:%2] Error in hanging protocol file %3: %4, error: %5").arg(m_xmlReader.lineNumber())
                      .arg(m_xmlReader.columnNumber()).arg(fileDevice->fileName()).arg(m_xmlReader.errorString()).arg(m_xmlReader.error()));
        }
        else
        {
            DEBUG_LOG(QString("[Line: %1, Column:%2] Error in hanging protocol: %3, error: %4").arg(m_xmlReader.lineNumber())
                      .arg(m_xmlReader.columnNumber()).arg(m_xmlReader.errorString()).arg(m_xmlReader.error()));
            ERROR_LOG(QString("[Line: %1, Column:%2] Error in hanging protocol: %3, error: %4").arg(m_xmlReader.lineNumber())
                      .arg(m_xmlReader.columnNumber()).arg(m_xmlReader.errorString()).arg(m_xmlReader.error()));
        }
    }

    return hangingProtocolLoaded;
}

HangingProtocol* HangingProtocolXMLReader::readHangingProtocol()
{
    HangingProtocol *hangingProtocol = new HangingProtocol();
    QStringList protocols;
    QMap<int, HangingProtocolImageSetRestriction> restrictions;

    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == "hangingProtocolName")
        {
            hangingProtocol->setName(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "numberScreens")
        {
            hangingProtocol->setNumberOfScreens(m_xmlReader.readElementText().toInt());
        }
        else if (m_xmlReader.name() == "protocol")
        {
            protocols << m_xmlReader.readElementText();
        }
        else if (m_xmlReader.name() == "institutions")
        {
            hangingProtocol->setInstitutionsRegularExpression(QRegExp(m_xmlReader.readElementText(), Qt::CaseInsensitive));
        }
        else if (m_xmlReader.name() == "restriction")
        {
            HangingProtocolImageSetRestriction restriction = readRestriction();
            restrictions[restriction.getIdentifier()] = restriction;
        }
        else if (m_xmlReader.name() == "imageSet")
        {
            HangingProtocolImageSet *imageSet = readImageSet(restrictions);
            hangingProtocol->addImageSet(imageSet);
        }
        else if (m_xmlReader.name() == "displaySet")
        {
            HangingProtocolDisplaySet *displaySet = readDisplaySet(hangingProtocol);
            hangingProtocol->addDisplaySet(displaySet);
        }
        else if (m_xmlReader.name() == "strictness")
        {
            hangingProtocol->setStrictness(m_xmlReader.readElementText().contains("yes"));
        }
        else if (m_xmlReader.name() == "allDifferent")
        {
            hangingProtocol->setAllDifferent(m_xmlReader.readElementText().contains("yes"));
        }
        else if (m_xmlReader.name() == "iconType")
        {
            hangingProtocol->setIconType(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "numberOfPriors")
        {
            hangingProtocol->setNumberOfPriors(m_xmlReader.readElementText().toInt());
        }
        else if (m_xmlReader.name() == "priority")
        {
            hangingProtocol->setPriority(m_xmlReader.readElementText().toDouble());
        }
        else
        {
            m_xmlReader.skipCurrentElement();
        }
    }

    if (!m_xmlReader.hasError())
    {
        hangingProtocol->setProtocolsList(protocols);
        return hangingProtocol;
    }
    else
    {
        delete hangingProtocol;
        return 0;
    }
}

HangingProtocolImageSetRestriction HangingProtocolXMLReader::readRestriction()
{
    HangingProtocolImageSetRestriction restriction;
    restriction.setIdentifier(m_xmlReader.attributes().value("identifier").toInt());

    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == "selectorAttribute")
        {
            restriction.setSelectorAttribute(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "selectorValue")
        {
            restriction.setSelectorValue(m_xmlReader.readElementText());
        }
        else
        {
            // Saltem l'element perquè no és conegut.
            m_xmlReader.skipCurrentElement();
        }

    }
    return restriction;
}

HangingProtocolImageSet* HangingProtocolXMLReader::readImageSet(const QMap<int, HangingProtocolImageSetRestriction> &restrictions)
{
    HangingProtocolImageSet *imageSet = new HangingProtocolImageSet();
    imageSet->setIdentifier(m_xmlReader.attributes().value("identifier").toString().toInt());

    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == "restrictionExpression")
        {
            HangingProtocolImageSetRestrictionExpression restrictionExpression(m_xmlReader.readElementText(), restrictions);
            imageSet->setRestrictionExpression(restrictionExpression);
        }
        else if (m_xmlReader.name() == "type")
        {
            imageSet->setTypeOfItem(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "abstractPriorValue")
        {
            imageSet->setAbstractPriorValue(m_xmlReader.readElementText().toInt());
        }
        else if (m_xmlReader.name() == "imageNumberInStudyModality")
        {
            imageSet->setImageNumberInStudyModality(m_xmlReader.readElementText().toInt());
        }
        else
        {
            // Saltem l'element perquè no és conegut.
            m_xmlReader.skipCurrentElement();
        }
    }
    return imageSet;
}

HangingProtocolDisplaySet* HangingProtocolXMLReader::readDisplaySet(HangingProtocol *hangingProtocol)
{

    HangingProtocolDisplaySet *displaySet = new HangingProtocolDisplaySet();
    displaySet->setIdentifier(m_xmlReader.attributes().value("identifier").toString().toInt());

    while (m_xmlReader.readNextStartElement())
    {
        if (m_xmlReader.name() == "imageSetNumber")
        {
            HangingProtocolImageSet *imageSet = hangingProtocol->getImageSet(m_xmlReader.readElementText().toInt());
            if (imageSet)
            {
                displaySet->setImageSet(imageSet);
            }
            else
            {
                ERROR_LOG(QString("No s'ha trobat l'image set requerit pel display set %1.").arg(displaySet->getIdentifier()));
            }
        }
        else if (m_xmlReader.name() == "position")
        {
            displaySet->setPosition(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "patientOrientation")
        {
            PatientOrientation patientOrientation;
            patientOrientation.setDICOMFormattedPatientOrientation(m_xmlReader.readElementText());
            displaySet->setPatientOrientation(patientOrientation);
        }
        else if (m_xmlReader.name() == "reconstruction")
        {
            displaySet->setReconstruction(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "phase")
        {
            displaySet->setPhase(m_xmlReader.readElementText().toInt());
        }
        else if (m_xmlReader.name() == "imageNumber")
        {
            displaySet->setSlice(m_xmlReader.readElementText().toInt());
        }
        else if (m_xmlReader.name() == "iconType")
        {
            displaySet->setIconType(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "alignment")
        {
            displaySet->setAlignment(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "toolActivation")
        {
            displaySet->setToolActivation(m_xmlReader.readElementText());
        }
        else if (m_xmlReader.name() == "windowLevel")
        {
            while (m_xmlReader.readNextStartElement())
            {
                if (m_xmlReader.name() == "width")
                {
                    displaySet->setWindowWidth(m_xmlReader.readElementText().toDouble());
                }
                else if (m_xmlReader.name() == "center")
                {
                    displaySet->setWindowCenter(m_xmlReader.readElementText().toDouble());
                }
            }
        }
        else
        {
            // Saltem l'element perquè no és conegut.
            m_xmlReader.skipCurrentElement();
        }
    }
    return displaySet;
}
}
