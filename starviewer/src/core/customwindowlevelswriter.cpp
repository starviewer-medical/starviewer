#ifndef UDGCUSTOMWINDOWLEVELSWRITER_CPP
#define UDGCUSTOMWINDOWLEVELSWRITER_CPP

#include "customwindowlevelswriter.h"
#include "customwindowlevel.h"
#include "customwindowlevelsrepository.h"
#include "starviewerapplication.h"
#include "logging.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QList>
#include <QApplication>

namespace udg {

CustomWindowLevelsWriter::CustomWindowLevelsWriter()
{
}

CustomWindowLevelsWriter::~CustomWindowLevelsWriter()
{
}

void CustomWindowLevelsWriter::write()
{
    // establir el path per defecte on es guardarà el fitxer
    QString path = getPath();
    QFile file(path);
    if (file.exists()) 
    {
        // si ja existeix l'esborrem ja que si és més llarg que el nou, hi quedaran caràcters del vell
        QFile::remove(path);
    }
    if (!file.open(QFile::ReadWrite | QFile::Text))
    {
        return;
    }

    QXmlStreamWriter writer(&file);

    // encara que no hi hagi res al repository s'esborra el fitxer i es crea de nou
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QString("CustomWindowLevels"));
    // TODO: probablement s'hauria de posar l'esquema que fa servir
    // Per exemple com a atribut xmlns

    // Per cada custom window level
    foreach (CustomWindowLevel *windowLevel, CustomWindowLevelsRepository::getRepository()->getItems())
    {
        writer.setAutoFormattingIndent(1);
        writer.writeStartElement(QString("CustomWindowLevel"));
        writer.writeAttribute(QString("name"), QString("%1").arg(windowLevel->getName()));
        writer.setAutoFormattingIndent(2);
        // un tag nou per window i per level
        writer.writeTextElement(QString("width"), QString("%1").arg(windowLevel->getWidth()));
        writer.writeTextElement(QString("level"), QString("%1").arg(windowLevel->getLevel()));
        writer.setAutoFormattingIndent(1);
        writer.writeEndElement();
    }
    writer.setAutoFormattingIndent(0);
    writer.writeEndElement();
    writer.writeEndDocument();
 
}

QString CustomWindowLevelsWriter::getPath()
{
    /// Custom window levels definits per defecte, agafa el directori de l'executable TODO això podria ser un setting més
    QString defaultPath = "/etc/xdg/" + OrganizationNameString + "/" + ApplicationNameString + "/customwindowlevels/"; // Path linux
    
    if (!QFile::exists(defaultPath))
    {    
        defaultPath = qApp->applicationDirPath() + "/customwindowlevels/";
    }
    if (!QFile::exists(defaultPath))
    {
        // En entorn de desenvolupament Windows & Linux
        defaultPath = qApp->applicationDirPath() + "/../customwindowlevels/";
    }
    if (!QFile::exists(defaultPath))
    {
        // En entorn de desenvolupament Mac OS X
        defaultPath = qApp->applicationDirPath() + "/../../../../customwindowlevels/";
    }

    defaultPath += "customwindowlevels.xml";

    return defaultPath;
}

}

#endif
