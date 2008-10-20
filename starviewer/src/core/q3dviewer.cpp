/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dviewer.h"
#include "volume.h"
#include "image.h"
#include "imageplane.h"
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

#include <vtkMatrix4x4.h>

// obscurances
#include "obscurancemainthread.h"
#include <vtkPointData.h>
#include "vtkVolumeRayCastVoxelShaderCompositeFunction.h"
#include "ambientvoxelshader.h"
#include "directilluminationvoxelshader.h"
#include "obscurancevoxelshader.h"
#include "vtk4DLinearRegressionGradientEstimator.h"
#include <vtkEncodedGradientShader.h>

// settings
#include <QSettings>

namespace udg {

Q3DViewer::Q3DViewer( QWidget *parent )
 : QViewer( parent ), m_vtkVolume(0), m_volumeProperty(0), m_transferFunction(0)
{
    // Creem el Renderer de VTK i li assignem al widget que ens associa Qt amb VTK
    m_renderer = vtkRenderer::New();
    m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
    m_vtkWidget->setAutomaticImageCacheEnabled( true );
    this->getInteractorStyle()->SetCurrentRenderer( m_renderer );
    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );

    m_renderFunction = RayCasting; // per defecte

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
    m_volumeMapper = vtkVolumeRayCastMapper::New();
    m_vtkVolume->SetMapper( m_volumeMapper );
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

    m_volumeRayCastFunction = vtkVolumeRayCastCompositeFunction::New();
    m_volumeRayCastFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastVoxelShaderFunction = vtkVolumeRayCastVoxelShaderCompositeFunction::New();
    m_volumeRayCastFunction->SetCompositeMethodToClassifyFirst();

    m_firstRender = true;
    m_obscuranceMainThread = 0;
    m_obscurance = 0;

    m_ambientVoxelShader = new AmbientVoxelShader();
    m_directIlluminationVoxelShader = new DirectIlluminationVoxelShader();
    m_obscuranceVoxelShader = new ObscuranceVoxelShader();

    m_4DLinearRegressionGradientEstimator = 0;
}

Q3DViewer::~Q3DViewer()
{
    m_renderer->Delete();
    if ( m_obscuranceMainThread && m_obscuranceMainThread->isRunning() )
    {
        m_obscuranceMainThread->stop();
        m_obscuranceMainThread->wait();
        emit obscuranceCancelledByProgram();
    }
    delete m_obscuranceMainThread;
    delete[] m_obscurance;
    delete m_ambientVoxelShader;
    delete m_directIlluminationVoxelShader;
    delete m_obscuranceVoxelShader;
    if ( m_4DLinearRegressionGradientEstimator ) m_4DLinearRegressionGradientEstimator->Delete();
}

vtkRenderer *Q3DViewer::getRenderer()
{
    return m_renderer;
}

void Q3DViewer::getCurrentWindowLevel( double wl[2] )
{
    // TODO estem obligats a implementar-lo. De moment retornem 0,0
    wl[0] = wl[1] = 0.0;
}

void Q3DViewer::resetView( CameraOrientationType view )
{
    m_currentOrientation = view;
    //TODO replantejar si necessitem aquest mètode i el substituïm per aquest mateix
    resetOrientation();
}

void Q3DViewer::setRenderFunction(RenderFunction function)
{
    m_renderFunction = function;
}

void Q3DViewer::setRenderFunctionToRayCasting()
{
    m_renderFunction = RayCasting;
}

void Q3DViewer::setRenderFunctionToRayCastingShading()
{
    m_renderFunction = RayCastingShading;
}

void Q3DViewer::setRenderFunctionToRayCastingObscurance()
{
    m_renderFunction = RayCastingObscurance;
}

