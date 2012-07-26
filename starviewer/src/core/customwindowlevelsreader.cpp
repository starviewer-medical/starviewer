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
                            customWindowLevel->setLevel(reader.readElementText().toDouble());
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
