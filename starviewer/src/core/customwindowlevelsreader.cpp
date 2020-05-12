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

#include "customwindowlevelsreader.h"
#include "windowlevel.h"
#include "logging.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlSchema>
#include <QList>

namespace udg {

CustomWindowLevelsReader::CustomWindowLevelsReader()
{
   QXmlSchema schema;
   schema.load(QUrl("qrc:xmlSchemes/CustomWindowLevels.xml"));
   m_customWindowLevelsSchemaValidator.setSchema(schema);
}

CustomWindowLevelsReader::~CustomWindowLevelsReader()
{
}

QList<WindowLevel*> CustomWindowLevelsReader::readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return QList<WindowLevel*>();
    }

    if (!m_customWindowLevelsSchemaValidator.validate(QUrl::fromLocalFile(file.fileName())))
    {
        DEBUG_LOG("No es processa perquè es invalid");
        return QList<WindowLevel*>();
    }

    QList<WindowLevel*> listOfCustomWindowLevels;

    QXmlStreamReader reader(&file);
    if (reader.readNextStartElement())
    {
        if (reader.name() == "CustomWindowLevels")
        {
            while (reader.readNextStartElement())
            {
                if (reader.name() == "CustomWindowLevel")
                {
                    WindowLevel *customWindowLevel = new WindowLevel();
                    QString name = reader.attributes().first().value().toString();
                    customWindowLevel->setName(name);

                    while (reader.readNextStartElement())
                    {
                        if (reader.name() == "width")
                        {
                            customWindowLevel->setWidth(reader.readElementText().toDouble());
                        }
                        else if (reader.name() == "level")
                        {
                            customWindowLevel->setCenter(reader.readElementText().toDouble());
                        }
                    }
                    listOfCustomWindowLevels.append(customWindowLevel);
                }
            }
        }
    }

    return listOfCustomWindowLevels;
}

}
