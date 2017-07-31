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

#ifndef UDGVIEWERSLAYOUT_H
#define UDGVIEWERSLAYOUT_H

#include <QWidget>

#include <QMap>
#include <QSet>

namespace udg {

class AnatomicalPlane;
class Q2DViewerWidget;
class RelativeGeometryLayout;
class Volume;

/**
    Classe que permet distribuir sobre un widget una sèrie Q2DViewerWidgets
    amb diferents layouts i geometries de forma versàtil.
  */
class ViewersLayout : public QWidget {
Q_OBJECT
public:
    ViewersLayout(QWidget *parent = 0);
    ~ViewersLayout();

    /// Obtenir el visualitzador seleccionat
    Q2DViewerWidget* getSelectedViewer() const;

    /// Ens retorna en nombre de viewers totals que conté el gestor de layouts,
    /// independentment de si són visibles o no.
    int getNumberOfViewers() const;

    /// Ens retorna el visor amb índex "number". Si number està fora de rang,
    /// ens retornarà un punter nul.
    Q2DViewerWidget* getViewerWidget(int number) const;

    /// Fa un layout regular amb les files i columnes indicades
    void setGrid(int rows, int columns);
    void setGridInArea(int rows, int columns, const QRectF &geometry);

    /// Afegeix un nou visualitzador amb la geometria indicada
    Q2DViewerWidget* addViewer(const QRectF &geometry);

    /// Neteja el layout, eliminant tots els visors i geometries,
    /// deixant-lo en l'estat inicial, com si acabéssim de crear l'objecte
    void cleanUp();
    void cleanUp(const QRectF &geometry);

    QRectF convertGeometry(const QRectF &viewerGeometry, const QRectF &newGeometry);

    /// Maps all viewers inside \a oldGeometry to occupy the corresponding space inside \a newGeometry.
    void mapViewersToNewGeometry(const QRectF &oldGeometry, const QRectF &newGeometry);

    /// Returns the relative geometry of the given viewer in its normal (i.e. not maximized) state.
    /// If the given viewer is not in this ViewersLayout, returns a null QRectF.
    QRectF getGeometryOfViewer(Q2DViewerWidget *viewer) const;

    /// Return viewers located inside the given geometry.
    QList<Q2DViewerWidget *> getViewersInsideGeometry(const QRectF &geometry);

public slots:
    /// Marquem com a seleccionat el viewer passat per paràmetre
    void setSelectedViewer(Q2DViewerWidget *viewer);

    /// Resizes the given viewer to change between normal (demaximized) and maximized states.
    void toggleMaximization(Q2DViewerWidget *viewer);

signals:
    /// Senyal que s'emet quan s'afegeix un visualitzador
    void viewerAdded(Q2DViewerWidget *viewer);

    /// Signal to be emitted just before the viewer widget is going to be deleted
    void viewerRemoved(Q2DViewerWidget *viewer);

    /// Senyal que s'emet quan s'amaga un visualitzador, és a dir,
    // deixa de ser visible però segueix existint
    void viewerHidden(Q2DViewerWidget *viewer);

    /// Emitted when a previously hid viewer is shown
    void viewerShown(Q2DViewerWidget *viewer);

    /// Senyal que s'emet quan el visualitzador seleccionat canvia
    void selectedViewerChanged(Q2DViewerWidget *viewer);

    void manualSynchronizationStateChanged(bool enable);

    /// Propagation of the matching signal in Q2DViewerWidget.
    void fusionLayout2x1FirstRequested(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane);
    /// Propagation of the matching signal in Q2DViewerWidget.
    void fusionLayout2x1SecondRequested(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane);
    /// Propagation of the matching signal in Q2DViewerWidget.
    void fusionLayout3x1Requested(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane);
    /// Propagation of the matching signal in Q2DViewerWidget.
    void fusionLayout2x3FirstRequested(const QList<Volume*> &volumes);
    /// Propagation of the matching signal in Q2DViewerWidget.
    void fusionLayout2x3SecondRequested(const QList<Volume*> &volumes);
    /// Propagation of the matching signal in Q2DViewerWidget.
    void fusionLayout3x3Requested(const QList<Volume*> &volumes);
    /// Propagation of the matching signal in Q2DViewerWidget.
    void fusionLayoutMprRightRequested(const QList<Volume*> &volumes);

private:
    /// Crea i retorna un nou visor configurat adequadament
    Q2DViewerWidget* getNewQ2DViewerWidget();

    /// Mètode auxiliar per un problema que ens pot provar que ens quedem sense memòria per un bug no solucionat
    void deleteQ2DViewerWidget(Q2DViewerWidget *viewer);

    /// Coloca el viewer donat en la posició i mides proporcionats
    /// @param viewer Visor que volem posicionar i ajustar dins del layout
    /// @param geometry amb les posicions i mides realitives corresponents al viewer
    void setViewerGeometry(Q2DViewerWidget *viewer, const QRectF &geometry);

    /// Fa les accions necessàries per amagar el viewer indicat del layout actual
    void hideViewer(Q2DViewerWidget *viewer);

    /// Performs the needed actions to show the given viewer of the current layout
    void showViewer(Q2DViewerWidget *viewer);

    /// Demaximized maximized viewers that overlap with the given geometry.
    void demaximizeViewersIntersectingGeometry(const QRectF &geometry);

private:
    RelativeGeometryLayout *m_layout;

    /// Visualitzador selecciona. Sempre en tindrem un.
    Q2DViewerWidget *m_selectedViewer;

    /// This struct holds the information needed to demaximize a maximized viewer.
    struct MaximizationData
    {
        QRectF normalGeometry;
        QSet<Q2DViewerWidget*> occludedViewers;
    };

    /// Maps each maximized viewer to its maximization data.
    QMap<Q2DViewerWidget*, MaximizationData> m_maximizedViewers;

};

}

#endif
