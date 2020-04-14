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

#ifndef UDGCIRCLETOOL_H
#define UDGCIRCLETOOL_H

#include "tool.h"

#include "vector3.h"

#include <QPointer>

namespace udg {

class DrawerPolygon;
class Q2DViewer;

/**
    Eina per dibuixar un cercle amb una marca al centre.
 */
class CircleTool : public Tool {

    Q_OBJECT

public:

    explicit CircleTool(QViewer *viewer, QObject *parent = nullptr);
    ~CircleTool() override;

    /// Decideix què s'ha de fer per cada event rebut.
    virtual void handleEvent(unsigned long eventId) override;

private:

    /// Comença a dibuixar el cercle.
    void startDrawing();
    /// Acaba de dibuixar el cercle.
    void endDrawing();
    /// Avorta el dibuix del cercle.
    void abortDrawing();
    /// Actualitza el cercle quan es mou el ratolí mentre s'està dibuixant.
    void updateCircle();
    /// Obté el punt on acaba la bounding box del cercle, assegurant-se que entre aquest i l'inicial es forma un quadrat.
    void getEndPoint();
    /// Actualitza els punts del polígon perquè resulti el dibuix del cercle.
    void updatePolygonPoints();
    /// Retorna el centre del cercle.
    Vector3 getCenter() const;
    /// Equalitza la profunditat dels elements que formen el cercle final.
    void equalizeDepth();

private slots:

    /// Inicialitza l'eina.
    void initialize();

private:

    /// Visor 2D sobre el qual treballem.
    Q2DViewer *m_2DViewer;
    /// El cercle.
    QPointer<DrawerPolygon> m_circle;
    /// Cert mentre s'està dibuixant un cercle.
    bool m_isDrawing;
    /// Punt on comença la bounding box del cercle.
    Vector3 m_startPoint;
    /// Punt on acaba la bounding box del cercle.
    Vector3 m_endPoint;

};

}

#endif
