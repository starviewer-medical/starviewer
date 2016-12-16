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

#ifndef UDGSEEDTOOLDATA_H
#define UDGSEEDTOOLDATA_H

#include "tooldata.h"

#include "vector3.h"

namespace udg {

class DrawerPoint;
class Volume;

class SeedToolData : public ToolData {
Q_OBJECT
public:
    SeedToolData(QObject *parent = 0);
    ~SeedToolData();

    void setSeedPosition(Vector3 pos);

    const Vector3& getSeedPosition() const;
    DrawerPoint* getPoint();
    void setPoint(DrawerPoint *p = NULL);
    Volume* getVolume();
    void setVolume(Volume *vol);

private:
    /// Hi guardem la posició de la llavor
    Vector3 m_position;

    /// Punt que es dibuixa
    DrawerPoint *m_point;

    /// Volume que es visualitza
    Volume* m_volume;

};

}

#endif
