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
#include "q3dviewertoolmanager.h"

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
 : QViewer( parent ), m_vtkVolume(0)
{
    // Creem el Renderer de VTK i li assignem al widget que ens associa Qt amb VTK
    m_renderer = vtkRenderer::New();
    m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
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
    // tool manager init
    m_toolManager = new Q3DViewerToolManager( this );
    this->enableTools();
}

Q3DViewer::~Q3DViewer()
{
    m_renderer->Delete();
}

void Q3DViewer::setEnableTools( bool enable )
{
    if( enable )
        this->enableTools();
    else
        this->disableTools();
}

void Q3DViewer::enableTools()
{
    connect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
}

void Q3DViewer::disableTools()
{
    disconnect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
}

vtkRenderer *Q3DViewer::getRenderer()
{
    return m_renderer;
}

vtkInteractorStyle *Q3DViewer::getInteractorStyle()
{
    return vtkInteractorStyle::SafeDownCast( this->getInteractor()->GetInteractorStyle() );
}

void Q3DViewer::setRenderFunction(RenderFunction function)
{
    m_renderFunction = function;
}

void Q3DViewer::setRenderFunctionToRayCasting()
{
    m_renderFunction = RayCasting;
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
        //\TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction
        // Creem la funció de transferència de l'opacitat
        vtkPiecewiseFunction* opacityTransferFunction = vtkPiecewiseFunction::New();
        opacityTransferFunction->AddPoint( 20, 0.0 );
        opacityTransferFunction->AddPoint( 255, 0.2 );

        // Creem la funció de transferència de colors
        vtkColorTransferFunction* colorTransferFunction = vtkColorTransferFunction::New();
        colorTransferFunction->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 64.0, 1.0, 0.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 128.0, 0.0, 0.0, 1.0 );
        colorTransferFunction->AddRGBPoint( 192.0, 0.0, 1.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 255.0, 0.0, 0.2, 0.0 );

        // La propietat descriurà com es veuran les dades
        vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
        //\TODO aquí tindrem m_currentTransferFunction->getRGB/ColorTransferFunction() i m_currentTransferFunction->getOpacityTransferFunction() respectivament
        volumeProperty->SetColor( colorTransferFunction );
        volumeProperty->SetScalarOpacity( opacityTransferFunction );

        // el mapper (funcio de ray cast) sabrà com visualitzar les dades
        vtkVolumeRayCastCompositeFunction* compositeFunction = vtkVolumeRayCastCompositeFunction::New();
        vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();

        volumeMapper->SetVolumeRayCastFunction( compositeFunction );
        volumeMapper->SetInput( m_imageCaster->GetOutput()  ); // abans inputImage->getVtkData()

        // el volum conté el mapper i la propietat i es pot usar per posicionar/orientar el volum
        if( !m_vtkVolume )
        {
            m_vtkVolume = vtkVolume::New();
            m_renderer->AddViewProp( m_vtkVolume );
        }

        m_vtkVolume->SetMapper( volumeMapper );
        m_vtkVolume->SetProperty( volumeProperty );
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
        vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
        opacityTransferFunction->AddPoint( 20 , 0.0 );
        opacityTransferFunction->AddPoint( 255 , 1.0 );

        // Create a transfer function mapping scalar value to color (grey)
        vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
        grayTransferFunction->AddSegment( 0 , 0.0 , 255 , 1.0 );

        // Create a set of properties for mip
        vtkVolumeProperty *mipProperty;

        //\TODO aquí tindrem m_currentTransferFunction->getGrayTransferFunction() i m_currentTransferFunction->getOpacityTransferFunction() que són 1D totes dues
        mipProperty = vtkVolumeProperty::New();
        mipProperty->SetScalarOpacity( opacityTransferFunction );
        mipProperty->SetColor( grayTransferFunction );
        mipProperty->SetInterpolationTypeToLinear();

        // creem la funció del raig MIP, en aquest cas maximitzem l'opacitat, si fos Scalar value, ho faria pel valor
        vtkVolumeRayCastMIPFunction* mipFunction = vtkVolumeRayCastMIPFunction::New();
        mipFunction->SetMaximizeMethodToOpacity();

    //     vtkFiniteDifferenceGradientEstimator *gradientEstimator = vtkFiniteDifferenceGradientEstimator::New();
        vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();

        volumeMapper->SetVolumeRayCastFunction( mipFunction );
        volumeMapper->SetInput( m_imageCaster->GetOutput()  );
    //     volumeMapper->SetGradientEstimator( gradientEstimator );
        // el volum conté el mapper i la propietat i es pot usar per posicionar/orientar el volum
        if( !m_vtkVolume )
        {
            m_vtkVolume = vtkVolume::New();
            m_renderer->AddViewProp( m_vtkVolume );
        }
        m_vtkVolume->SetMapper( volumeMapper );
        m_vtkVolume->SetProperty( mipProperty );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per MIP, no s'ha proporcionat cap volum d'entrada" );
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

        // Create a set of properties with varying options
        vtkVolumeProperty* prop = vtkVolumeProperty::New();

        prop->SetShade(1);

        /*prop->SetAmbient(0.3);
        prop->SetDiffuse(1.0);
        prop->SetSpecular(0.2);
        prop->SetSpecularPower(50.0);
        */
        //\TODO aquí tindrem m_currentTransferFunction->getRGB/ColorTransferFunction() i m_currentTransferFunction->getOpacityTransferFunction(), potser faltaria la del gradient, en aquest cas és una mica més especial
        prop->SetScalarOpacity(oTFun);
        prop->SetGradientOpacity( goTFun );
        prop->SetColor( cTFun );
    //     prop->SetColor( grayTransferFunction );
        prop->SetInterpolationTypeToLinear(); //prop[index]->SetInterpolationTypeToNearest();

        // Create an isosurface ray function
        vtkVolumeRayCastIsosurfaceFunction *isosurfaceFunction = vtkVolumeRayCastIsosurfaceFunction::New();
        isosurfaceFunction->SetIsoValue(80);
        vtkFiniteDifferenceGradientEstimator *gradientEstimator = vtkFiniteDifferenceGradientEstimator::New();

        vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
        volumeMapper->SetVolumeRayCastFunction( isosurfaceFunction );
        volumeMapper->SetInput( m_imageCaster->GetOutput()  ); // abans inputImage->getVtkData()
        volumeMapper->SetGradientEstimator( gradientEstimator );
        // el volum conté el mapper i la propietat i es pot usar per posicionar/orientar el volum
        if( !m_vtkVolume )
        {
            m_vtkVolume = vtkVolume::New();
            m_renderer->AddViewProp( m_vtkVolume );
        }

        m_vtkVolume->SetMapper( volumeMapper );
        m_vtkVolume->SetProperty( prop );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per IsoSurface, no s'ha proporcionat cap volum d'entrada" );
}