void Q3DViewer::setRenderFunctionToRayCastingShadingObscurance()
{
    m_renderFunction = RayCastingShadingObscurance;
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
    case RayCastingShading:
        result = "RayCastingShading";
    break;
    case RayCastingObscurance:
        result = "RayCastingObscurance";
    break;
    case RayCastingShadingObscurance:
        result = "RayCastingShadingObscurance";
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
    DEBUG_LOG( "setInput" );

    m_mainVolume = volume;

    // aquí corretgim el fet que no s'hagi adquirit la imatge en un espai ortogonal
    //\TODO: caldria fer el mateix amb el vtkImageActor del q2Dviewer (veure tiquet 702)
    ImagePlane * currentPlane = new ImagePlane();
    currentPlane->fillFromImage( m_mainVolume->getImage(0,0) );
    double currentPlaneRowVector[3], currentPlaneColumnVector[3];
    currentPlane->getRowDirectionVector( currentPlaneRowVector );
    currentPlane->getColumnDirectionVector( currentPlaneColumnVector );

    DEBUG_LOG( QString("currentPlaneRowVector: %1 %2 %3").arg(currentPlaneRowVector[0]).arg(currentPlaneRowVector[1]).arg(currentPlaneRowVector[2]));
    DEBUG_LOG( QString("currentPlaneColumnVector: %1 %2 %3").arg(currentPlaneColumnVector[0]).arg(currentPlaneColumnVector[1]).arg(currentPlaneColumnVector[2]));

    vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
    projectionMatrix->Identity();
    int row;
    for( row = 0; row < 3; row++ )
    {
        projectionMatrix->SetElement(row,0, (currentPlaneRowVector[ row ]));
        projectionMatrix->SetElement(row,1, (currentPlaneColumnVector[ row ]));
    }

    m_vtkVolume->SetUserMatrix(projectionMatrix);
    delete currentPlane;


    if ( rescale() ) m_volumeMapper->SetInput( m_imageCaster->GetOutput() );

    vtkImageData *image = m_volumeMapper->GetInput();
    unsigned char *data = reinterpret_cast<unsigned char*>( image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
    m_ambientVoxelShader->setData( data );
    m_directIlluminationVoxelShader->setData( data );
    m_obscuranceVoxelShader->setData( data );

    if ( m_obscuranceMainThread && m_obscuranceMainThread->isRunning() )
    {
        m_obscuranceMainThread->stop();
        m_obscuranceMainThread->wait();
        emit obscuranceCancelledByProgram();
    }
    delete m_obscuranceMainThread; m_obscuranceMainThread = 0;
    delete[] m_obscurance; m_obscurance = 0;

    render();
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
        case RayCastingShading:
            renderRayCastingShading();
        break;
        case RayCastingObscurance:
            renderRayCastingObscurance();
        break;
        case RayCastingShadingObscurance:
            renderRayCastingShadingObscurance();
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

        if ( m_firstRender )
        {
            this->resetOrientation();
            m_firstRender = false;
        }
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
    m_ambientVoxelShader->setTransferFunction( *m_transferFunction );
    m_directIlluminationVoxelShader->setTransferFunction( *m_transferFunction );

    if ( m_renderFunction == RayCastingShadingObscurance )
    {
        vtkEncodedGradientEstimator *gradientEstimator = m_volumeMapper->GetGradientEstimator();
        m_directIlluminationVoxelShader->setEncodedNormals( gradientEstimator->GetEncodedNormals() );
        vtkEncodedGradientShader *gradientShader = m_volumeMapper->GetGradientShader();
        gradientShader->UpdateShadingTable( m_renderer, m_vtkVolume, gradientEstimator );
        m_directIlluminationVoxelShader->setDiffuseShadingTables( gradientShader->GetRedDiffuseShadingTable( m_vtkVolume ),
                                                                  gradientShader->GetGreenDiffuseShadingTable( m_vtkVolume ),
                                                                  gradientShader->GetBlueDiffuseShadingTable( m_vtkVolume ) );
        m_directIlluminationVoxelShader->setSpecularShadingTables( gradientShader->GetRedSpecularShadingTable( m_vtkVolume ),
                                                                   gradientShader->GetGreenSpecularShadingTable( m_vtkVolume ),
                                                                   gradientShader->GetBlueSpecularShadingTable( m_vtkVolume ) );
    }
}

void Q3DViewer::setWindowLevel( double, double )
{
    // TODO estem obligats a implementar-lo.
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

// reescalem entre 0 i 255 si el rang és més gran (el que passarà normalment); sinó el deixem igual
bool Q3DViewer::rescale()
{
    if ( m_mainVolume )
    {
        vtkImageData *image = m_mainVolume->getVtkData();
        double *range = image->GetScalarRange();
        double min = range[0], max = range[1];
        DEBUG_LOG( QString( "Q3DViewer: m_mainVolume scalar range: min = %1, max = %2" ).arg( min ).arg( max ) );

        if ( min >= 0.0 && max <= 255.0 )   // si ja està dins del rang que volem només cal fer un cast
        {
            // cal fer el casting perquè ens arriba com a int
            m_imageCaster->SetInput( image );

            emit scalarRange( min, max );
        }
        else
        {
            double shift = -min;
            double slope = 255.0 / ( max - min );

            vtkImageShiftScale *rescaler = vtkImageShiftScale::New();
            rescaler->SetInput( image );
            rescaler->SetShift( shift );
            rescaler->SetScale( slope );
            rescaler->SetOutputScalarTypeToUnsignedChar();
            rescaler->ClampOverflowOn();
            rescaler->Update();
            m_imageCaster->SetInput( rescaler->GetOutput() );

            double *newRange = rescaler->GetOutput()->GetScalarRange();
            DEBUG_LOG( QString( "Q3DViewer: new scalar range: new min = %1, new max = %2" ).arg( newRange[0] ).arg( newRange[1] ) );

            rescaler->Delete();

            emit scalarRange( 0, 255 );
        }

        m_imageCaster->SetOutputScalarTypeToUnsignedChar();
        m_imageCaster->Update();

        return true;
    }
    else return false;
}

void Q3DViewer::renderRayCasting()
{
    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->ShadeOff();
    m_volumeProperty->SetInterpolationTypeToLinear();

    m_vtkVolume->SetMapper( m_volumeMapper );
    m_volumeMapper->SetVolumeRayCastFunction( m_volumeRayCastFunction );

    // no funciona sense fer la còpia
    TransferFunction *transferFunction = m_transferFunction;
    setTransferFunction( new TransferFunction( *transferFunction ) );
    delete transferFunction;

    m_vtkWidget->GetRenderWindow()->Render();
}

void Q3DViewer::renderRayCastingShading()
{
    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->ShadeOn();
    m_volumeProperty->SetInterpolationTypeToLinear();

    m_vtkVolume->SetMapper( m_volumeMapper );
    m_volumeMapper->SetVolumeRayCastFunction( m_volumeRayCastFunction );

    // no funciona sense fer la còpia
    TransferFunction *transferFunction = m_transferFunction;
    setTransferFunction( new TransferFunction( *transferFunction ) );
    delete transferFunction;

    m_vtkWidget->GetRenderWindow()->Render();
}

void Q3DViewer::renderRayCastingObscurance()
{
    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->ShadeOff();

    QSettings settings;
    settings.beginGroup( "3DViewer" );
    QString interpolation = settings.value( "interpolation" ).toString();
    settings.endGroup();
    if ( interpolation == "linear" ) m_volumeProperty->SetInterpolationTypeToLinear();
    else m_volumeProperty->SetInterpolationTypeToNearest();

    m_vtkVolume->SetMapper( m_volumeMapper );
    m_volumeMapper->SetVolumeRayCastFunction( m_volumeRayCastVoxelShaderFunction );
    m_volumeRayCastVoxelShaderFunction->RemoveVoxelShader( 0 );
    if ( m_volumeRayCastVoxelShaderFunction->IndexOfVoxelShader( m_ambientVoxelShader ) < 0 )
        m_volumeRayCastVoxelShaderFunction->InsertVoxelShader( 0, m_ambientVoxelShader );

    // no funciona sense fer la còpia
    TransferFunction *transferFunction = m_transferFunction;
    setTransferFunction( new TransferFunction( *transferFunction ) );
    delete transferFunction;

    m_vtkWidget->GetRenderWindow()->Render();
}

void Q3DViewer::renderRayCastingShadingObscurance()
{
    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->ShadeOn();

    QSettings settings;
    settings.beginGroup( "3DViewer" );
    QString interpolation = settings.value( "interpolation" ).toString();
    settings.endGroup();
    if ( interpolation == "linear" ) m_volumeProperty->SetInterpolationTypeToLinear();
    else m_volumeProperty->SetInterpolationTypeToNearest();

    m_vtkVolume->SetMapper( m_volumeMapper );
    m_volumeMapper->SetVolumeRayCastFunction( m_volumeRayCastVoxelShaderFunction );
    m_volumeRayCastVoxelShaderFunction->RemoveVoxelShader( 0 );
    if ( m_volumeRayCastVoxelShaderFunction->IndexOfVoxelShader( m_directIlluminationVoxelShader ) < 0 )
        m_volumeRayCastVoxelShaderFunction->InsertVoxelShader( 0, m_directIlluminationVoxelShader );

    vtkEncodedGradientEstimator *gradientEstimator = m_volumeMapper->GetGradientEstimator();
    m_directIlluminationVoxelShader->setEncodedNormals( gradientEstimator->GetEncodedNormals() );
    vtkEncodedGradientShader *gradientShader = m_volumeMapper->GetGradientShader();
    gradientShader->UpdateShadingTable( m_renderer, m_vtkVolume, gradientEstimator );
    m_directIlluminationVoxelShader->setDiffuseShadingTables( gradientShader->GetRedDiffuseShadingTable( m_vtkVolume ),
                                                              gradientShader->GetGreenDiffuseShadingTable( m_vtkVolume ),
                                                              gradientShader->GetBlueDiffuseShadingTable( m_vtkVolume ) );
    m_directIlluminationVoxelShader->setSpecularShadingTables( gradientShader->GetRedSpecularShadingTable( m_vtkVolume ),
                                                               gradientShader->GetGreenSpecularShadingTable( m_vtkVolume ),
                                                               gradientShader->GetBlueSpecularShadingTable( m_vtkVolume ) );

    // no funciona sense fer la còpia
    TransferFunction *transferFunction = m_transferFunction;
    setTransferFunction( new TransferFunction( *transferFunction ) );
    delete transferFunction;

    m_vtkWidget->GetRenderWindow()->Render();
}

void Q3DViewer::renderMIP3D()
{
    // quan fem MIP3D deixarem disable per defecte ja que la orientació no la sabem ben bé quina és ja que el pla de tall pot ser arbitrari \TODO no sempre un mip serà sobre un pla mpr, llavors tampoc és del tot correcte decidir això aquí
//         m_orientationMarker->disable();
    //================================================================================================
    // Create a transfer function mapping scalar value to opacity
    // assignem una rampa d'opacitat total per valors alts i nula per valors petits
    // després en l'escala de grisos donem un  valor de gris constant ( blanc )

    //\TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction

    // Creem la funció de transferència de l'opacitat
//     m_transferFunction->addPointToOpacity( 20, .0 );
//     m_transferFunction->addPointToOpacity( 255, 1. );
    TransferFunction mipTransferFunction;
    mipTransferFunction.addPointToOpacity( 20.0, 0.0 );
    mipTransferFunction.addPointToOpacity( 255.0, 1.0 );

    // Creem la funció de transferència de colors
    // Create a transfer function mapping scalar value to color (grey)
    vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
    grayTransferFunction->AddSegment( 0 , 0.0 , 255 , 1.0 );

//     m_volumeProperty->SetScalarOpacity( m_transferFunction->getOpacityTransferFunction() );
    m_volumeProperty->SetScalarOpacity( mipTransferFunction.getOpacityTransferFunction() );
    m_volumeProperty->SetColor( grayTransferFunction /*m_transferFunction->getColorTransferFunction()*/ );
    m_volumeProperty->ShadeOff();
    m_volumeProperty->SetInterpolationTypeToLinear();

    grayTransferFunction->Delete();

    // creem la funció del raig MIP, en aquest cas maximitzem l'opacitat, si fos Scalar value, ho faria pel valor
    vtkVolumeRayCastMIPFunction* mipFunction = vtkVolumeRayCastMIPFunction::New();
    mipFunction->SetMaximizeMethodToOpacity();

//         vtkFiniteDifferenceGradientEstimator *gradientEstimator = vtkFiniteDifferenceGradientEstimator::New();
//     vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();

    m_vtkVolume->SetMapper( m_volumeMapper );
    m_volumeMapper->SetVolumeRayCastFunction( mipFunction );
//     volumeMapper->SetInput( m_imageCaster->GetOutput()  );
//         volumeMapper->SetGradientEstimator( gradientEstimator );

//     m_vtkVolume->SetMapper( volumeMapper );

    mipFunction->Delete();

    m_vtkWidget->GetRenderWindow()->Render();
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

        m_renderer->AddViewProp( m_3DActor );
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
    /// \todo Això és massa lent, potser l'hauríem de treure.
    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->ShadeOff();   // off perquè vagi més ràpid
    m_volumeProperty->SetInterpolationTypeToNearest();  // nearest perquè vagi més ràpid

    vtkVolumeTextureMapper2D *volumeMapper = vtkVolumeTextureMapper2D::New();

    // target texture size: en teoria com més gran millor
    // màxim en una Quadro FX 4500 = 4096x4096
//     volumeMapper->SetTargetTextureSize( 4096, 4096 );

    // max number of planes: This is the maximum number of planes that will be created for texture mapping the volume. If the volume has more
    // voxels than this along the viewing direction, then planes of the volume will be skipped to ensure that this maximum is not violated. A
    // skip factor is used, and is incremented until the maximum condition is satisfied.
    // 128 és el que té millor relació qualitat/preu amb un model determinat a l'ordinador de la uni
//     volumeMapper->SetMaximumNumberOfPlanes( 128 );

    volumeMapper->SetInput( m_imageCaster->GetOutput()  );
    m_vtkVolume->SetMapper( volumeMapper );
    volumeMapper->Delete();

    // no funciona sense fer la còpia
    TransferFunction *transferFunction = m_transferFunction;
    setTransferFunction( new TransferFunction( *transferFunction ) );
    delete transferFunction;

    m_vtkWidget->GetRenderWindow()->Render();
}

void Q3DViewer::renderTexture3D()
{
    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->ShadeOff();   /// \todo amb on també funciona; hauríem de donar l'opció, o deixar-ho sempre a off o sempre a on?
    m_volumeProperty->SetInterpolationTypeToLinear();

    vtkVolumeTextureMapper3D *volumeMapper = vtkVolumeTextureMapper3D::New();
    volumeMapper->SetInput( m_imageCaster->GetOutput()  );
    m_vtkVolume->SetMapper( volumeMapper );
    volumeMapper->Delete();

    // no funciona sense fer la còpia
    TransferFunction *transferFunction = m_transferFunction;
    setTransferFunction( new TransferFunction( *transferFunction ) );
    delete transferFunction;

    m_vtkWidget->GetRenderWindow()->Render();
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

void Q3DViewer::setSpecular( bool on )
{
    m_volumeProperty->SetSpecular( on ? 1.0 : 0.0 );
}

void Q3DViewer::setSpecularPower( double power )
{
    m_volumeProperty->SetSpecularPower( power );
}

void Q3DViewer::computeObscurance( ObscuranceQuality quality )
{
    Q_ASSERT( !m_obscuranceMainThread || m_obscuranceMainThread->isFinished() );

    delete m_obscuranceMainThread; m_obscuranceMainThread = 0;

    if ( !m_4DLinearRegressionGradientEstimator )
    {
        m_4DLinearRegressionGradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
        m_volumeMapper->SetGradientEstimator( m_4DLinearRegressionGradientEstimator );  // radi 1 per defecte (-> 3³)
        m_4DLinearRegressionGradientEstimator->SetInput( m_volumeMapper->GetInput() );  /// \todo hauria de funcionar sense això, però no !?!?!
    }

    QSettings settings;
    settings.beginGroup( "3DViewer" );
    settings.beginGroup( "obscurances" );

    switch ( quality )
    {
        case Low:
            settings.beginGroup( "low" );
            break;
        case Medium:
            settings.beginGroup( "medium" );
            break;
        case High:
            settings.beginGroup( "high" );
            break;
        default:
            ERROR_LOG( QString( "Valor inesperat per a la qualitat de les obscurances: %1" ).arg( quality ) );
    }

    int numberOfDirections = settings.value( "numberOfDirections" ).toInt();
    ObscuranceMainThread::Function function = static_cast<ObscuranceMainThread::Function>( settings.value( "function" ).toInt() );
    ObscuranceMainThread::Variant variant = static_cast<ObscuranceMainThread::Variant>( settings.value( "variant" ).toInt() );
    unsigned int gradientRadius = settings.value( "gradientRadius" ).toUInt();

    double distance = m_vtkVolume->GetLength() / 2.0;   /// \todo de moment la meitat de la diagonal, però podria ser una altra funció
    // el primer paràmetre és el nombre de direccions
    // pot ser >= 0 i llavors es fan 10*4^n+2 direccions (12, 42, 162, 642, ...)
    // també pot ser < 0 i llavors es fan -n direccions (valors permesos: -4, -6, -8, -12, -20; amb qualsevol altre s'aplica -4)
    m_obscuranceMainThread = new ObscuranceMainThread( numberOfDirections, distance, function, variant, this );

    /// \todo Només canviant això ja recalcularà les normals o cal fer alguna cosa més?
    if ( m_4DLinearRegressionGradientEstimator->GetRadius() < gradientRadius )
        m_4DLinearRegressionGradientEstimator->SetRadius( gradientRadius );

    settings.endGroup();    // qualitat
    settings.endGroup();    // obscurances
    settings.endGroup();    // 3DViewer

    // Preparem el vector que farem servir, reaprofitant memòria si podem
    vtkImageData *image = m_volumeMapper->GetInput();
    int dataSize = image->GetPointData()->GetScalars()->GetSize();

    if ( !m_obscurance ) m_obscurance = new double[dataSize];
    for ( int i = 0; i < dataSize; i++ ) m_obscurance[i] = 0.0;

    m_obscuranceMainThread->setVolume( m_vtkVolume );
    m_obscuranceMainThread->setTransferFunction( *m_transferFunction );
    m_obscuranceMainThread->setObscurance( m_obscurance, 0 );

    render();   // això cal perquè sinó el DirectIllumationVoxelShader produeix artefactes estranys perquè hem canviat el gradient estimator

    connect( m_obscuranceMainThread, SIGNAL( progress(int) ), this, SIGNAL( obscuranceProgress(int) ) );
    connect( m_obscuranceMainThread, SIGNAL( computed() ), this, SLOT( endComputeObscurance() ) );
    m_obscuranceMainThread->start();
}

void Q3DViewer::cancelObscurance()
{
    Q_ASSERT( m_obscuranceMainThread && m_obscuranceMainThread->isRunning() );

    m_obscuranceMainThread->stop();
}

void Q3DViewer::endComputeObscurance()
{
    Q_ASSERT( m_obscuranceMainThread );

    m_obscuranceVoxelShader->setObscurance( m_obscurance );

    emit obscuranceComputed();
}

void Q3DViewer::setObscurance( bool on )
{
    if ( on && m_volumeRayCastVoxelShaderFunction->IndexOfVoxelShader( m_obscuranceVoxelShader ) < 0 )
    {
        m_volumeRayCastVoxelShaderFunction->AddVoxelShader( m_obscuranceVoxelShader );
        DEBUG_LOG( "obscurança afegida" );
    }
    else
        m_volumeRayCastVoxelShaderFunction->RemoveVoxelShader( m_obscuranceVoxelShader );

    m_volumeRayCastVoxelShaderFunction->Print( std::cout );
}

void Q3DViewer::setObscuranceFactor( double factor )
{
    m_obscuranceVoxelShader->setFactor( factor );
}

};  // end namespace udg {
