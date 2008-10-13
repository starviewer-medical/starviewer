/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volume3dfusedviewer.h"
#include "volume.h"
#include "logging.h"
#include "q3dorientationmarker.h"
#include "transferfunction.h"

#include <iostream.h>

// include's qt
#include <QString>

// include's vtk
#include <QVTKWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPointData.h>
// rendering 3D
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>

#include <vtkImageData.h>

// Ray Cast
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include "vtkVolumeRayCastCompositeFunctionFx.h"
#include "vtkVolumeRayCastCompositeFxFunction.h"
#include "vtk4DLinearRegressionGradientEstimator.h"


// MIP
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
//Contouring
#include <vtkPolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkReverseSense.h>
#include <vtkImageShrink3D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkProperty.h>
#include <vtkDecimatePro.h>
// IsoSurface
#include <vtkVolumeRayCastIsosurfaceFunction.h>
// Texture2D i Texture3D
#include <vtkVolumeTextureMapper2D.h>
#include <vtkVolumeTextureMapper3D.h>
// LUT's
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
// Casting
#include <vtkImageCast.h>
#include <vtkImageShiftScale.h>

// Kit de Llums
#include <vtkLightKit.h>
#include <vtkLight.h>

// interacció
#include <vtkInteractorStyle.h>
#include <vtkInteractorObserver.h>

#include <vtkImageViewer.h>

//Voxel Shaders

#include "voxelshader.h"
#include "ambientvoxelshader.h"
#include "directilluminationvoxelshader.h"
#include <vtkEncodedGradientShader.h>
#include "fusionvoxelshader.h"


namespace udg {

Volume3DFusedViewer::Volume3DFusedViewer( QWidget *parent )
 : QViewer( parent ), m_vtkVolume(0), m_volumeProperty(0), m_transferFunction(0)
{
    // Creem el Renderer de VTK i li assignem al widget que ens associa Qt amb VTK
    m_renderer = vtkRenderer::New();
   
    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );

    m_renderFunction = RayCasting; // per defecte

    m_imageCaster = vtkImageCast::New();
    m_currentOrientation = Axial;
   
	//m_orientationMarker->setEnabled(true);

    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    // despatxa qualsevol event-> tools
    m_vtkQtConnections->Connect( m_vtkWidget->GetRenderWindow()->GetInteractor(),
                                 vtkCommand::AnyEvent,
                                 this,
#ifdef VTK_QT_5_0_SUPPORT
                                 SLOT( eventHandler(vtkObject*, unsigned long, void*, vtkCommand*) )
#else
                                 SLOT( eventHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*) )
#endif
                                 );
    // \TODO fer això aquí? o fer-ho en el tool manager?
    this->getInteractor()->RemoveObservers( vtkCommand::LeftButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::RightButtonPressEvent );

    m_vtkVolume = vtkVolume::New();
    m_volumeProperty = vtkVolumeProperty::New();
    m_volumeProperty->SetInterpolationTypeToLinear();
    m_vtkVolume->SetProperty( m_volumeProperty );
    m_renderer->AddVolume( m_vtkVolume );


    m_transferFunction = new TransferFunction;
    // creem una funció de transferència per defecte TODO la tenim només per tenir alguna cosa per defecte
    // Opacitat
    m_transferFunction->addPointToOpacity( 20, .0 );
    m_transferFunction->addPointToOpacity( 255, 1.0 );
    // Colors
    m_transferFunction->addPointToColorRGB( 0.0, 0.0, 0.0, 0.0 );
    m_transferFunction->addPointToColorRGB( 64.0, 1.0, 0.0, 0.0 );
    m_transferFunction->addPointToColorRGB( 128.0, 0.0, 0.0, 1.0 );
    m_transferFunction->addPointToColorRGB( 192.0, 0.0, 1.0, 0.0 );
    m_transferFunction->addPointToColorRGB( 255.0, 0.0, 0.2, 0.0 );

    m_volumeProperty->SetColor( m_transferFunction->getColorTransferFunction() );
    m_volumeProperty->SetScalarOpacity( m_transferFunction->getOpacityTransferFunction() );
	//this->showLights();
	m_orientationMarker = new Q3DOrientationMarker( this->getInteractor() );
	this->deactivateGradient();
 	m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
    this->getInteractorStyle()->SetCurrentRenderer( m_renderer );
}

