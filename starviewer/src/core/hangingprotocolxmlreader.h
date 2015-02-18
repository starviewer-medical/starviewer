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

#include <QXmlStreamReader>
#include <QtContainerFwd>

namespace udg {

class HangingProtocol;
class HangingProtocolDisplaySet;
class HangingProtocolImageSet;
class HangingProtocolImageSetRestriction;

/**
 * @brief The HangingProtocolXMLReader class can read a HangingProtocol from a XML file.
 */
class HangingProtocolXMLReader {

public:
    HangingProtocolXMLReader();
    ~HangingProtocolXMLReader();

    /// Reads and returns a HangingProtocol from the file with the given path. If the hanging protocol can't be read, returns null.
    HangingProtocol* readFile(const QString &path);
    /// Reads and returns a HangingProtocol from the given device. If the hanging protocol can't be read, returns null.
    HangingProtocol* read(QIODevice *device);

private:
    /// Reads and returns a hanging protocol from the XML reader.
    HangingProtocol* readHangingProtocol();
    /// Reads and returns a restriction from the XML reader.
    HangingProtocolImageSetRestriction readRestriction();
    /// Reads and returns a HangingProtocolImageSet from the XML reader.
    HangingProtocolImageSet* readImageSet(const QMap<int, HangingProtocolImageSetRestriction> &restrictions);
    /// Reads and returns a HangingProtocolDisplaySet from the XML reader.
    HangingProtocolDisplaySet* readDisplaySet(HangingProtocol *hangingProtocol);

private:
    /// The reader that is used to read XML files.
    QXmlStreamReader m_xmlReader;

};

}

#endif
