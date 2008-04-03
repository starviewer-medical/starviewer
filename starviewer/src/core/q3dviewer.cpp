/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dviewer.h"
#include "volume.h"
#include "logging.h"
#include "q3dorientationmarker.h"
#include "transferfunction.h"

// include's qt
#include <QString>

// include's vtk
#include <QVTKWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>
// rendering 3D
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
// Ray Cast
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
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

// interacció
#include <vtkInteractorStyle.h>
#include <vtkInteractorObserver.h>

#include <vtkImageViewer.h>

namespace udg {

Q3DViewer::Q3DViewer( QWidget *parent )
 : QViewer( parent ), m_vtkVolume(0), m_volumeProperty(0), m_transferFunction(0)
{
    // Creem el Renderer de VTK i li assignem al widget que ens associa Qt amb VTK
    m_renderer = vtkRenderer::New();
    m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
    this->getInteractorStyle()->SetCurrentRenderer( m_renderer );
    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );

    m_renderFunction = Contouring; // per defecte

    m_imageCaster = vtkImageCast::New();
    m_currentOrientation = Axial;
    m_orientationMarker = new Q3DOrientationMarker( this->getInteractor() );

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

    // creem el pipeline del volum
    m_vtkVolume = vtkVolume::New();
    m_volumeProperty = vtkVolumeProperty::New();
    m_volumeProperty->SetInterpolationTypeToLinear();
    m_vtkVolume->SetProperty( m_volumeProperty );
    m_renderer->AddViewProp( m_vtkVolume );

    m_transferFunction = new TransferFunction;
    // creem una funció de transferència per defecte TODO la tenim només per tenir alguna cosa per defecte
    // Opacitat
    m_transferFunction->addPointToOpacity( 20, .0 );
    m_transferFunction->addPointToOpacity( 255, .2 );
    // Colors
    m_transferFunction->addPointToColorRGB( 0.0, 0.0, 0.0, 0.0 );
    m_transferFunction->addPointToColorRGB( 64.0, 1.0, 0.0, 0.0 );
    m_transferFunction->addPointToColorRGB( 128.0, 0.0, 0.0, 1.0 );
    m_transferFunction->addPointToColorRGB( 192.0, 0.0, 1.0, 0.0 );
    m_transferFunction->addPointToColorRGB( 255.0, 0.0, 0.2, 0.0 );

    m_volumeProperty->SetColor( m_transferFunction->getColorTransferFunction() );
    m_volumeProperty->SetScalarOpacity( m_transferFunction->getOpacityTransferFunction() );
}

Q3DViewer::~Q3DViewer()
{
    m_renderer->Delete();
}

vtkRenderer *Q3DViewer::getRenderer()
{
    return m_renderer;
}

void Q3DViewer::setRenderFunction(RenderFunction function)
{
    m_renderFunction = function;
}

void Q3DViewer::setRenderFunctionToRayCasting()
{
    m_renderFunction = RayCasting;
}

void Q3DViewer::setRenderFunctionToContouring()
{
    m_renderFunction = Contouring;
}

void Q3DViewer::setRenderFunctionToMIP3D()
{
    m_renderFunction = MIP3D;
}

void Q3DViewer::setRenderFunctionToIsoSurface()
{
    m_renderFunction = IsoSurface;
}

void Q3DViewer::setRenderFunctionToTexture2D()
{
    m_renderFunction = Texture2D;
}

void Q3DViewer::setRenderFunctionToTexture3D()
{
    m_renderFunction = Texture3D;
}

QString Q3DViewer::getRenderFunctionAsString()
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

void Q3DViewer::setInput( Volume* volume )
{
    m_mainVolume = volume;
}

void Q3DViewer::render()
{
    if( m_mainVolume )
    {
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
        WARN_LOG("Q3DViewer:: Cridant a render() sense haver donat cap input");
    }
}

void Q3DViewer::reset()
{
    // \TODO implementar tot el que falti
    m_currentOrientation = Axial;
    this->resetOrientation();
}

void Q3DViewer::setTransferFunction( TransferFunction *transferFunction )
{
    m_transferFunction = transferFunction;
    m_volumeProperty->SetScalarOpacity( m_transferFunction->getOpacityTransferFunction() );
    m_volumeProperty->SetColor( m_transferFunction->getColorTransferFunction() );
}

void Q3DViewer::resetOrientation()
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
        DEBUG_LOG("Q3DViewer: m_currentOrientation no és cap de les tres esperades ( Axial,Sagital,Coronal). Agafem Axial per defecte");
        this->resetViewToAxial();
    break;
    }
}

bool Q3DViewer::rescale()
{
    bool ok = false;
    if( m_mainVolume )
    {
        // Fem un casting de les dades ja que el ray cast mapper nomes accepta unsigned char/short
        double * range = m_mainVolume->getVtkData()->GetScalarRange();
        // Fem un rescale primer, perquè les dades quedin en un rang 0-256
        // tal com està fet ara no és del tot bo, potser caldria passar-li el filtre itk::RescaleIntensityImageFilter
        vtkImageShiftScale* rescale = vtkImageShiftScale::New();
        rescale->SetInput( m_mainVolume->getVtkData() );
        rescale->SetShift(0);
        rescale->SetScale( 256.0 / range[1] );
        rescale->SetOutputScalarType( VTK_UNSIGNED_SHORT );
    //     Fem un casting de les dades ja que el ray cast mapper nomes accepta unsigned char/short
        m_imageCaster->SetInput(  rescale->GetOutput() );
        m_imageCaster->SetOutputScalarType( VTK_UNSIGNED_SHORT ); // tbé seria vàlid VTK_UNSIGNED_SHORT
        m_imageCaster->ClampOverflowOn();
        m_imageCaster->Update();
        ok = true;
    }
    return ok;
}

void Q3DViewer::renderRayCasting()
{
    if( rescale() )
    {
        m_volumeProperty->DisableGradientOpacityOn();
        m_volumeProperty->ShadeOff();

        // el mapper (funcio de ray cast) sabrà com visualitzar les dades
        vtkVolumeRayCastCompositeFunction* compositeFunction = vtkVolumeRayCastCompositeFunction::New();
        compositeFunction->SetCompositeMethodToClassifyFirst();
        vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();

        volumeMapper->SetVolumeRayCastFunction( compositeFunction );
        volumeMapper->SetInput( m_imageCaster->GetOutput()  ); // abans inputImage->getVtkData()

        m_vtkVolume->SetMapper( volumeMapper );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per ray casting, no s'ha proporcionat cap volum d'entrada" );
}

void Q3DViewer::renderMIP3D()
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

void Q3DViewer::renderContouring()
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

void Q3DViewer::renderIsoSurface()
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

void Q3DViewer::renderTexture2D()
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

void Q3DViewer::renderTexture3D()
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

void Q3DViewer::resetViewToAxial()
{
    this->setCameraOrientation( Axial );
    m_currentOrientation = Axial;
}

void Q3DViewer::resetViewToSagital()
{
    this->setCameraOrientation( Sagital );
    m_currentOrientation = Sagital;
}

void Q3DViewer::resetViewToCoronal()
{
    this->setCameraOrientation( Coronal );
    m_currentOrientation = Coronal;
}

void Q3DViewer::enableOrientationMarker( bool enable )
{
    m_orientationMarker->setEnabled( enable );
}

void Q3DViewer::orientationMarkerOn()
{
    this->enableOrientationMarker( true );
}

void Q3DViewer::orientationMarkerOff()
{
    this->enableOrientationMarker( false );
}

};  // end namespace udg {
