#include "hangingprotocolmask.h"

namespace udg {

HangingProtocolMask::HangingProtocolMask(QObject *parent)
 : QObject(parent)
{
}

HangingProtocolMask::~HangingProtocolMask()
{
}

void HangingProtocolMask::setProtocolsList(const QStringList &protocols)
{
    m_protocolsList = protocols;
}

QStringList HangingProtocolMask::getProtocolList() const
{
    return m_protocolsList;
}

}
