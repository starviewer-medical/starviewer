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

#ifndef UDG_WINDOWLEVELLEFTTOOL_H
#define UDG_WINDOWLEVELLEFTTOOL_H

#include "windowleveltool.h"

namespace udg {

/**
 * @brief The WindowLevelLeftTool class is a variation of the WindowLevelTool that works with the left mouse button instead of the right one.
 */
class WindowLevelLeftTool : public WindowLevelTool
{

    Q_OBJECT

public:

    WindowLevelLeftTool(QViewer *viewer, QObject *parent = nullptr);

    void handleEvent(unsigned long eventID) override;

};

} // namespace udg

#endif // UDG_WINDOWLEVELLEFTTOOL_H
