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

#ifndef UDGQ3DMPRVIEWER_H
#define UDGQ3DMPRVIEWER_H

#include "qviewer.h"

class vtkImagePlaneWidget;
class vtkActor;

namespace udg {

// FWD declarations
class Q3DOrientationMarker;

/**
    Visor de Reconstrucció multiplanar 3D
  */
class Q3DMPRViewer : public QViewer {
Q_OBJECT
public:
    Q3DMPRViewer(QWidget *parent = 0);
    ~Q3DMPRViewer();

    /// Retorna el volum transformat segons el reslice de cada vista
    Volume* getAxialResliceOutput();
    Volume* getSagitalResliceOutput();
    Volume* getCoronalResliceOutput();

    /// Mètodes per obtenir les coordenades que defineixen els plans
    double* getAxialPlaneOrigin();
    double* getAxialPlaneNormal();
    void getAxialPlaneOrigin(double origin[3]);
    void getAxialPlaneNormal(double normal[3]);

    double* getSagitalPlaneOrigin();
    double* getSagitalPlaneNormal();
    void getSagitalPlaneOrigin(double origin[3]);
    void getSagitalPlaneNormal(double normal[3]);

    double* getCoronalPlaneOrigin();
    double* getCoronalPlaneNormal();
    void getCoronalPlaneOrigin(double origin[3]);
    void getCoronalPlaneNormal(double normal[3]);

    void getCurrentWindowLevel(double wl[2]);

signals:
    /// Senyal que indica que algun dels plans han canviat
    void planesHasChanged(void);

public slots:
    /// Li indiquem el volum a visualitzar
    virtual void setInput(Volume *inputImage);

    void resetViewToAxial();
    void resetViewToSagital();
    void resetViewToCoronal();

    /// Habilitar/Deshabilitar la visibilitat d'un dels plans
    void setSagitalVisibility(bool enable);
    void setCoronalVisibility(bool enable);
    void setAxialVisibility(bool enable);

    /// Reinicia de nou els plans
    virtual void resetPlanes();

    void setWindowLevel(double window, double level);

    /// Mètodes per controlar la visibilitat de l'outline
    void enableOutline(bool enable);
    void outlineOn();
    void outlineOff();

    /// Mètodes per controlar la visibilitat de l'orientation marker widget
    void enableOrientationMarker(bool enable);
    void orientationMarkerOn();
    void orientationMarkerOff();

    /// Chapussa per agafar els events dels image plane widgets i enviar una senya conforme han canviat \TODO mirar si es pot millorar
    /// un mètode en comptes de fer això
    void planeInteraction();

protected:
    void getCurrentRenderedItemBounds(double bounds[6]);
    
    /// Inicialitza els plans
    void initializePlanes();

    /// Actualitza les dades sobre les que tracten els plans
    virtual void updatePlanesData();

    /// Afegeix l'outline de la boundingbox del model
    virtual void createOutline();

    /// Crea tots els actors que intervenen en l'escena
    void createActors();

    /// Afegeix els actors a l'escena
    void addActors();

protected:
    /// Els respectius volums sobre cada pla de reslice
    Volume *m_axialResliced;
    Volume *m_sagitalResliced;
    Volume *m_coronalResliced;
    
    /// Els plans
    vtkImagePlaneWidget *m_axialImagePlaneWidget;
    vtkImagePlaneWidget *m_sagitalImagePlaneWidget;
    vtkImagePlaneWidget *m_coronalImagePlaneWidget;

    /// La bounding box del volum
    vtkActor *m_outlineActor;

    /// Widget per veure la orientació en 3D
    Q3DOrientationMarker *m_orientationMarker;

    /// Control de visibilitat dels plans
    bool m_axialPlaneVisible, m_sagitalPlaneVisible, m_coronalPlaneVisible;

    /// Control de visibilitat de l'outline i l'orientation marker widget \TODO és possible que aquests membres acabin sent superflus i innecessaris
    bool m_isOutlineEnabled;
};

};  // End namespace udg

#endif
