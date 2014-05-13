/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGHANGINGPROTOCOLXMLREADER_H
#define UDGHANGINGPROTOCOLXMLREADER_H

#include <QObject>
#include <QList>
#include <QXmlStreamReader>

#include "hangingprotocolimageset.h"

namespace udg {

class HangingProtocol;
class HangingProtocolDisplaySet;

class HangingProtocolXMLReader : public QObject {
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
    HangingProtocolDisplaySet* readDisplaySet(QXmlStreamReader *reader, HangingProtocol *hangingProtocol);

};

}

#endif
