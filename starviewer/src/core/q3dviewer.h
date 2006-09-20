/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ3DVIEWER_H
#define UDGQ3DVIEWER_H

#include "qviewer.h"

// FWD declarations
class vtkRenderer;
class vtkImageCast;
class vtkRenderWindowInteractor;

namespace udg {

/**
Classe base per als visualitzadors 3D

@author Grup de Gràfics de Girona  ( GGG )
*/

// FWD declarations
class Volume;

class Q3DViewer : public QViewer{
Q_OBJECT
public:
    enum RenderFunction{ RayCasting , MIP3D, IsoSurface };
    
    Q3DViewer( QWidget *parent = 0 );
    ~Q3DViewer();

    ///\TODO implemenar bé el mètode
    virtual vtkRenderWindowInteractor *getInteractor();

    /// retorna el tipu de visualització que es té assignat
    RenderFunction getRenderFunction() const { return m_renderFunction; }

    /// retorna el tipu de visualització que es té assignat com a un string
    QString getRenderFunctionAsString();
    
    virtual void setInput( Volume* volume );

    virtual vtkRenderer *getRenderer();
    
public slots:

    virtual void render();
    /// assignem el tipus de visualització 3D que volem. RayCasting, MIP, reconstrucció de superfícies...
    void setRenderFunction(RenderFunction function){ m_renderFunction = function; };
    void setRenderFunctionToRayCasting(){ m_renderFunction = RayCasting; };
    void setRenderFunctionToMIP3D(){ m_renderFunction = MIP3D; };
    void setRenderFunctionToIsoSurface(){ m_renderFunction = IsoSurface; };
    
    /// Reinicia la vista
    void resetViewToAxial();
    void resetViewToSagital();
    void resetViewToCoronal();
    
protected:
    /// el renderer
    vtkRenderer* m_renderer;
    /// la funció que es fa servir pel rendering
    RenderFunction m_renderFunction;

private:
    /// fa la visualització per raycasting
    void renderRayCasting();

    /// fa la visualització per MIP3D
    void renderMIP3D();

    /// fa la visualització per reconstrucció de superfíces
    void renderIsoSurface();

    /// rescala les dades en el format adequat per als corresponents algorismes. Retorna fals si no hi ha cap volum assignat
    bool rescale();

    enum { Axial , Sagital , Coronal };
    /// Canvia la orientació de la càmera
    void setCameraOrientation( int orientation );
    
    /// el caster de les imatges
    vtkImageCast* m_imageCaster;

    /// Orientació que tenim
    int m_currentOrientation;

    /// reinicia la orientació
    void resetOrientation();
};

};  //  end  namespace udg 

#endif
