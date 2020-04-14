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

#ifndef UDGCURSOR3DTOOL_H
#define UDGCURSOR3DTOOL_H

#include "tool.h"

namespace udg {

class Cursor3DToolData;
class Q2DViewer;
class DrawerCrossHair;
class SliceLocator;

/**
    Implementació de la tool del cursor 3D.
  */
class Cursor3DTool : public Tool {
Q_OBJECT
public:
    /// Estats de la tool
    enum { None, Computing };

    explicit Cursor3DTool(QViewer *viewer, QObject *parent = nullptr);
    ~Cursor3DTool() override;

    void setToolData(ToolData *data) override;

    void handleEvent(long unsigned eventID) override;

    void setVisibility(bool visible);

private slots:
    /// Actualitza les línies a projectar sobre la imatge segons les dades de la tool
    void updateProjectedPoint();

    /// Actualitza el frame of reference de les dades a partir del volum donat
    void updateFrameOfReference();

    /// Aquest slot es crida per actualitzar les dades que marquen quin és el pla de referència
    /// Es cridarà quan el viewer sigui actiu o quan el viewer actiu canvïi d'input
    void refreshReferenceViewerData();

    /// Actualitza la posició de la tool
    void updatePosition();

    /// Gestiona les accions a fer amb el crossHair al canviar de llesca
    void handleImageChange();

    /// Amaga el cursor 3D
    void hideCursor();

private:
    /// Projecta el pla de referència sobre el pla de localitzador
    void projectPoint();

    /// Inicialitza la posició de la tool
    void initializePosition();

    /// Desactiva la tool
    void removePosition();

    /// Crea una nova primitiva pel cursor 3D
    void createNewCrossHair();

private:
    /// Dades específiques de la tool
    Cursor3DToolData *m_myData;

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Ens guardem el frame of reference del nostre viewer, per no haver de "preguntar cada cop"
    QString m_myFrameOfReferenceUID;

    /// Ens guardem el l'instance UID del nostre viewer, per no haver de "preguntar cada cop"
    QString m_myInstanceUID;

    /// Controlar l'estat de la tool
    int m_state;

    /// Objecte crosshair per representar el punt
    DrawerCrossHair *m_crossHair;

    /// Class to locate the corresponding slice to the cursor 3D point
    SliceLocator *m_sliceLocator;
};

}

#endif
