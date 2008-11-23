/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUME3DFUSEDVIEWER_H
#define UDGVOLUME3DFUSEDVIEWER_H

#include "qviewer.h"

// FWD declarations
class vtkRenderer;
class vtkImageCast;
class vtkRenderWindowInteractor;
class vtkInteractorStyle;
class vtkVolumeRayCastMapper;
class vtkVolume;
class vtkVolumeProperty;
class vtkLightKit;
class vtkLight;
class vtkImageData;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastCompositeFunctionFx;




namespace udg {

// FWD declarations
class Volume;
class Q3DOrientationMarker;
class TransferFunction;
class AmbientVoxelShader;
class DirectIlluminationVoxelShader;
class ContourVoxelShader;
class vtkVolumeRayCastCompositeFxFunction;
class FusionVoxelShader;

/**
Classe base per als visualitzadors 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class Volume3DFusedViewer : public QViewer{
Q_OBJECT
public:
    enum RenderFunction{ RayCasting , MIP3D, IsoSurface , Texture2D , Texture3D, Contouring };

    Volume3DFusedViewer( QWidget *parent = 0 );
    ~Volume3DFusedViewer();

    /// retorna el tipu de visualització que es té assignat
    RenderFunction getRenderFunction() const { return m_renderFunction; }

    /// retorna el tipu de visualització que es té assignat com a un string
    QString getRenderFunctionAsString();

    virtual void setInput( Volume* volume );
	
	/// Li donem la imatge a processar
    void setImage( vtkImageData * image );
	void setShade( bool on );
	
    virtual vtkRenderer *getRenderer();
	void render2();
	vtkVolume * getVtkVolume();
	vtkVolumeProperty * getVtkVolumeProperty();
	TransferFunction * getTransferFunction();
    void getCurrentWindowLevel( double wl[2] );
    void resetView( CameraOrientationType view );
	void deepCopy(Volume3DFusedViewer * volume3DFused);
 	void showLights();
	void fusion( int option, double *range,unsigned char * data1, unsigned char *data2, TransferFunction *tranferFunction1, TransferFunction *tranferFunction2 );


public slots:
    /// assignem el tipus de visualització 3D que volem. RayCasting, MIP, reconstrucció de superfícies...
    void setRenderFunction(RenderFunction function);
    void setRenderFunctionToRayCasting();
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
	void activateGradient();
	void deactivateGradient();
    void setWindowLevel( double window , double level );

private:
    /// fa la visualització per raycasting
    void renderRayCasting();

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

	///Paràmetre per activar el gradient
	bool m_gradient;

    /// el caster de les imatges
    vtkImageCast* m_imageCaster;

    /// Widget per veure la orientació en 3D
    Q3DOrientationMarker *m_orientationMarker;

	///VtkImage del model
	vtkImageData *m_vtkImage;

	///Funció RayCasting Principal
	vtkVolumeRayCastCompositeFunction *m_mainVolumeRayCastFunction;

	///Mapper del volum
	vtkVolumeRayCastMapper* m_volumeMapper;
	
    /// El prop 3D de vtk per representar el volum
    vtkVolume *m_vtkVolume;

    /// Propietats que defineixen com es renderitzarà el volum
    vtkVolumeProperty *m_volumeProperty;

    /// La funció de transferència que s'aplica
    TransferFunction *m_transferFunction;

	///Kit de llums de l'escena
 	vtkLightKit *m_lights;

	///llum de l'escena
 	vtkLight *m_light;

	///dades del model
	unsigned char *m_vtkData;

	///Mida de les dades
	int m_vtkDataSize;

	vtkVolumeRayCastCompositeFxFunction *m_volumeRayCastFunctionFx2;

	/// Ambient voxel shader.
	FusionVoxelShader * m_fusionVoxelShader;

	/// Ambient voxel shader.
    AmbientVoxelShader *m_ambientVoxelShader;
    /// Direct illumination voxel shader.
    DirectIlluminationVoxelShader *m_directIlluminationVoxelShader;
    /// Contour voxel shader.
    ContourVoxelShader *m_contourVoxelShader;
};

};  //  end  namespace udg

#endif
