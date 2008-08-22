/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "hangingprotocolmask.h"

namespace udg {

HangingProtocolMask::HangingProtocolMask(QObject *parent)
 : QObject(parent)
{
}


HangingProtocolMask::~HangingProtocolMask()
{
}


void HangingProtocolMask::setProtocolsList( QList<QString> protocols )
{
    m_protocolsList = protocols;
}

QList<QString> HangingProtocolMask::getProtocolList()
{
    return m_protocolsList;
}

}
