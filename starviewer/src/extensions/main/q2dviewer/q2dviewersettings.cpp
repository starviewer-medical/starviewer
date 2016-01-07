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

#include "q2dviewersettings.h"

namespace udg {

// Definició de les claus
const QString Q2DViewerSettings::KeyPrefix("Starviewer-App-2DViewer/");
const QString Q2DViewerSettings::UserProfile(KeyPrefix + "profile");

Q2DViewerSettings::Q2DViewerSettings()
{
}

Q2DViewerSettings::~Q2DViewerSettings()
{
}

void Q2DViewerSettings::init()
{
}

} // end namespace udg