Volume3DFusedViewer::~Volume3DFusedViewer()
{
    m_renderer->Delete();
	m_imageCaster->Delete();

	///Funció RayCasting Principal
	m_mainVolumeRayCastFunction->Delete();

	///Mapper del volum
	m_volumeMapper->Delete();
	
    /// El prop 3D de vtk per representar el volum
    m_vtkVolume->Delete();

    /// Propietats que defineixen com es renderitzarà el volum
    m_volumeProperty->Delete();

	///Kit de llums de l'escena
 	m_lights->Delete();

	///llum de l'escena
 	m_light->Delete();

	m_volumeRayCastFunctionFx2->Delete();

}

void Volume3DFusedViewer::setImage(vtkImageData * image)
{
	m_vtkImage=image;
}

void Volume3DFusedViewer::setInput( Volume* volume )
{
    m_mainVolume = volume;

	m_volumeRayCastFunctionFx2 = vtkVolumeRayCastCompositeFxFunction::New();
    m_volumeRayCastFunctionFx2->AddVoxelShader( m_ambientVoxelShader );

}

void Volume3DFusedViewer::setShade( bool on )
{
   if ( on )
    {
        m_volumeProperty->ShadeOn();
       
    }
    else
    {
        m_volumeProperty->ShadeOff();
    }
}

vtkRenderer *Volume3DFusedViewer::getRenderer()
{
    return m_renderer;
}

void Volume3DFusedViewer::deepCopy(Volume3DFusedViewer * volume3DFused)
{
}

void Volume3DFusedViewer::getCurrentWindowLevel( double wl[2] )
{
    // TODO estem obligats a implementar-lo. De moment retornem 0,0
    wl[0] = wl[1] = 0.0;
}

void Volume3DFusedViewer::resetView( CameraOrientationType view )
{
    m_currentOrientation = view;
    //TODO replantejar si necessitem aquest mètode i el substituïm per aquest mateix
    resetOrientation();
}

void Volume3DFusedViewer::setRenderFunction(RenderFunction function)
{
    m_renderFunction = function;
}

void Volume3DFusedViewer::setRenderFunctionToRayCasting()
{
    m_renderFunction = RayCasting;
}

void Volume3DFusedViewer::setRenderFunctionToContouring()
{
    m_renderFunction = Contouring;
}

void Volume3DFusedViewer::setRenderFunctionToMIP3D()
{
    m_renderFunction = MIP3D;
}

void Volume3DFusedViewer::setRenderFunctionToIsoSurface()
{
    m_renderFunction = IsoSurface;
}

void Volume3DFusedViewer::setRenderFunctionToTexture2D()
{
    m_renderFunction = Texture2D;
}

void Volume3DFusedViewer::setRenderFunctionToTexture3D()
{
    m_renderFunction = Texture3D;
}

QString Volume3DFusedViewer::getRenderFunctionAsString()
{
    QString result;
    switch( m_renderFunction )
    {
    case RayCasting:
        result = "RayCasting";
    break;
    case MIP3D:
        result = "MIP 3D";
    break;
    case IsoSurface:
        result = "IsoSurface";
    break;
    case Texture2D:
        result = "Texture2D";
    break;
    case Texture3D:
        result = "Texture3D";
    break;
    case Contouring:
        result = "Contouring";
    break;
    }
    return result;
}


void Volume3DFusedViewer::render()
{
    if( m_mainVolume )
    {
		cout << "entra" << endl;
        switch( m_renderFunction )
        {
        case Contouring:
            renderContouring();
        break;
        case RayCasting:
            renderRayCasting();
        break;
        case MIP3D:
            renderMIP3D();
        break;
        case IsoSurface:
            renderIsoSurface();
        break;
        case Texture2D:
            renderTexture2D();
        break;
        case Texture3D:
            renderTexture3D();
        break;
        }
        this->resetOrientation();
    }
    else
    {
        WARN_LOG("Volume3DFusedViewer:: Cridant a render() sense haver donat cap input");
    }
}

