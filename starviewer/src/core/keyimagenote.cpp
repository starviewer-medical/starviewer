/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "keyimagenote.h"

#include "logging.h"
#include "keyimagenotefilereader.h"


namespace udg {

KeyImageNote::KeyImageNote()
{
    m_keyObjectDescription = "";
    m_documentTitle = "";
    m_observableContext = "";
    m_documentTitleQualityReason = "";
}

KeyImageNote::~KeyImageNote()
{
}

bool KeyImageNote::loadFromFile(const QString &filename)
{
    if (filename.isEmpty())
        return false;

    KeyImageNoteFileReader KINReader(filename);
    KeyImageNote *keyImageNote = KINReader.read();
    if ( ! keyImageNote )
    {
        WARN_LOG( "Al llegir l'image note s'ha produït un error: " + KINReader.getLastErrorDescription() );
        return false;
    }
    operator=(*keyImageNote);
    return true;
}

QString KeyImageNote::getDocumentTitle() const
{
    return m_documentTitle;
}

bool KeyImageNote::hasDocumentTitle() const
{
    return ! this->getDocumentTitle().isEmpty();
}

void KeyImageNote::setDocumentTitle(const QString &title)
{
    m_documentTitle = title;
}

QString KeyImageNote::getDocumentTitleQualityReasons() const
{
    return m_documentTitleQualityReason;
}

void KeyImageNote::setDocumentTitleQualityReasons(const QString &reason)
{
    m_documentTitleQualityReason = reason;
}

bool KeyImageNote::hasDocumentTitleQualityReasons() const
{
    return ! this->getDocumentTitleQualityReasons().isEmpty();
}

QString KeyImageNote::getKeyObjectDescription() const
{
    return m_keyObjectDescription;
}

void KeyImageNote::setKeyObjectDescription(const QString &description)
{
    m_keyObjectDescription = description;
}

bool KeyImageNote::hasKeyObjectDescription() const
{
    return ! this->getKeyObjectDescription().isEmpty();
}

QString KeyImageNote::getObservableContext() const
{
    return m_observableContext;
}

void KeyImageNote::setObservableContext(const QString &context)
{
    m_observableContext = context;
}

bool KeyImageNote::hasObservableContext() const
{
    return ! this->getObservableContext().isEmpty();
}

QString KeyImageNote::getPrintableText() const
{
    QString printableText;

    if(this->hasDocumentTitle())
    {
        printableText = "Title: " + this->getDocumentTitle();
    }

    if(this->hasDocumentTitleQualityReasons())
    {
        if (!printableText.isEmpty())
            printableText += " ";
        printableText += "Reason: " + this->getDocumentTitleQualityReasons();
    }

    if(this->hasKeyObjectDescription())
    {

        if (!printableText.isEmpty())
            printableText += " ";
        printableText += "\nDescription: " + this->getKeyObjectDescription();
    }

    return printableText;
}

QStringList KeyImageNote::getReferencedSOPInstancesUID() const
{
    return m_referencedSOPInstancesUIDList;
}

void KeyImageNote::setReferencedSOPInstancesUID(const QStringList &list)
{
    m_referencedSOPInstancesUIDList = list;
}

}
