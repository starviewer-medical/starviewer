/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLMASK_H
#define UDGHANGINGPROTOCOLMASK_H

#include <QObject>
#include <QList>
#include <QString>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolMask : public QObject
{
Q_OBJECT
public:
    HangingProtocolMask(QObject *parent = 0);

    ~HangingProtocolMask();

    /// Llista de protocols a tractar del hanging protocol
    void setProtocolsList( QList<QString> protocols );

    /// Obtenir la llista de protocols
    QList<QString> getProtocolList();

private:

    QList<QString> m_protocolsList;
};

}

#endif
