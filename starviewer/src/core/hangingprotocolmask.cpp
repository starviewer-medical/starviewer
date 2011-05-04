#include "hangingprotocolmask.h"

namespace udg {

HangingProtocolMask::HangingProtocolMask(QObject *parent)
 : QObject(parent)
{
}

HangingProtocolMask::~HangingProtocolMask()
{
}

void HangingProtocolMask::setProtocolsList(const QList<QString> &protocols)
{
    m_protocolsList = protocols;
}

QList<QString> HangingProtocolMask::getProtocolList() const
{
    return m_protocolsList;
}

}
