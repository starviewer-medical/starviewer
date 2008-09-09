/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLXMLREADER_H
#define UDGHANGINGPROTOCOLXMLREADER_H

#include <QObject>
#include <QList>
#include <QXmlStreamReader>
#include "hangingprotocolimageset.h"

namespace udg {

class HangingProtocol;
class HangingProtocolDisplaySet;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolXMLReader : public QObject
{
Q_OBJECT
public:
    HangingProtocolXMLReader(QObject *parent = 0);

    ~HangingProtocolXMLReader();

    /// llegeix els hanging protocols del fitxer
    QList<HangingProtocol * > readFile( QString path );

private:

    /// Llegeix una restriccio
    HangingProtocolImageSet::Restriction readRestriction( QXmlStreamReader * reader );

    /// Llegeix un image set
    HangingProtocolImageSet * readImageSet( QXmlStreamReader * reader );

    /// Llegeix un display set
    HangingProtocolDisplaySet * readDisplaySet( QXmlStreamReader * reader );

private:

    QList<HangingProtocolImageSet::Restriction> m_restrictionsList;
};

}

#endif
