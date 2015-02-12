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

#include <QList>
#include <QXmlStreamReader>

#include "hangingprotocolimageset.h"

namespace udg {

class HangingProtocol;
class HangingProtocolDisplaySet;

/**
 * @brief The HangingProtocolXMLReader class can read a HangingProtocol from a XML file.
 */
class HangingProtocolXMLReader {

public:
    HangingProtocolXMLReader();
    ~HangingProtocolXMLReader();

    /// Reads and returns a HangingProtocol from the file with the given path. If the hanging protocol can't be read, returns null.
    HangingProtocol* readFile(const QString &path);

private:
    /// Reads and returns a restriction from the XML reader.
    HangingProtocolImageSet::Restriction readRestriction(QXmlStreamReader *reader);
    /// Reads and returns a HangingProtocolImageSet from the XML reader.
    HangingProtocolImageSet* readImageSet(QXmlStreamReader *reader, const QList<HangingProtocolImageSet::Restriction> &restrictionList);
    /// Reads and returns a HangingProtocolDisplaySet from the XML reader.
    HangingProtocolDisplaySet* readDisplaySet(QXmlStreamReader *reader, HangingProtocol *hangingProtocol);

};

}

#endif