///METODE DE PROVES!!!!!!!!!!

vtkVolume * Volume3DFusedViewer::getVtkVolume()
{
	return m_vtkVolume;
}
vtkVolumeProperty * Volume3DFusedViewer::getVtkVolumeProperty()
{
	return m_volumeProperty;
}
TransferFunction * Volume3DFusedViewer::getTransferFunction()
{
	return m_transferFunction;
}

void Volume3DFusedViewer::showLights()
{

	m_light = vtkLight::New();
	vtkLight * light2 = vtkLight::New();
	m_light->SetFocalPoint(50.0,50.0,0.0);
	m_light->SetPosition(10.0,-10.0,-30.0);
	m_light->SetColor(0.0,1.0,0.0);
// 	m_light->SetIntensity(1.0);
// 	m_light->SetLightTypeToSceneLight();
	

	light2->SetFocalPoint(1.0,0.0,1.0);
	light2->SetPosition(10.0,1.0,1.0);
	light2->SetColor(1.0,0.0,0.0);
// 	light2->SetAmbientColor(1.0,0.0,0.0);
// 	light2->SetSpecularColor(1.0,0.0,0.0);
// 	light2->SetDiffuseColor(1.0,0.0,0.0);
// 	light2->SetIntensity(1.0);
// 	light2->SwitchOn();
	light2->SetLightTypeToSceneLight();
	//m_renderer->TwoSidedLightingOn();
	m_renderer->AddLight(m_light);
	m_renderer->AddLight(light2);
	//m_light->SwitchOn();
	//m_renderer->SetAmbient(1.0,1.0,1.0);
	//m_renderer->SetTwoSidedLighting(2);
/*
	m_lights = vtkLightKit::New();
	m_lights->AddLightsToRenderer (m_renderer);
	m_lights->Update();*/

}

void Volume3DFusedViewer::reset()
{
    // \TODO implementar tot el que falti
    m_currentOrientation = Axial;
    this->resetOrientation();
}

void Volume3DFusedViewer::setTransferFunction( TransferFunction *transferFunction )
{
    m_transferFunction = transferFunction;
    m_volumeProperty->SetScalarOpacity( m_transferFunction->getOpacityTransferFunction() );
    m_volumeProperty->SetColor( m_transferFunction->getColorTransferFunction() );
}

void Volume3DFusedViewer::activateGradient()
{
	m_gradient=true;
}
void Volume3DFusedViewer::deactivateGradient()
{
	m_gradient=false;
}

void Volume3DFusedViewer::setWindowLevel( double, double )
{
    // TODO estem obligats a implementar-lo.
}

void Volume3DFusedViewer::resetOrientation()
{
    switch( m_currentOrientation )
    {
    case Axial:
        this->resetViewToAxial();
    break;

    case Sagital:
        this->resetViewToSagital();
    break;

    case Coronal:
        this->resetViewToCoronal();
    break;

    default:
        DEBUG_LOG("Volume3DFusedViewer: m_currentOrientation no és cap de les tres esperades ( Axial,Sagital,Coronal). Agafem Axial per defecte");
        this->resetViewToAxial();
    break;
    }
}

bool Volume3DFusedViewer::rescale()
{
    bool ok = false;
    if( m_mainVolume )
    {
        m_imageCaster->SetInput(  m_mainVolume->getVtkData() );
        m_imageCaster->SetOutputScalarType( VTK_UNSIGNED_CHAR ); // tbé seria vàlid VTK_UNSIGNED_SHORT
        m_imageCaster->ClampOverflowOn();
        m_imageCaster->Update();
        ok = true;
	}
    return ok;
}


