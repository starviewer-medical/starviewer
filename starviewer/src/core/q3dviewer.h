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
class vtkInteractorStyle;
class vtkVolume;
class vtkVolumeProperty;

namespace udg {

// FWD declarations
class Volume;
class Q3DOrientationMarker;
class TransferFunction;

/**
Classe base per als visualitzadors 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class Q3DViewer : public QViewer{
Q_OBJECT
public:
    enum RenderFunction{ RayCasting, RayCastingShading,
                         MIP3D, IsoSurface , Texture2D , Texture3D, Contouring };

    Q3DViewer( QWidget *parent = 0 );
    ~Q3DViewer();

    /// retorna el tipu de visualització que es té assignat
    RenderFunction getRenderFunction() const { return m_renderFunction; }

    /// retorna el tipu de visualització que es té assignat com a un string
    QString getRenderFunctionAsString();

    virtual void setInput( Volume* volume );

    virtual vtkRenderer *getRenderer();

    void getCurrentWindowLevel( double wl[2] );
    void resetView( CameraOrientationType view );

public slots:
    /// assignem el tipus de visualització 3D que volem. RayCasting, MIP, reconstrucció de superfícies...
    void setRenderFunction(RenderFunction function);
    void setRenderFunctionToRayCasting();
    void setRenderFunctionToRayCastingShading();
    void setRenderFunctionToMIP3D();
    void setRenderFunctionToIsoSurface();
    void setRenderFunctionToTexture2D();
    void setRenderFunctionToTexture3D();
    void setRenderFunctionToContouring();

    void resetViewToAxial();
    void resetViewToSagital();
    void resetViewToCoronal();

    /// mètodes per controlar la visibilitat de l'orientation marker widget
    void enableOrientationMarker( bool enable );
    void orientationMarkerOn();
    void orientationMarkerOff();

    virtual void render();
    void reset();

    /// Li assignem la funció de transferència que volem aplicar
    void setTransferFunction( TransferFunction *transferFunction );
    void setWindowLevel( double window , double level );

    /// Paràmetres d'especularitat
    void setSpecular( bool on );
    void setSpecularPower( double power );

private:
    /// fa la visualització per raycasting
    void renderRayCasting();

    /// fa la visualització per raycasting amb shading i opcionalment especularitat
    void renderRayCastingShading();

    /// fa la visualització per contouring
    void renderContouring();

    /// fa la visualització per MIP3D
    void renderMIP3D();

    /// fa la visualització per reconstrucció de superfíces
    void renderIsoSurface();

    /// fa la visualització per textures 2D \TODO afegir comprovació de si el hard o suporta o no
    void renderTexture2D();

    /// fa la visualització per textures 3D \TODO afegir comprovació de si el hard o suporta o no
    void renderTexture3D();

    /// rescala les dades en el format adequat per als corresponents algorismes. Retorna fals si no hi ha cap volum assignat
    bool rescale();

    /// Orientació que tenim
    int m_currentOrientation;

    /// reinicia la orientació
    void resetOrientation();

protected:
    /// el renderer
    vtkRenderer* m_renderer;

    /// la funció que es fa servir pel rendering
    RenderFunction m_renderFunction;

private:
    /// el caster de les imatges
    vtkImageCast* m_imageCaster;

    /// Widget per veure la orientació en 3D
    Q3DOrientationMarker *m_orientationMarker;

    /// El prop 3D de vtk per representar el volum
    vtkVolume *m_vtkVolume;

    /// Propietats que defineixen com es renderitzarà el volum
    vtkVolumeProperty *m_volumeProperty;

    /// La funció de transferència que s'aplica
    TransferFunction *m_transferFunction;

};

};  //  end  namespace udg

#endif
