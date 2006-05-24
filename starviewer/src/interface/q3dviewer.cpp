/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q3dviewer.h"
#include "volume.h"
#include "logging.h"

// include's qt
#include <QString>

// include's vtk
#include <QVTKWidget.h>
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
// LUT's
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
// Casting
#include <vtkImageCast.h> 
#include <vtkImageShiftScale.h>

#include <vtkImageViewer.h>

namespace udg {

Q3DViewer::Q3DViewer( QWidget *parent )
 : QViewer( parent )
{
    // Creem el Renderer de VTK i li assignem al widget que ens associa Qt amb VTK
    m_renderer = vtkRenderer::New();
    m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );
    
    m_renderFunction = RayCasting; // per defecte
    
    m_imageCaster = vtkImageCast::New();
}


Q3DViewer::~Q3DViewer()
{
    m_renderer->Delete();
}

vtkRenderWindowInteractor *Q3DViewer::getInteractor()
{
    return m_vtkWidget->GetRenderWindow()->GetInteractor();
}

vtkRenderer *Q3DViewer::getRenderer()
{
    return m_renderer;       
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
    }
    return result;
}

void Q3DViewer::setInput( Volume* volume )
{
    m_mainVolume = volume;
    // \TODO fer que el sistema de càmeres funcioni
//     this->resetViewToAxial();
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
        }
        
    }
    else
    {
        WARN_LOG("Q3DViewer:: Cridant a render() sense haver donat cap input");
    }
    //else: mostrar error/avís?
}

void Q3DViewer::rescale()
{
    // Fem un casting de les dades ja que el ray cast mapper nomes accepta unsigned char/short
    double * range = m_mainVolume->getVtkData()->GetScalarRange();
    // Fem un rescale primer, perquè les dades quedin en un rang 0-256
    // tal com està fet ara no és del tot bo, potser caldria passar-li el filtre itk::RescaleIntensityImageFilter
    vtkImageShiftScale* rescale = vtkImageShiftScale::New();
    rescale->SetInput( m_mainVolume->getVtkData() );
    rescale->SetShift(0);
    rescale->SetScale( 256.0 / range[1] );
    rescale->SetOutputScalarType( VTK_UNSIGNED_CHAR );
//     Fem un casting de les dades ja que el ray cast mapper nomes accepta unsigned char/short
    m_imageCaster->SetInput(  rescale->GetOutput() );
    m_imageCaster->SetOutputScalarType( VTK_UNSIGNED_CHAR ); // tbé seria vàlid VTK_UNSIGNED_SHORT
    m_imageCaster->ClampOverflowOn();
    m_imageCaster->Update();
}

void Q3DViewer::renderRayCasting()
{
    
    rescale();
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
    volumeProperty->SetColor( colorTransferFunction );
    volumeProperty->SetScalarOpacity( opacityTransferFunction );
    
    // el mapper (funcio de ray cast) sabrà com visualitzar les dades
    vtkVolumeRayCastCompositeFunction* compositeFunction = vtkVolumeRayCastCompositeFunction::New();
    vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();
    
    volumeMapper->SetVolumeRayCastFunction( compositeFunction );
    volumeMapper->SetInput( m_imageCaster->GetOutput()  ); // abans inputImage->getVtkData()
    
    // el volum conté el mapper i la propietat i es pot usar per posicionar/orientar el volum
    vtkVolume* volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( volumeProperty );

    m_renderer->AddViewProp( volume );
    m_renderer->Render();
}

void Q3DViewer::renderMIP3D()
{
    rescale();
    //================================================================================================
    // Create a transfer function mapping scalar value to opacity
    // assignem una rampa d'opacitat total per valors alts i nula per valors petits
    // després en l'escala de grisos donem un  valor de gris constant ( blanc )
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint( 20 , 0.0 );
    opacityTransferFunction->AddPoint( 255 , 1.0 );
    
    // Create a transfer function mapping scalar value to color (grey)
    vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
    grayTransferFunction->AddSegment( 0 , 0.0 , 255 , 1.0 );

    // Create a set of properties for mip
    vtkVolumeProperty *mipProperty;
    
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
    vtkVolume* volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( mipProperty );

    m_renderer->AddViewProp( volume );
    m_renderer->Render();
}

void Q3DViewer::renderIsoSurface()
{
    rescale();
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
    vtkVolume* volume = vtkVolume::New();
    volume->SetMapper( volumeMapper );
    volume->SetProperty( prop );

    m_renderer->AddViewProp( volume );
    m_renderer->Render();  
}

void Q3DViewer::resetViewToAxial()
{
    this->setCameraOrientation( Axial );
    this->getInteractor()->Render();
}

void Q3DViewer::resetViewToSagital()
{
    this->setCameraOrientation( Sagital );
    this->getInteractor()->Render();
}

void Q3DViewer::resetViewToCoronal()
{
    this->setCameraOrientation( Coronal );
    this->getInteractor()->Render();
}

void Q3DViewer::setCameraOrientation(int orientation)
{
    vtkCamera *cam = this->getRenderer() ? this->getRenderer()->GetActiveCamera() : NULL;
    if (cam)
    {
        switch (orientation)
        {
        case Axial:
//             cam->SetFocalPoint(0,0,0);
//             cam->SetPosition(0,0,-1); // -1 if medical ?
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
    }
}

};  // end namespace udg {
