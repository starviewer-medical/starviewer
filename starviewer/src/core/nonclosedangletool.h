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

#ifndef UDGNONCLOSEDANGLETOOL_H
#define UDGNONCLOSEDANGLETOOL_H

#include "tool.h"

#include "vector3.h"

#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerLine;
class DrawerText;

/**
    Tool per calcular angles
  */
class NonClosedAngleTool : public Tool {
Q_OBJECT
public:
    /// Possibles estats de la tool
    enum { None, FirstLineFixed, SecondLineFixed };

    NonClosedAngleTool(QViewer *viewer, QObject *parent = 0);
    ~NonClosedAngleTool();

    void handleEvent(long unsigned eventID);

private:
    /// Gestiona quin punt de l'angle estem dibuixant. Es cridarà cada cop que
    /// haguem fet un clic amb el botó esquerre del mouse.
    void handlePointAddition();

    /// Ens permet anotar els punts de les línies.
    void annotateLinePoints();

    /// Gestiona quina de les línies estem dibuixant mentres es mou el mouse
    void handleLineDrawing();

    /// Ens simula la linia que estem dibuixant respecte el punt on està el mouse.
    void simulateLine(DrawerLine *line);

    /// Calcula l'angle de les dues línies dibuixades
    void computeAngle();

    /// Calcula la correcta posició del caption de l'angle segons els punts de l'angle
    void placeText(const Vector3 &firstLineVertex, const Vector3 &secondLineVertex, DrawerText *angleText);

    /// Elimina la representacio temporal de la tool
    void deleteTemporalRepresentation();

    /// Equalitza la profunditat dels elements que formen l'angle obert final.
    void equalizeDepth();

private slots:
    /// Inicialitza l'estat de la tool
    void initialize();

private:
    /// Estats de la línia segons els punts
    enum { NoPoints, FirstPoint };

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Primera línia
    QPointer<DrawerLine> m_firstLine;

    /// Segona línia
    QPointer<DrawerLine> m_secondLine;

    /// Línia d'unió
    QPointer<DrawerLine> m_middleLine;

    /// Estat de la tool
    int m_state;

    /// Estat d'una línia
    int m_lineState;
};

}

#endif
