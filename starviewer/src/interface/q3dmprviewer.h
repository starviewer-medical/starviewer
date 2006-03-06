/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ3DMPRVIEWER_H
#define UDGQ3DMPRVIEWER_H

#include "qviewer.h"

class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImagePlaneWidget;
class vtkActor;
class vtkAnnotatedCubeActor;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;

namespace udg {

/**
Vista de Reconstrucció multiplanar 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class Q3DMPRViewer : public QViewer{

Q_OBJECT
public:
    Q3DMPRViewer(QWidget *parent = 0, const char *name = 0);

    ~Q3DMPRViewer();

    /// Li indiquem el volum a visualitzar
    virtual void setInput( Volume* inputImage );

    virtual vtkRenderer *getRenderer();
    virtual vtkRenderWindowInteractor *getInteractor(); 
    virtual void render();
    
    /// Retorna el volum transformat segons el reslice de cada vista
    Volume *getAxialResliceOutput();
    Volume *getSagitalResliceOutput();
    Volume *getCoronalResliceOutput();
    
    /// Mètodes per obtenir les coordenades que defineixen els plans
    double *getAxialPlaneOrigin();
    double *getAxialPlaneNormal();
    void getAxialPlaneOrigin( double origin[3] );
    void getAxialPlaneNormal( double normal[3] );
    
    double *getSagitalPlaneOrigin();
    double *getSagitalPlaneNormal();
    void getSagitalPlaneOrigin( double origin[3] );
    void getSagitalPlaneNormal( double normal[3] );
    
    double *getCoronalPlaneOrigin();
    double *getCoronalPlaneNormal();
    void getCoronalPlaneOrigin( double origin[3] );
    void getCoronalPlaneNormal( double normal[3] );
signals:
    /// senyal que indica que algun dels plans han canviat
    void planesHasChanged( void );
    
public slots:
    void resetViewToSagital();
    void resetViewToCoronal();
    void resetViewToAxial();
    
    void setSagitalVisibility( bool enable );
    void setCoronalVisibility( bool enable );
    void setAxialVisibility( bool enable );
    
    void resetPlanes();
    
    // chapussa per agafar els events dels image plane widgets i enviar una senya conforme han canviat
    void planeInteraction();
private:
    Volume *m_axialResliced, *m_sagitalResliced , *m_coronalResliced;
    /// Crea l'actor que mostra una referència de l'orientació dels eixos
    void createOrientationMarker();
    void setCameraOrientation(int orientation);
    
    
    enum {SAGITAL, CORONAL, AXIAL};

    vtkRenderer* m_ren;

    vtkImagePlaneWidget * m_axialImagePlaneWidget;
    vtkImagePlaneWidget * m_sagitalImagePlaneWidget;
    vtkImagePlaneWidget * m_coronalImagePlaneWidget;
    
    /// La bounding box del volum
    vtkActor *m_outlineActor;
    ///
    vtkAnnotatedCubeActor *m_cubeActor;
    /// connexions d'events vtk amb slots / signals qt
    vtkEventQtSlotConnect *m_vtkQtConnections; 

};

};  //  end  namespace udg 

#endif
