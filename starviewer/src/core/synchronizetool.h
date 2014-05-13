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

#ifndef UDGSYNCHRONIZETOOL_H
#define UDGSYNCHRONIZETOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class SynchronizeToolData;

/**
    Tool de sincronització entre visualitzadors.
  */
class SynchronizeTool : public Tool {
Q_OBJECT
public:
    SynchronizeTool(QViewer *viewer, QObject *parent = 0);
    ~SynchronizeTool();

    void handleEvent(unsigned long eventID);

    /// Per posar dades (compartides)
    virtual void setToolData(ToolData *data);

    /// Per obtenir les dades (per compartir)
    virtual ToolData* getToolData() const;

    /// Habilita o deshabilita la tool
    /// En estat habilitat, rebrà els signals de sincronització
    /// En estat deshabilitat, no sincronitzarà res respecte la resta
    /// Per defecte la tool es troba en estat deshabilitat
    void setEnabled(bool enabled);

private slots:
    /// Posa l'increment a les dades
    void setIncrement(int slice);

    /// Aplica els canvis de canvi de llesca
    void applySliceChanges();

    /// Reseteja els paràmetres amb els que es realitza la sincronització
    /// S'invoca quan es canvia de volum, vista o s'activa l'eina
    void reset();

private:
    /// Llesca anterior per saber l'increment
    int m_lastSlice;

    /// Vista anterior per saber si s'ha de calcular l'increment
    QString m_lastView;

    /// Thickness perdut per arrodoniment
    double m_roundLostSpacingBetweenSlices;

    /// Dades de la tool
    SynchronizeToolData *m_toolData;

    /// Visualitzador 2d al que pertany la tool
    Q2DViewer *m_q2dviewer;

    /// Estat de la tool
    bool m_enabled;
};

}

#endif