void Q3DViewer::renderTexture2D()
{
    if( rescale() )
    {
        //\TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction
        // Creem la funció de transferència de l'opacitat
        vtkPiecewiseFunction* opacityTransferFunction = vtkPiecewiseFunction::New();
        opacityTransferFunction->AddPoint( 20, 0.0 );
        opacityTransferFunction->AddPoint( 255, 0.2 );

        // Creem la funció de transferència de colors
        vtkColorTransferFunction* colorTransferFunction = vtkColorTransferFunction::New();
        colorTransferFunction->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 64.0, 1.0, 0.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 128.0, 0.0, 0.0, 1.0 );
        colorTransferFunction->AddRGBPoint( 192.0, 0.0, 1.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 255.0, 0.0, 0.2, 0.0 );

        // La propietat descriurà com es veuran les dades
        //\TODO aquí tindrem m_currentTransferFunction->getRGB/ColorTransferFunction() i m_currentTransferFunction->getOpacityTransferFunction() respectivament
        vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
        volumeProperty->SetColor( colorTransferFunction );
        volumeProperty->SetScalarOpacity( opacityTransferFunction );

        vtkVolumeTextureMapper2D* volumeMapper = vtkVolumeTextureMapper2D::New();
        volumeMapper->SetInput( m_imageCaster->GetOutput()  );

        // el volum conté el mapper i la propietat i es pot usar per posicionar/orientar el volum
        if( !m_vtkVolume )
        {
            m_vtkVolume = vtkVolume::New();
            m_renderer->AddViewProp( m_vtkVolume );
        }

        m_vtkVolume->SetMapper( volumeMapper );
        m_vtkVolume->SetProperty( volumeProperty );
        m_renderer->Render();
    }
    else
        DEBUG_LOG( "No es pot fer render per textures 2D, no s'ha proporcionat cap volum d'entrada" );
}

