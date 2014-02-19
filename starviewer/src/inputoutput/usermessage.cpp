#include "usermessage.h"

#include "starviewerapplication.h"

namespace udg {

UserMessage::UserMessage()
{
}

UserMessage::~UserMessage()
{
}

QString UserMessage::getProblemPersistsAdvice()
{
    return QObject::tr("If the problem persists contact with an administrator");
}

QString UserMessage::getCloseWindowsAndTryAgainAdvice()
{
    return QObject::tr("Close all %1 windows and try again.").arg(ApplicationNameString);
}

} // End namespace udg
