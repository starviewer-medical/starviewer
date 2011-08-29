#include "utils.h"

#include <dcuid.h>
#include <QTcpServer>

#include "logging.h"

namespace udg {

QString Utils::generateUID(const QString &prefix)
{
    char uid[512];
    if (prefix.isEmpty())
    {
        // Tindrà el prefix de dcmtk
        dcmGenerateUniqueIdentifier(uid);
    }
    else
    {
        dcmGenerateUniqueIdentifier(uid, qPrintable(prefix));
    }

    DEBUG_LOG("Obtained UID: " + QString(uid));

    return QString(uid);
}

}; // End udg namespace