void Volume3DFusedViewer::fusion( int option, double *range,unsigned char* data1,unsigned char* data2, TransferFunction *transferFunction1, TransferFunction *transferFunction2 )
{
	///Metode per fer la fusió de propietats.
	cout << "entro al fusion" << endl;

	if (rescale() )
	{
		m_volumeProperty->DisableGradientOpacityOn();
        m_volumeProperty->ShadeOff();

	
		m_fusionVoxelShader = new FusionVoxelShader();
		m_fusionVoxelShader->setData( data1 );
		m_fusionVoxelShader->setData2( data2 );
		m_fusionVoxelShader->setTransferFunction( *transferFunction1 );
		m_fusionVoxelShader->setTransferFunction2( *transferFunction2 );
	
		
		m_volumeRayCastFunctionFx2 = vtkVolumeRayCastCompositeFxFunction::New();
		m_volumeRayCastFunctionFx2->SetCompositeMethodToClassifyFirst();

		m_volumeRayCastFunctionFx2->AddVoxelShader( m_fusionVoxelShader );

		m_volumeMapper=vtkVolumeRayCastMapper::New();
	
		m_volumeMapper->SetVolumeRayCastFunction( m_volumeRayCastFunctionFx2 );	
		m_volumeMapper->SetInput( m_imageCaster->GetOutput() );
	
 		vtk4DLinearRegressionGradientEstimator *gradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
 		m_volumeMapper->SetGradientEstimator( gradientEstimator );
	
		gradientEstimator->Delete();
		m_vtkVolume->SetMapper( m_volumeMapper );		
		m_vtkWidget->GetRenderWindow()->Render();
	}	
	
}



void Volume3DFusedViewer::renderRayCasting()
{
    if( rescale() )
    {
         m_volumeProperty->DisableGradientOpacityOn();
         m_volumeProperty->ShadeOff();

		m_mainVolumeRayCastFunction = vtkVolumeRayCastCompositeFunction::New();
		m_mainVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
		
 		m_volumeMapper=vtkVolumeRayCastMapper::New();

		///Falta assignar la funció ray cast que fa servir els voxelshaders!!!!!!!!!!

 		m_volumeMapper->SetVolumeRayCastFunction( m_mainVolumeRayCastFunction );	
 		m_volumeMapper->SetInput( m_imageCaster->GetOutput() );

		if(m_gradient)
		{
			cout << "fem servir gradient" << endl;
	    	vtk4DLinearRegressionGradientEstimator *gradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
		    m_volumeMapper->SetGradientEstimator( gradientEstimator );	
    		gradientEstimator->Delete();
	        m_volumeProperty->ShadeOn();

		}
		else m_volumeProperty->ShadeOff();
		m_vtkVolume->SetMapper( m_volumeMapper );

        m_renderer->Render();
		cout << "render" << endl;

    }
    else
        DEBUG_LOG( "No es pot fer render per ray casting, no s'ha proporcionat cap volum d'entrada" );
}


