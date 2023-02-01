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

#ifndef UDG_ECHOTOWADOPACS_H
#define UDG_ECHOTOWADOPACS_H

#include <QString>

namespace udg {

class PacsDevice;

/**
 * @brief The EchoToWadoPacs tests if a WADO PACS responds to WADO requests.
 */
class EchoToWadoPacs
{
public:
    EchoToWadoPacs();

    /// Sends a search request to the given PACS and returns true if successful and false otherwise. In case of failure errors are recorded.
    bool echo(const PacsDevice &pacs);

    /// Returns the errors produced in the request, if any.
    const QString& getErrors() const;

private:
    /// Stores the errors produced in the request.
    QString m_errors;
};

} // namespace udg

#endif // UDG_ECHOTOWADOPACS_H
