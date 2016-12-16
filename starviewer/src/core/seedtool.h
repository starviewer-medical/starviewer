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

#ifndef UDGSEEDTOOL_H
#define UDGSEEDTOOL_H

#include "tool.h"

#include "vector3.h"

namespace udg {

class Q2DViewer;
class SeedToolData;

/**
    Eina que serveix per posar llavors en un visor 2D
  */
class SeedTool : public Tool {
Q_OBJECT
public:
    /// Estats de la tool
    enum { None, Seeding };

    SeedTool(QViewer *viewer, QObject *parent = 0);
    ~SeedTool();

    void handleEvent(long unsigned eventID);

    /// Retorna les dades pròpies de la seed
    ToolData* getToolData() const;

    /// Assigna les dades pròpies de la seed (persistent data)
    void setToolData(ToolData *data);

    /// Posem la llavor i li passem la posició i la llesca: útil per fer-ho per codi
    void setSeed(const Vector3 &seedPosition, int slice);

signals:
    void seedChanged(double, double, double);

private:
    /// Posem la llavor
    void setSeed();

    /// Calcula la llesca que s'ha de moure
    void doSeeding();

    /// Atura l'estat de seeding
    void endSeeding();

    /// Actualitzem la posició de la llavor
    void updateSeedPosition();

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Estat de la tool
    int m_state;

    /// Dades específiques de la tool
    SeedToolData *m_myData;

    /// Bool per saber si està pintada o no (si l'hem enviat al drawer)
    bool m_drawn;

};

}

#endif
