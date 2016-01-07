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

#ifndef UDGTRANSDIFFERENCETOOL_H
#define UDGTRANSDIFFERENCETOOL_H

#include <QPoint>

#include "tool.h"

namespace udg {

class Q2DViewer;
class TransDifferenceToolData;

class TransDifferenceTool : public Tool {
Q_OBJECT
public:
    /// Estats de la tool
    enum { None, Moving };

    TransDifferenceTool(QViewer *viewer, QObject *parent = 0);
    ~TransDifferenceTool();

    void handleEvent(unsigned long eventID);

    /// Retorna les dades pròpies de la seed
    ToolData* getToolData() const;

    /// Assigna les dades pròpies de la seed (persistent data)
    void setToolData(ToolData *data);

    /// Inicialitza i calcula tota la imatge diferència
    void initializeDifferenceImage();

    /// Assigna una determinada translació a una llesca
    void setSingleDifferenceImage(int dx, int dy);

private slots:
    /// Comença la translació
    void startTransDifference();

    /// Calcula la nova diferència
    void doTransDifference();

    /// Atura l'estat de la diferència
    void endTransDifference();

    /// Calcula la imatge diferència
    void computeSingleDifferenceImage(int dx, int dy, int slice = -1);

    /// Incrementa els valors dels paràmeters a la tranformació actual
    void increaseSingleDifferenceImage(int dx, int dy);

private:
    /// Dades específiques de la tool
    TransDifferenceToolData *m_myData;

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Valors per controlar la translació
    QPoint m_startPosition;
    int m_dx, m_dy;

    /// Estats d'execució de la tool
    int m_state;
};

}

#endif
