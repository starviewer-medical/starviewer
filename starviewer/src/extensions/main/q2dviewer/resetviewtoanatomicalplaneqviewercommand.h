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

#ifndef UDG_RESETVIEWTOANATOMICALPLANEQVIEWERCOMMAND_H
#define UDG_RESETVIEWTOANATOMICALPLANEQVIEWERCOMMAND_H

#include "qviewercommand.h"

#include "anatomicalplane.h"

namespace udg {

class Q2DViewer;

/**
 * @brief The ResetViewToAnatomicalPlaneQViewerCommand class is a QViewerCommand that resets the viewer to a specific anatomical plane.
 */
class ResetViewToAnatomicalPlaneQViewerCommand : public QViewerCommand
{
    Q_OBJECT

public:
    ResetViewToAnatomicalPlaneQViewerCommand(Q2DViewer *viewer, const AnatomicalPlane &anatomicalPlane, QObject *parent = 0);
    virtual ~ResetViewToAnatomicalPlaneQViewerCommand();

public slots:
    virtual void execute();

private:
    Q2DViewer *m_viewer;
    AnatomicalPlane m_anatomicalPlane;

};

} // namespace udg

#endif // UDG_RESETVIEWTOANATOMICALPLANEQVIEWERCOMMAND_H
