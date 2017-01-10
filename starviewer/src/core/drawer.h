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

#ifndef UDGDRAWER_H
#define UDGDRAWER_H

#include <QObject>
#include <QMultiMap>
#include <QSet>

#include "q2dviewer.h"

namespace udg {

class DrawerPrimitive;

/**
    Classe encarregada de pintar els objectes de primitiva gràfica en el viewer assignat
  */
class Drawer : public QObject {
Q_OBJECT
public:
    Drawer(Q2DViewer *viewer, QObject *parent = 0);
    ~Drawer();

    /// Dibuixa la primitiva donada sobre el pla i llesca indicats
    /// @param primitive Primitiva a pintar
    /// @param plane Pla sobre el qual volem pintar la primitiva
    /// @param slice Llesca a la que adjuntem la primitiva
    void draw(DrawerPrimitive *primitive, const OrthogonalPlane &plane, int slice);

    /// Dibuixa la primitiva sempre al cim sense importar en quin pla o llesca ens trobem. 
    /// La visibilitat dependrà només de la propietat isVisible() de la primitiva
    void draw(DrawerPrimitive *primitive);

    /// Esborra totes les primitives esborrables que es veuen al visor, és a dir, en el pla i llesques actuals.
    void clearViewer();

    /// Afegim una primitiva al grup indicat.
    /// @param primitive Primitiva que volem afegir
    /// @param groupName nom del grup on la volem incloure
    void addToGroup(DrawerPrimitive *primitive, const QString &groupName);

    /// Deshabilita les primitives que hi ha en un determinat grup, fent que mai siguin visibles, sota cap condició
    /// @param groupName Nom del grup de primitives que volem deshabilitar
    void disableGroup(const QString &groupName);

    /// Habilita les primitives que hi ha en un determinat grup, fent que tornin a tenir el comportament habitual 
    /// on el Drawer decidirà si han de ser visibles o no segons el pla i llesca assignats
    /// @param groupName Nom del grup de primitives que volem habilitar
    void enableGroup(const QString &groupName);

    /// Retorna la primitiva esborrable més propera al punt donat, dins de la vista i llesca proporcionats
    /// Aquest mètode no té en compte cap llindar de proximitat, és a dir, ens retorna la primitiva que en termes
    /// absoluts és més propera al punt donat. En cas que no hi hagi cap primitiva per aquella vista i llesca, es retornarà nul.
    DrawerPrimitive* getNearestErasablePrimitiveToPoint(const Vector3 &point, const OrthogonalPlane &view, int slice, Vector3 &closestDisplayPoint);

    /// Ens esborra les primitives esborrables que estiguin dins de la zona delimitada pels punts passats per paràmetre.
    void erasePrimitivesInsideBounds(const std::array<double, 4> &displayBounds, const OrthogonalPlane &view, int slice);

    /// Ens diu el total de primitives dibuixades en totes les vistes
    int getNumberOfDrawnPrimitives();

public slots:
    /// Deixa de mantenir la primitiva dins de la seva estructura interna
    /// i l'elimina de l'escena on s'estava pintant
    /// @param primitive Primitiva que volem deixar de controlar
    void erasePrimitive(DrawerPrimitive *primitive);

    /// Esborra totes les primitives registrades al drawer (inclou les primitives no esborrables).
    void removeAllPrimitives();

private:
    /// Mostra/amaga les primitives que hi ha en un pla i llesca determinats
    /// @param plane Pla sobre que volem mostrar/amagar les primitives
    /// @param slice Llesca dins d'aquell pla.
    void hide(const OrthogonalPlane &plane, int slice);
    void show(const OrthogonalPlane &plane, int slice);

    /// Ens diu si la primitiva donada, que es troba a la vista view, està dins dels bounds indicats
    bool isPrimitiveInside(DrawerPrimitive *primitive, const std::array<double, 4> &displayBounds);

    /// Esborra la primitiva donada del contenidor de primitives especificat.
    /// Si la troba l'esborra. Retorna cert si la troba, fals altrament.
    bool erasePrimitiveFromContainer(DrawerPrimitive *primitive, QMultiMap<int, DrawerPrimitive*> &primitiveContainer);

    /// Fa que la primitiva es pugui visualitzar al visor associat
    void renderPrimitive(DrawerPrimitive *primitive);

private slots:
    /// Refresca les primitives que s'han de veure pel viewer segons el seu estat
    void refresh();

private:
    /// Viewer sobre el qual pintarem les primitives
    Q2DViewer *m_2DViewer;

    /// Contenidors de primitives per cada pla possible
    QMultiMap<int, DrawerPrimitive*> m_XYPlanePrimitives;
    QMultiMap<int, DrawerPrimitive*> m_YZPlanePrimitives;
    QMultiMap<int, DrawerPrimitive*> m_XZPlanePrimitives;
    QList<DrawerPrimitive*> m_top2DPlanePrimitives;

    /// Pla i llesca en el que es troba en aquell moment el 2D Viewer. Serveix per controlar
    /// els canvis de llesca i de pla, per saber quines primitives hem de netejar
    OrthogonalPlane m_currentPlane;
    int m_currentSlice;

    /// Grups de primitives. Les agrupem per nom
    QMultiMap<QString, DrawerPrimitive*> m_primitiveGroups;

    /// Conjunt de primitives en estat disabled
    QSet<DrawerPrimitive*> m_disabledPrimitives;
};

}

#endif
