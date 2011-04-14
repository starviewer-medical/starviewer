#ifndef UDGHANGINGPROTOCOLXMLREADER_H
#define UDGHANGINGPROTOCOLXMLREADER_H

#include <QObject>
#include <QList>
#include <QXmlStreamReader>

#include "hangingprotocolimageset.h"

namespace udg {

class HangingProtocol;
class HangingProtocolDisplaySet;

class HangingProtocolXMLReader : public QObject
{
Q_OBJECT
public:
    HangingProtocolXMLReader(QObject *parent = 0);

    ~HangingProtocolXMLReader();

    /// Llegeix el hanging protocol del fitxer. Retrona NULL si s'han pogut carregar
    HangingProtocol* readFile(const QString &path);

private:

    /// Llegeix una restriccio
    HangingProtocolImageSet::Restriction readRestriction(QXmlStreamReader *reader);

    /// Llegeix un image set
    HangingProtocolImageSet* readImageSet(QXmlStreamReader *reader, const QList<HangingProtocolImageSet::Restriction> &restrictionList);

    /// Llegeix un display set
    HangingProtocolDisplaySet* readDisplaySet(QXmlStreamReader * reader, HangingProtocol *hangingProtocol);

};

}

#endif
