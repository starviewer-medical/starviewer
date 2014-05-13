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

#include "dicomdumpdefaulttagsreader.h"
#include "dicomtag.h"
#include "dicomdumpdefaulttags.h"
#include "logging.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlSchema>

namespace udg {

DICOMDumpDefaultTagsReader::DICOMDumpDefaultTagsReader()
{
   QXmlSchema schema;
   schema.load(QUrl("qrc:xmlSchemes/DICOMDumpDefaultTags.xml"));
   m_defaultTagsSchemaValidator.setSchema(schema);
}

DICOMDumpDefaultTagsReader::~DICOMDumpDefaultTagsReader()
{
}

DICOMDumpDefaultTags* DICOMDumpDefaultTagsReader::readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return NULL;
    }

    if (!m_defaultTagsSchemaValidator.validate(QUrl::fromLocalFile(file.fileName())))
    {
        DEBUG_LOG("No es processa perquè es invalid");
        return NULL;
    }

    DICOMDumpDefaultTags *dicomDumpDefaultTag = new DICOMDumpDefaultTags();

    QXmlStreamReader reader(&file);
    if (reader.readNextStartElement())
    {
        if (reader.name() == "DICOMDumpDefaultTags")
        {
            while (reader.readNextStartElement())
            {
                if (reader.name() == "SOPClassUID")
                {
                    dicomDumpDefaultTag->setSOPClassUID(reader.readElementText());
                }
                else if (reader.name() == "restrictions")
                {
                    while (reader.readNextStartElement())
                    {
                        DICOMDumpDefaultTagsRestriction restriction = readRestriction(&reader);
                        dicomDumpDefaultTag->addRestriction(restriction);
                    }
                }
                else if (reader.name() == "tagsToShow")
                {
                    while (reader.readNextStartElement())
                    {
                        DICOMTag dicomTag = readTag(&reader);
                        dicomDumpDefaultTag->addTagToShow(dicomTag);
                    }
                }
            }
        }
    }

    return dicomDumpDefaultTag;
}

DICOMTag DICOMDumpDefaultTagsReader::readTag(QXmlStreamReader *reader)
{
    DICOMTag dicomTag;

    while (reader->readNextStartElement())
    {
        if (reader->name() == "group")
        {
            dicomTag.setGroup(reader->readElementText().toUInt(0, 16));
        }
        else if (reader->name() == "element")
        {
            dicomTag.setElement(reader->readElementText().toUInt(0, 16));
        }
        else if (reader->name() == "name")
        {
            dicomTag.setName(reader->readElementText());
        }
    }

    return dicomTag;
}

DICOMDumpDefaultTagsRestriction DICOMDumpDefaultTagsReader::readRestriction(QXmlStreamReader *reader)
{
    DICOMDumpDefaultTagsRestriction dicomRestriction;

    while (reader->readNextStartElement())
    {
        if (reader->name() == "tag")
        {
            DICOMTag dicomTag = readTag(reader);
            dicomRestriction.setDICOMTag(dicomTag);
        }
        else if (reader->name() == "value")
        {
            dicomRestriction.setValue(reader->readElementText());
        }
        else if (reader->name() == "operator")
        {
            dicomRestriction.setOperator(reader->readElementText());
        }
    }

    return dicomRestriction;
}
}
