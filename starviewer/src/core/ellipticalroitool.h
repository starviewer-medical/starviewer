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

#ifndef UDGELLIPTICALROITOOL_H
#define UDGELLIPTICALROITOOL_H

#include "roitool.h"

#include "vector3.h"

namespace udg {

class Q2DViewer;
class DrawerText;

/**
    Tool per calcular l'àrea i la mitjana de grisos d'un òval
  */
class EllipticalROITool : public ROITool {
Q_OBJECT
public:
    EllipticalROITool(QViewer *viewer, QObject *parent = 0);
    ~EllipticalROITool();

    void handleEvent(long unsigned eventID);

protected:
    virtual void setTextPosition(DrawerText *text);

private:
    /// Estats de la Tool
    enum { Ready, FirstPointFixed };

    /// Gestiona les accions a realitzar quan es clica el ratolí
    void handlePointAddition();

    /// Simula la forma de l'el·lipse quan tenim el primer punt i movem el mouse
    void simulateEllipse();

    /// Calcula el centre de l'el·lipse a partir dels punts introduits mitjançant la interacció de l'usuari
    Vector3 computeEllipseCentre() const;

    /// Actualitza els punts del polígon perquè resulti el dibuix de l'òval
    void updatePolygonPoints();

    /// Dona el dibuix de l'òval per finalitzat
    void closeForm();

    /// Elimina la representacio temporal de la tool
    void deleteTemporalRepresentation();

    /// Equalitza la profunditat dels elements que formen l'el·lipse final.
    void equalizeDepth();

private slots:
    /// Inicialitza la tool
    void initialize();

private:
    /// Punts que annotem de la interacció de l'usuari per crear l'òval
    Vector3 m_firstPoint;
    Vector3 m_secondPoint;

    /// Estat de la tool
    int m_state;
};

}

#endif
