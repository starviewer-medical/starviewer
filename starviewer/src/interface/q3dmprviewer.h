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
class vtkLookupTable;

namespace udg {

/**
Vista de Reconstrucció multiplanar 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class Q3DMPRViewer : public QViewer{

Q_OBJECT
public:
    Q3DMPRViewer( QWidget *parent = 0 );

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

    /// Assigna la LUT en format vtk
    void setVtkLUT( vtkLookupTable * lut );

    /// Retorna la LUT en format vtk
    vtkLookupTable *getVtkLUT();
    
signals:
    /// senyal que indica que algun dels plans han canviat
    void planesHasChanged( void );
    
public slots:
    /// Inicialitza la vista de la càmara per veure el model des d'una orientació per defecte determinada ( acial , sagital o coronal )
    void resetViewToSagital();
    void resetViewToCoronal();
    void resetViewToAxial();

    /// Habilitar/Deshabilitar la visibilitat d'un dels plans
    void setSagitalVisibility( bool enable );
    void setCoronalVisibility( bool enable );
    void setAxialVisibility( bool enable );

    /// Reinicia de nou els plans
    void resetPlanes();

    /// Ajusta el window/level
    void setWindowLevel( double window , double level );

    /// Mètodes per donar diversos window level per defecte
    void resetWindowLevelToDefault();
    void resetWindowLevelToBone();
    void resetWindowLevelToSoftTissue();
    void resetWindowLevelToFat();
    void resetWindowLevelToLung();
    
    /// chapussa per agafar els events dels image plane widgets i enviar una senya conforme han canviat \TODO mirar si es pot millorar un mètode en comptes de fer això
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