void Q3DViewer::renderTexture3D()
{
    if( rescale() )
    {
        //\TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction
        // Creem la funció de transferència de l'opacitat
        vtkPiecewiseFunction* opacityTransferFunction = vtkPiecewiseFunction::New();
        opacityTransferFunction->AddPoint( 20, 0.0 );
        opacityTransferFunction->AddPoint( 255, 0.2 );

        // Creem la funció de transferència de colors
        vtkColorTransferFunction* colorTransferFunction = vtkColorTransferFunction::New();
        colorTransferFunction->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 64.0, 1.0, 0.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 128.0, 0.0, 0.0, 1.0 );
        colorTransferFunction->AddRGBPoint( 192.0, 0.0, 1.0, 0.0 );
        colorTransferFunction->AddRGBPoint( 255.0, 0.0, 0.2, 0.0 );

        // La propietat descriurà com es veuran les dades
        vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
        //\TODO aquí tindrem m_currentTransferFunction->getRGB/ColorTransferFunction() i m_currentTransferFunction->getOpacityTransferFunction() respectivament
        volumeProperty->SetColor( colorTransferFunction );
        volumeProperty->SetScalarOpacity( opacityTransferFunction );

        vtkVolumeTextureMapper3D* volumeMapper = vtkVolumeTextureMapper3D::New();
        volumeMapper->SetInput( m_imageCaster->GetOutput()  );

        // el volum conté el mapper i la propietat i es pot usar per posicionar/orientar el volum
        if( !m_vtkVolume )
        {
            m_vtkVolume = vtkVolume::New();
            m_renderer->AddViewProp( m_vtkVolume );
        }

        m_vtkVolume->SetMapper( volumeMapper );
        m_vtkVolume->SetProperty( volumeProperty );
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

void Q3DViewer::setTool( QString toolName )
{
    if( m_toolManager->setCurrentTool( toolName ) )
    {
        ///\Todo per implementar
        DEBUG_LOG( QString("OK, hem activat la tool: ") + toolName );
    }
    else
    {
        ///\Todo per implementar
        DEBUG_LOG( QString(":/ no s'ha pogut activar la tool: ") + toolName );
    }
}

void Q3DViewer::setCameraOrientation(int orientation)
{
    vtkCamera *cam = this->getRenderer() ? this->getRenderer()->GetActiveCamera() : NULL;
    if (cam)
    {
        switch (orientation)
        {
        case Axial:
            cam->SetFocalPoint(0,0,0);
            cam->SetPosition(0,0,-1); // -1 if medical ?
            cam->SetViewUp(0,-1,0);
            break;

        case Coronal:
            cam->SetFocalPoint(0,0,0);
            cam->SetPosition(0,-1,0); // 1 if medical ?
            cam->SetViewUp(0,0,1);
            break;

        case Sagital:
            cam->SetFocalPoint(0,0,0);
            cam->SetPosition(1,0,0); // -1 if medical ?
            cam->SetViewUp(0,0,1);
            break;
        }
        this->getRenderer()->ResetCamera();
        this->refresh();
    }
}

};  // end namespace udg {