void Volume3DFusedViewer::renderMIP3D()
{
    if( rescale() )
    {
        // quan fem MIP3D deixarem disable per defecte ja que la orientació no la sabem ben bé quina és ja que el pla de tall pot ser arbitrari \TODO no sempre un mip serà sobre un pla mpr, llavors tampoc és del tot correcte decidir això aquí
//         m_orientationMarker->disable();
        //================================================================================================
        // Create a transfer function mapping scalar value to opacity
        // assignem una rampa d'opacitat total per valors alts i nula per valors petits
        // després en l'escala de grisos donem un  valor de gris constant ( blanc )

        //\TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction

        // Creem la funció de transferència de l'opacitat
        m_transferFunction->addPointToOpacity( 20, .0 );
        m_transferFunction->addPointToOpacity( 255, 1. );

        // Creem la funció de transferència de colors
        // Create a transfer function mapping scalar value to color (grey)
        vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
        grayTransferFunction->AddSegment( 0 , 0.0 , 255 , 1.0 );

        m_volumeProperty->SetScalarOpacity( m_transferFunction->getOpacityTransferFunction() );
        m_volumeProperty->SetColor( grayTransferFunction /*m_transferFunction->getColorTransferFunction()*/ );
        m_volumeProperty->ShadeOff();

        // creem la funció del raig MIP, en aquest cas maximitzem l'opacitat, si fos Scalar value, ho faria pel valor
        vtkVolumeRayCastMIPFunction* mipFunction = vtkVolumeRayCastMIPFunction::New();
        mipFunction->SetMaximizeMethodToOpacity();

//         vtkFiniteDifferenceGradientEstimator *gradientEstimator = vtkFiniteDifferenceGradientEstimator::New();
        vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();

        volumeMapper->SetVolumeRayCastFunction( mipFunction );
        volumeMapper->SetInput( m_imageCaster->GetOutput()  );
//         volumeMapper->SetGradientEstimator( gradientEstimator );

        m_vtkVolume->SetMapper( volumeMapper );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per MIP, no s'ha proporcionat cap volum d'entrada" );
}

void Volume3DFusedViewer::renderContouring()
{
    if ( m_mainVolume != 0 )
    {
        vtkImageShrink3D *m_shrink = vtkImageShrink3D::New();
        m_shrink->SetInput( m_mainVolume->getVtkData() );
        vtkImageGaussianSmooth *m_smooth = vtkImageGaussianSmooth::New();
        m_smooth->SetDimensionality( 3 );
        m_smooth->SetRadiusFactor( 2 );
        m_smooth->SetInput( m_shrink->GetOutput() );

        vtkContourFilter *contour = vtkContourFilter::New();
        contour->SetInputConnection( m_smooth->GetOutputPort());
        contour->GenerateValues( 1, 30, 30);
        contour->ComputeScalarsOff();
        contour->ComputeGradientsOff();

        vtkDecimatePro *decimator = vtkDecimatePro::New();
        decimator->SetInputConnection( contour->GetOutputPort() );
        decimator->SetTargetReduction( 0.9 );
        decimator->PreserveTopologyOn();

        vtkReverseSense *reverse = vtkReverseSense::New();
        reverse->SetInputConnection(decimator->GetOutputPort());
        reverse->ReverseCellsOn();
        reverse->ReverseNormalsOn();

        vtkPolyDataMapper *m_polyDataMapper = vtkPolyDataMapper::New();

        m_polyDataMapper->SetInputConnection( reverse->GetOutputPort() );
        m_polyDataMapper->ScalarVisibilityOn();
        m_polyDataMapper->ImmediateModeRenderingOn();

        vtkActor *m_3DActor = vtkActor::New();
        m_3DActor->SetMapper( m_polyDataMapper );
        m_3DActor->GetProperty()->SetColor(1,0.8,0.81);

        m_renderer->AddActor( m_3DActor );
        m_renderer->Render();

        decimator->Delete();
        m_3DActor->Delete();
        m_polyDataMapper->Delete();
        contour->Delete();
        m_smooth->Delete();
        m_shrink->Delete();
        reverse->Delete();
    }
    else
        DEBUG_LOG( "No s'ha proporcionat cap volum d'entrada" );
}

void Volume3DFusedViewer::renderIsoSurface()
{
    if( rescale() )
    {
        //\TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction
        // Create a transfer function mapping scalar value to opacity
        vtkPiecewiseFunction *oTFun = vtkPiecewiseFunction::New();
        oTFun->AddSegment(10, 0.0, 255, 0.3);

        vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
        opacityTransferFunction->AddSegment(  0, 0.0, 128, 1.0);
        opacityTransferFunction->AddSegment(128, 1.0, 255, 0.0);

        // Create a transfer function mapping scalar value to color (grey)
        vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
        grayTransferFunction->AddSegment(0, 1.0, 255, 1.0);

        // Create a transfer function mapping scalar value to color (color)
        vtkColorTransferFunction *cTFun = vtkColorTransferFunction::New();
        cTFun->AddRGBPoint(   0, 1.0, 0.0, 0.0 );
        cTFun->AddRGBPoint(  64, 1.0, 1.0, 0.0 );
        cTFun->AddRGBPoint( 128, 0.0, 1.0, 0.0 );
        cTFun->AddRGBPoint( 192, 0.0, 1.0, 1.0 );
        cTFun->AddRGBPoint( 255, 0.0, 0.0, 1.0 );

        // Create a transfer function mapping magnitude of gradient to opacity
        vtkPiecewiseFunction *goTFun = vtkPiecewiseFunction::New();
        goTFun->AddPoint(   0, 0.0 );
        goTFun->AddPoint(  30, 0.0 );
        goTFun->AddPoint(  40, 1.0 );
        goTFun->AddPoint( 255, 1.0 );

        m_volumeProperty->ShadeOn();
        m_volumeProperty->SetAmbient(0.3);
        m_volumeProperty->SetDiffuse(1.0);
        m_volumeProperty->SetSpecular(0.2);
        m_volumeProperty->SetSpecularPower(50.0);

        m_volumeProperty->SetScalarOpacity(oTFun);
        m_volumeProperty->DisableGradientOpacityOff();
        m_volumeProperty->SetGradientOpacity( goTFun );
        m_volumeProperty->SetColor( cTFun );
//         m_volumeProperty->SetColor( grayTransferFunction );
        m_volumeProperty->SetInterpolationTypeToLinear(); //prop[index]->SetInterpolationTypeToNearest();

        // Create an isosurface ray function
        vtkVolumeRayCastIsosurfaceFunction *isosurfaceFunction = vtkVolumeRayCastIsosurfaceFunction::New();
        isosurfaceFunction->SetIsoValue(80);
        vtkFiniteDifferenceGradientEstimator *gradientEstimator = vtkFiniteDifferenceGradientEstimator::New();

        vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
        volumeMapper->SetVolumeRayCastFunction( isosurfaceFunction );
        volumeMapper->SetInput( m_imageCaster->GetOutput()  ); // abans inputImage->getVtkData()
        volumeMapper->SetGradientEstimator( gradientEstimator );

        m_vtkVolume->SetMapper( volumeMapper );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per IsoSurface, no s'ha proporcionat cap volum d'entrada" );
}

void Volume3DFusedViewer::renderTexture2D()
{
    if( rescale() )
    {
        m_volumeProperty->DisableGradientOpacityOn();
        m_volumeProperty->ShadeOff();

        vtkVolumeTextureMapper2D* volumeMapper = vtkVolumeTextureMapper2D::New();
        volumeMapper->SetInput( m_imageCaster->GetOutput()  );

        m_vtkVolume->SetMapper( volumeMapper );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per textures 2D, no s'ha proporcionat cap volum d'entrada" );
}

void Volume3DFusedViewer::renderTexture3D()
{
    if( rescale() )
    {
        m_volumeProperty->DisableGradientOpacityOn();
        m_volumeProperty->ShadeOff();

        vtkVolumeTextureMapper3D* volumeMapper = vtkVolumeTextureMapper3D::New();
        volumeMapper->SetInput( m_imageCaster->GetOutput()  );

        m_vtkVolume->SetMapper( volumeMapper );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per textures 3D, no s'ha proporcionat cap volum d'entrada" );
}

void Volume3DFusedViewer::resetViewToAxial()
{
    this->setCameraOrientation( Axial );
    m_currentOrientation = Axial;
}

void Volume3DFusedViewer::resetViewToSagital()
{
    this->setCameraOrientation( Sagital );
    m_currentOrientation = Sagital;
}

void Volume3DFusedViewer::resetViewToCoronal()
{
    this->setCameraOrientation( Coronal );
    m_currentOrientation = Coronal;
}

void Volume3DFusedViewer::enableOrientationMarker( bool enable )
{
    m_orientationMarker->setEnabled( enable );
}

void Volume3DFusedViewer::orientationMarkerOn()
{
    this->enableOrientationMarker( true );
}

void Volume3DFusedViewer::orientationMarkerOff()
{
    this->enableOrientationMarker( false );
}

};  // end namespace udg {
