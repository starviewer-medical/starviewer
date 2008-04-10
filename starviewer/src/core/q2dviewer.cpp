/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewer.h"
#include "drawer.h"
#include "volume.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "imageplane.h"
//thickslab
#include "vtkProjectionImageFilter.h"

// include's qt
#include <QResizeEvent>
#include <QSize>
#include <QMenu>
#include <QAction>
#include <QMutableMapIterator>
// Tools
#include "q2dviewertoolmanager.h"

// include's bàsics vtk
#include <QVTKWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageViewer2.h>
#include <vtkCamera.h>
#include <vtkMath.h>
// composició d'imatges
#include <vtkImageCheckerboard.h>
#include <vtkImageBlend.h>
#include <vtkImageRectilinearWipe.h>
// anotacions
#include <vtkCornerAnnotation.h>
#include <vtkAxisActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkProp.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
// desar imatges
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkBMPWriter.h>
#include <vtkMetaImageWriter.h>
// voxel information
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkPropPicker.h>

#include <vtkPropCollection.h>
#include <vtkRendererCollection.h>
#include <vtkCollection.h>
#include <vtkActor2DCollection.h>
#include <vtkImageActor.h>

// displayed area
#include <vtkImageChangeInformation.h>
#include <vtkImageResample.h>
#include <vtkExtractVOI.h>

// grayscale pipeline
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageShiftScale.h>
#include <vtkWindowLevelLookupTable.h>

// projeccio de punts
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>

namespace udg {

Q2DViewer::Q2DViewer( QWidget *parent )
 : QViewer( parent ), m_currentSlice(0), m_currentPhase(0), m_overlayVolume(0), m_blender(0), m_picker(0), m_serieInformationAnnotation(0), m_sideRuler(0), m_bottomRuler(0), m_scalarBar(0), m_rotateFactor(0), m_columns(1), m_rows(1), m_numberOfSlicesWindows(1), m_numberOfPhases(1), m_maxSliceValue(0), m_applyFlip(false), m_isImageFlipped(false),m_modalityLUTRescale(0), m_modalityLut(0), m_windowLevelLut(0), m_presentationLut(0), m_slabThickness(1), m_firstSlabSlice(0), m_lastSlabSlice(0), m_thickSlabActive(false), m_slabProjectionMode( AccumulatorFactory::Maximum )
{
    // per composar el thickSlab
    m_thickSlabProjectionFilter = vtkProjectionImageFilter::New();

    m_enabledAnnotations = Q2DViewer::AllAnnotation;
    m_lastView = Q2DViewer::Axial;
    m_imageSizeInformation[0] = 0;
    m_imageSizeInformation[1] = 0;
    m_overlay = CheckerBoard; // per defecte
    // CheckerBoard
    // el nombre de divisions per defecte, serà de 2, per simplificar
    m_divisions[0] = m_divisions[1] = m_divisions[2] = 2;

    // inicialització viewport de les llesques
    m_slicesViewportExtent[0] = .0;
    m_slicesViewportExtent[1] = .0;
    m_slicesViewportExtent[2] = 1.;
    m_slicesViewportExtent[3] = 1.;

    // inicialitzacions d'objectes
    // visor
    m_viewer = vtkImageViewer2::New();
    // grayscale pipeline
    m_windowLevelLUTMapper = vtkImageMapToWindowLevelColors::New();

    setupInteraction();
    m_enabledTools = false;
    m_toolManager = new Q2DViewerToolManager( this );
    this->enableTools();

    // anotacions
    createAnnotations();
    addActors();

    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );

    m_sliceActorCollection = vtkPropCollection::New();
    m_rendererCollection = vtkRendererCollection::New();
    m_informationCollection = vtkActor2DCollection::New();
    m_scalarBarCollection = vtkActor2DCollection::New();

    m_sliceActorCollection->AddItem( m_viewer->GetImageActor() );
    m_rendererCollection->AddItem( m_viewer->GetRenderer() );
    m_informationCollection->AddItem ( m_serieInformationAnnotation );
    m_scalarBarCollection->AddItem( m_scalarBar );

    m_anchoredRulerCoordinatesCollection = vtkCollection::New();
    m_anchoredRulerCoordinatesCollection->AddItem( m_anchoredRulerCoordinates );
    m_rulerActorCollection = vtkActor2DCollection::New();
    m_rulerActorCollection->AddItem( m_bottomRuler );
    m_rulerActorCollection->AddItem( m_sideRuler );

    //creem el drawer, passant-li com a visor l'objecte this
    m_drawer = new Drawer( this );

    connect( this, SIGNAL(cameraChanged()), SLOT(updateRulers()) );
}

Q2DViewer::~Q2DViewer()
{
    m_scalarBar->Delete();
    m_patientOrientationTextActor[0]->Delete();
    m_patientOrientationTextActor[1]->Delete();
    m_patientOrientationTextActor[2]->Delete();
    m_patientOrientationTextActor[3]->Delete();
    m_sideRuler->Delete();
    m_bottomRuler->Delete();
    m_serieInformationAnnotation->Delete();
    m_picker->Delete();
    m_viewer->Delete();
    m_vtkQtConnections->Delete();
}

vtkRenderer *Q2DViewer::getRenderer()
{
    if( m_viewer )
        return m_viewer->GetRenderer();
    else
        return NULL;
}

void Q2DViewer::createAnnotations()
{
    // contenidor d'anotacions de texte FIXE
    m_serieInformationAnnotation = vtkCornerAnnotation::New();

    // contenidor d'anotacions de texte variable
    m_sliceAnnotationsCollection = vtkActor2DCollection::New();

    // escala de colors
    m_scalarBar = createScalarBar();
    // anotacions de l'orientació del pacient
    createOrientationAnnotations();
    // Marcadors d'escala
    createRulers();
}

void Q2DViewer::createOrientationAnnotations()
{
    // informació de referència de la orientació del pacient
    for( int i = 0; i < 4; i++ )
    {
        m_patientOrientationTextActor[i] = vtkTextActor::New();
        m_patientOrientationTextActor[i]->ScaledTextOff();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontSize( 18 );
        m_patientOrientationTextActor[i]->GetTextProperty()->BoldOn();

        m_patientOrientationTextActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        m_patientOrientationTextActor[i]->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
    }
    // ara posem la informació concreta de cadascuna de les referència d'orientació. 0-4 en sentit anti-horari, començant per 0 = esquerra de la pantalla
    m_patientOrientationTextActor[0]->GetTextProperty()->SetJustificationToLeft();
    m_patientOrientationTextActor[0]->SetPosition( 0.01 , 0.5 );

    m_patientOrientationTextActor[1]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[1]->SetPosition( 0.5 , 0.01 );

    m_patientOrientationTextActor[2]->GetTextProperty()->SetJustificationToRight();
    m_patientOrientationTextActor[2]->SetPosition( 0.99 , 0.5 );

    m_patientOrientationTextActor[3]->GetTextProperty()->SetJustificationToCentered();
    m_patientOrientationTextActor[3]->SetPosition( 0.5 , 0.95 );
}

void Q2DViewer::createRulers()
{
    // ruler lateral
    m_sideRuler = vtkAxisActor2D::New();
    m_sideRuler->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_sideRuler->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    m_sideRuler->AxisVisibilityOn();
    m_sideRuler->TickVisibilityOn();
    m_sideRuler->LabelVisibilityOn();
    m_sideRuler->AdjustLabelsOff();
    m_sideRuler->SetLabelFormat("%.2f");
    m_sideRuler->SetLabelFactor( 0.35 );
    m_sideRuler->GetLabelTextProperty()->ItalicOff();
    m_sideRuler->GetLabelTextProperty()->BoldOff();
    m_sideRuler->GetLabelTextProperty()->ShadowOff();
    m_sideRuler->GetLabelTextProperty()->SetColor( 0 , 0.7 , 0 );
    m_sideRuler->TitleVisibilityOff();
    m_sideRuler->SetTickLength( 10 );
    m_sideRuler->GetProperty()->SetColor( 0 , 1 , 0 );
    m_sideRuler->VisibilityOff(); // per defecte, fins que no hi hagi input son invisibles

    // ruler inferior
    m_bottomRuler = vtkAxisActor2D::New();
    m_bottomRuler->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_bottomRuler->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    m_bottomRuler->AxisVisibilityOn();
    m_bottomRuler->TickVisibilityOn();
    m_bottomRuler->LabelVisibilityOn();
    m_bottomRuler->AdjustLabelsOff();
    m_bottomRuler->SetLabelFormat("%.2f");
    m_bottomRuler->SetLabelFactor( 0.35 );
    m_bottomRuler->GetLabelTextProperty()->ItalicOff();
    m_bottomRuler->GetLabelTextProperty()->BoldOff();
    m_bottomRuler->GetLabelTextProperty()->ShadowOff();
    m_bottomRuler->GetLabelTextProperty()->SetColor( 0 , 0.7 , 0 );
    m_bottomRuler->TitleVisibilityOff();
    m_bottomRuler->SetTickLength( 10 );
    m_bottomRuler->GetProperty()->SetColor( 0 , 1 , 0 );
    m_bottomRuler->VisibilityOff(); // per defecte, fins que no hi hagi input son invisibles

    // coordenades fixes per ancorar els rulers al lateral i a la part inferior
    m_anchoredRulerCoordinates = vtkCoordinate::New();
    m_anchoredRulerCoordinates->SetCoordinateSystemToView();
    m_anchoredRulerCoordinates->SetValue( -0.95 , -0.9 , -0.95 );
}

vtkAxisActor2D* Q2DViewer::createRuler()
{
    vtkAxisActor2D *ruler = vtkAxisActor2D::New();
    ruler->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    ruler->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    ruler->AxisVisibilityOn();
    ruler->TickVisibilityOn();
    ruler->LabelVisibilityOn();
    ruler->AdjustLabelsOff();
    ruler->SetLabelFormat("%.2f");
    ruler->SetLabelFactor( 0.35 );
    ruler->GetLabelTextProperty()->ItalicOff();
    ruler->GetLabelTextProperty()->BoldOff();
    ruler->GetLabelTextProperty()->ShadowOff();
    ruler->GetLabelTextProperty()->SetColor( 0 , 0.7 , 0 );
    ruler->TitleVisibilityOff();
    ruler->SetTickLength( 10 );
    ruler->GetProperty()->SetColor( 0 , 1 , 0 );

    return ruler;
}

void Q2DViewer::updateRulers()
{
    int numRuler = 0;
    int i;
    vtkRenderer *renderer;
    vtkAxisActor2D *sideRuler;
    vtkAxisActor2D *bottomRuler;
    vtkCoordinate *anchoredRulerCoordinates;
    double *anchoredCoordinates;

    switch( m_lastView )
    {
    case Axial:
        for( i = 0; i < m_rendererCollection->GetNumberOfItems(); i++ )
        {
            anchoredRulerCoordinates = vtkCoordinate::SafeDownCast ( m_anchoredRulerCoordinatesCollection->GetItemAsObject( i ));
            renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( i ) );
            anchoredCoordinates = anchoredRulerCoordinates->GetComputedWorldValue( renderer );

            sideRuler = vtkAxisActor2D::SafeDownCast( m_rulerActorCollection->GetItemAsObject( numRuler ) );
            bottomRuler = vtkAxisActor2D::SafeDownCast( m_rulerActorCollection->GetItemAsObject( numRuler+1 ) );
            numRuler += 2;

            sideRuler->GetPositionCoordinate()->SetValue( anchoredCoordinates[0] , m_rulerExtent[3] , 0.0 );
            sideRuler->GetPosition2Coordinate()->SetValue( anchoredCoordinates[0] , m_rulerExtent[2] , 0.0 );
            sideRuler->SetRange( m_rulerExtent[3] , m_rulerExtent[2] );

            bottomRuler->GetPositionCoordinate()->SetValue( m_rulerExtent[1] , anchoredCoordinates[1]  , 0.0 );
            bottomRuler->GetPosition2Coordinate()->SetValue( m_rulerExtent[0] , anchoredCoordinates[1] , 0.0  );
            bottomRuler->SetRange( m_rulerExtent[1] , m_rulerExtent[0] );
        }
    break;

    case Sagital:
        for ( i = 0; i < m_rendererCollection->GetNumberOfItems(); i++)
        {
            anchoredRulerCoordinates = vtkCoordinate::SafeDownCast ( m_anchoredRulerCoordinatesCollection->GetItemAsObject( i ));
            renderer = vtkRenderer::SafeDownCast ( m_rendererCollection->GetItemAsObject( i ));
            anchoredCoordinates = anchoredRulerCoordinates->GetComputedWorldValue( renderer );
            sideRuler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( numRuler ));
            bottomRuler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( numRuler+1 ));
            numRuler += 2;

            sideRuler->GetPositionCoordinate()->SetValue( 0.0 , anchoredCoordinates[1] , m_rulerExtent[4] );
            sideRuler->GetPosition2Coordinate()->SetValue( 0.0 , anchoredCoordinates[1] , m_rulerExtent[5] );
            sideRuler->SetRange( m_rulerExtent[4] , m_rulerExtent[5] );

            bottomRuler->GetPositionCoordinate()->SetValue( 0.0 , m_rulerExtent[3] , anchoredCoordinates[2] );
            bottomRuler->GetPosition2Coordinate()->SetValue( 0.0 , m_rulerExtent[2] , anchoredCoordinates[2] );
            bottomRuler->SetRange( m_rulerExtent[3] , m_rulerExtent[2] );
        }
    break;

    case Coronal:
        for ( i = 0; i < m_rendererCollection->GetNumberOfItems(); i++)
        {
            anchoredRulerCoordinates = vtkCoordinate::SafeDownCast ( m_anchoredRulerCoordinatesCollection->GetItemAsObject( i ));
            renderer = vtkRenderer::SafeDownCast ( m_rendererCollection->GetItemAsObject( i ));
            anchoredCoordinates = anchoredRulerCoordinates->GetComputedWorldValue( renderer );
            sideRuler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( numRuler ));
            bottomRuler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( numRuler+1 ));
            numRuler += 2;

            sideRuler->GetPositionCoordinate()->SetValue( anchoredCoordinates[0] , 0.0 , m_rulerExtent[4] );
            sideRuler->GetPosition2Coordinate()->SetValue( anchoredCoordinates[0] , 0.0 , m_rulerExtent[5] );
            sideRuler->SetRange( m_rulerExtent[4] , m_rulerExtent[5] );

            bottomRuler->GetPositionCoordinate()->SetValue( m_rulerExtent[1] , 0.0 , anchoredCoordinates[2] );
            bottomRuler->GetPosition2Coordinate()->SetValue( m_rulerExtent[0] , 0.0 , anchoredCoordinates[2] );
            bottomRuler->SetRange( m_rulerExtent[1] , m_rulerExtent[0] );
        }
    break;
    }
}

void Q2DViewer::setupDefaultPipeline()
{
    if( m_mainVolume )
    {
        m_viewer->SetInput( m_mainVolume->getVtkData() );
        m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
    }
}

void Q2DViewer::setupThickSlabPipeline()
{
    m_viewer->SetInput( m_thickSlabProjectionFilter->GetOutput() );
    m_windowLevelLUTMapper->SetInput( m_thickSlabProjectionFilter->GetOutput() );
}

vtkScalarBarActor* Q2DViewer::createScalarBar()
{
    vtkScalarBarActor *scalarBar = vtkScalarBarActor::New();
    scalarBar->SetOrientationToVertical();
    scalarBar->GetPositionCoordinate()->SetCoordinateSystemToView();
    scalarBar->SetPosition( 0.8 , -0.8 );
    scalarBar->SetWidth( 0.08 );
    scalarBar->SetHeight( 0.6 );
    scalarBar->SetLabelFormat( " %.f  " );
    scalarBar->SetNumberOfLabels( 3 );
    scalarBar->GetLabelTextProperty()->ItalicOff();
    scalarBar->GetLabelTextProperty()->BoldOff();
    scalarBar->GetLabelTextProperty()->SetJustificationToRight();
    return scalarBar;
}

void Q2DViewer::updateScalarBar()
{
    if( m_mainVolume )
    {
        vtkScalarBarActor *scalarBar;
        int i;

        vtkWindowLevelLookupTable *lookup = vtkWindowLevelLookupTable::New();
        lookup->SetWindow( m_windowLevelLUTMapper->GetWindow() );
        lookup->SetLevel( m_windowLevelLUTMapper->GetLevel() );
        lookup->Build();
        for ( i = 0; i < m_scalarBarCollection->GetNumberOfItems(); i++ )
        {
            scalarBar = vtkScalarBarActor::SafeDownCast( m_scalarBarCollection->GetItemAsObject( i ) );
            scalarBar->SetLookupTable( lookup );
        }
    }
    else
        DEBUG_LOG( "No hi ha cap volum assignat. No podem donar LUT a l'escala de colors" );
}

void Q2DViewer::rotateClockWise()
{
    m_rotateFactor = (m_rotateFactor+1) % 4 ;
    updateCamera();
}

void Q2DViewer::rotateCounterClockWise()
{
    m_rotateFactor = (m_rotateFactor+3) % 4 ;
    updateCamera();
}

void Q2DViewer::setRotationFactor( int factor )
{
    m_rotateFactor = factor;
//     this->updateCameraRotation();
    updateCamera();
}

void Q2DViewer::horizontalFlip()
{
    m_applyFlip = true;
    m_isImageFlipped = ! m_isImageFlipped;
    updateCamera();
}

void Q2DViewer::verticalFlip()
{
    m_rotateFactor = (m_rotateFactor + 2) % 4 ;
    horizontalFlip();
}

void Q2DViewer::setModalityRescale( double slope, double intercept )
{
    if( !m_modalityLUTRescale )
    {
        m_modalityLUTRescale = vtkImageShiftScale::New();
        m_modalityLUTRescale->SetInput( m_mainVolume->getVtkData() );
    }
    m_modalityLUTRescale->SetShift( slope );
    m_modalityLUTRescale->SetScale( intercept );
}

void Q2DViewer::setModalityLUT( vtkWindowLevelLookupTable *lut )
{
    if( !lut && m_modalityLut )
    {
        m_modalityLut->Delete();
    }
    m_modalityLut = lut;
}

void Q2DViewer::setVOILUT( vtkWindowLevelLookupTable *lut )
{
    if( !lut && m_windowLevelLut )
    {
        m_windowLevelLut->Delete();
    }
    m_windowLevelLut = lut;
}

void Q2DViewer::setPresentationLUT( vtkWindowLevelLookupTable *lut )
{
    if( !lut && m_presentationLut )
    {
        m_presentationLut->Delete();
    }
    m_presentationLut = lut;
}

void Q2DViewer::mapOrientationStringToAnnotation()
{
    //\TODO Cal comprovar que els flips siguin correctes
    int index = (m_lastView == Axial) ? m_currentSlice : 0;
    // això es fa per si tenim un mhd que realment només té un arxiu (imatge) però té més llesques
    // TODO caldria millorar l'accés a les imatges a partir del volum, per no haver de fer aquestes filigranes
    // és a dir, al preguntar a Volume, getImage(index) ell ens retorna la imatge que toca i ja comprova rangs si cal
    // i no ens retorna la llista d'imatges a saco
    index = ( index >= m_mainVolume->getImages().size() ) ? 0 : index;
    QString orientation = m_mainVolume->getImages().at(index)->getPatientOrientation();
    QStringList list = orientation.split(",");

    if( list.size() > 1 )
    {
        // 0:Esquerra , 1:Abaix , 2:Dreta , 3:A dalt
        if( m_lastView == Axial )
        {
            m_patientOrientationTextActor[ (0 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(0) ) ) );
            m_patientOrientationTextActor[ (2 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[ (1 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[ (3 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(1) ) ) );
        }
        else if( m_lastView == Sagital )
        {
            m_patientOrientationTextActor[ (0 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(1) ) ) );
            m_patientOrientationTextActor[ (2 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[ (1 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(2) ) ) );
            m_patientOrientationTextActor[ (3 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(2) ) );
        }
        else if( m_lastView == Coronal )
        {
            m_patientOrientationTextActor[ (0 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(0) ) ) );
            m_patientOrientationTextActor[ (2 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[ (1 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( this->getOppositeOrientationLabel( list.at(2) ) ) );
            m_patientOrientationTextActor[ (3 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(2) ) );
        }
        if( m_isImageFlipped )
        {
            // llavors caldrà intercanviar esquerra per dreta
            QString swap( m_patientOrientationTextActor[0]->GetInput() );
            m_patientOrientationTextActor[0]->SetInput( m_patientOrientationTextActor[2]->GetInput() );
            m_patientOrientationTextActor[2]->SetInput( qPrintable( swap ) );
        }
    }
    else
    {
        DEBUG_LOG("L'orientació del pacient és buida. No s'aplicaran annotacions de referència sobre la imatge");
    }
}

void Q2DViewer::refreshAnnotations()
{
    if( !m_mainVolume )
        return;

    if( m_enabledAnnotations & Q2DViewer::PatientInformationAnnotation )
        m_serieInformationAnnotation->VisibilityOn();
    else
        m_serieInformationAnnotation->VisibilityOff();

    vtkCornerAnnotation *sliceAnnotation;
    vtkRenderer *renderer;
    vtkAxisActor2D *ruler;
    int i = 0, rulerIndex = 0;

    // Informació que es mostra per cada viewport
    for ( i = 0; i < m_rendererCollection->GetNumberOfItems(); i++)
    {
        renderer = vtkRenderer::SafeDownCast ( m_rendererCollection->GetItemAsObject( i ) );
        // Mostrem o amaguem els rulers del volum
        ruler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( rulerIndex ));
        if ( m_enabledAnnotations & Q2DViewer::RulersAnnotation )
            ruler->VisibilityOn();
        else
            ruler->VisibilityOff();

        ruler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( rulerIndex+1 ));
        if ( m_enabledAnnotations & Q2DViewer::RulersAnnotation )
            ruler->VisibilityOn();
        else
            ruler->VisibilityOff();

        rulerIndex += 2;

        sliceAnnotation = vtkCornerAnnotation::SafeDownCast ( m_sliceAnnotationsCollection->GetItemAsObject ( i ) );
        // informació de la finestra
        if( m_enabledAnnotations & Q2DViewer::WindowInformationAnnotation )
        {
                m_upperLeftText = tr("Image Size: %1 x %2\nView Size: %3 x %4\nWW: %5 WL: %6 ")
                    .arg( m_imageSizeInformation[0] )
                    .arg( m_imageSizeInformation[1] )
                    .arg( renderer->GetSize()[0] )
                    .arg( renderer->GetSize()[1] )
                    .arg( vtkMath::Round( m_windowLevelLUTMapper->GetWindow() ) )
                    .arg( vtkMath::Round( m_windowLevelLUTMapper->GetLevel() ) );
        }
        else
            m_upperLeftText = "";

        sliceAnnotation->SetText( 2 , qPrintable( m_upperLeftText ) );

        if ( m_enabledAnnotations & Q2DViewer::PatientOrientationAnnotation )
        {
            for( int j = 0; j < 4; j++ )
                m_patientOrientationTextActor[j]->VisibilityOn();
        }
        else
        {
            for( int j = 0; j < 4; j++ )
                m_patientOrientationTextActor[j]->VisibilityOff();
        }

        if ( m_enabledAnnotations & Q2DViewer::ScalarBarAnnotation )
            m_scalarBar->VisibilityOn();
        else
            m_scalarBar->VisibilityOff();
    }
    this->updateSliceAnnotationInformation();
    this->refresh();
}

void Q2DViewer::updateSliceAnnotation( vtkCornerAnnotation *sliceAnnotation, int currentSlice, int maxSlice, int currentPhase, int maxPhase )
{
    if( !sliceAnnotation )
    {
        DEBUG_LOG("l'sliceAnnotation proporcionat és NUL!");
        return;
    }

    if( m_enabledAnnotations & Q2DViewer::SliceAnnotation ) // si les annotacions estan habilitades
    {
        QString lowerLeftText;
        if( maxPhase > 1 ) // tenim fases
        {
            if( m_slabThickness > 1 )
            {
                lowerLeftText = tr("Slice: %1-%2/%3 Phase: %4/%5")
                        .arg( currentSlice )
                        .arg( currentSlice+m_slabThickness-1 )
                        .arg( maxSlice )
                        .arg( currentPhase )
                        .arg( maxPhase );
            }
            else
            {
                lowerLeftText = tr("Slice: %1/%2 Phase: %3/%4")
                        .arg( currentSlice )
                        .arg( maxSlice )
                        .arg( currentPhase )
                        .arg( maxPhase );
            }
        }
        else // només llesques
        {
            if( m_slabThickness > 1 )
            {
                lowerLeftText = tr("Slice: %1-%2/%3")
                        .arg( currentSlice )
                        .arg( currentSlice+m_slabThickness-1 )
                        .arg( maxSlice );
            }
            else
            {
                lowerLeftText = tr("Slice: %1/%2")
                        .arg( currentSlice )
                        .arg( maxSlice );
            }
        }
        //afegim el thickness de la llesca
        lowerLeftText += tr(" Thickness: %1 mm").arg( this->getThickness(), 0, 'g', 2 );

        sliceAnnotation->SetText( 0 , qPrintable(lowerLeftText) );
    }
    else
    {
        sliceAnnotation->SetText( 0 , "" );
    }
}

double Q2DViewer::getThickness()
{
    double thickness;
    switch( m_lastView )
    {
    case Axial:
        // TODO no seria més correcte demanar la informació de la imatge actual?
        thickness = m_mainVolume->getSpacing()[2] * m_slabThickness;
    break;
    case Sagital:
        thickness = m_mainVolume->getSpacing()[0] * m_slabThickness;
    break;
    case Coronal:
        thickness = m_mainVolume->getSpacing()[1] * m_slabThickness;
    break;
    }
    return thickness;
}

void Q2DViewer::getSliceRange(int &min, int &max)
{
    if( m_mainVolume )
    {
        if( m_numberOfPhases == 1 ) // si és un volum 3D normal...
        {
            m_mainVolume->updateInformation();
            int *extent = m_mainVolume->getWholeExtent();
            min = extent[m_lastView * 2];
            max = extent[m_lastView * 2 + 1];
        }
        else // si tenim 4D
        {
            // TODO assumim que sempre estem en axial!
            min = 0;
            max = m_mainVolume->getSeries()->getNumberOfSlicesPerPhase() - 1;
        }
    }
    else
    {
        min = max = 0;
        DEBUG_LOG("Alerta! Demanem el rang de llesques però no tenim volum! Valors de sortida = 0!");
    }
}

int *Q2DViewer::getSliceRange()
{
    if( m_mainVolume )
    {
        int *range = new int[2];
        this->getSliceRange(range[0],range[1]);
        return range;
    }
    else
    {
        return NULL;
    }
}

int Q2DViewer::getMinimumSlice()
{
    int min, trash;
    this->getSliceRange(min,trash);
    return min;
}

int Q2DViewer::getMaximumSlice()
{
    int max, trash;
    this->getSliceRange(trash,max);
    return max;
}

void Q2DViewer::addActors()
{
    // anotacions de texte FIXE
    this->getRenderer()->AddActor2D( m_serieInformationAnnotation );

    // anotacions de texte variable (window/level, window size, etc)
    vtkCornerAnnotation *sliceAnnotation = vtkCornerAnnotation::New();
    this->getRenderer()->AddActor2D( sliceAnnotation );
    m_sliceAnnotationsCollection->AddItem( sliceAnnotation );

    if( m_patientOrientationTextActor[0] )
    {
        this->getRenderer()->AddActor( m_patientOrientationTextActor[0] );
        this->getRenderer()->AddActor( m_patientOrientationTextActor[1] );
        this->getRenderer()->AddActor( m_patientOrientationTextActor[2] );
        this->getRenderer()->AddActor( m_patientOrientationTextActor[3] );
    }
    else
    {
        DEBUG_LOG( "No s'han creat els actors textuals d'informació d'orientació del pacient; no es poden afegir a l'escena" );
    }
    if( m_sideRuler )
        this->getRenderer()->AddActor2D( m_sideRuler );
    else
    {
        DEBUG_LOG( "No s'ha creat l'actor d'indicador d'escala lateral; no es pot afegir a l'escena" );
    }
    if( m_bottomRuler )
        this->getRenderer()->AddActor2D( m_bottomRuler );
    else
    {
        DEBUG_LOG( "No s'ha creat l'actor d'indicador d'escala inferior; no es pot afegir a l'escena" );
    }
    if( m_scalarBar ) // aquest l'hauríem d'afegir quan hi ha un input?
    {
        this->getRenderer()->AddActor2D( m_scalarBar );
    }
    else
    {
        DEBUG_LOG( "No s'ha creat l'actor d'escala de colors; no es pot afegir a l'escena" );
    }
}

QString Q2DViewer::getOppositeOrientationLabel( QString label )
{
    int i = 0;
    QString oppositeLabel;
    while( i < label.size() )
    {
        if( label.at( i ) == 'L' )
            oppositeLabel += "R";
        else if( label.at( i ) == 'R' )
            oppositeLabel += "L";
        else if( label.at( i ) == 'A' )
            oppositeLabel += "P";
        else if( label.at( i ) == 'P' )
            oppositeLabel += "A";
        else if( label.at( i ) == 'S' )
            oppositeLabel += "I";
        else if( label.at( i ) == 'I' )
            oppositeLabel += "S";
        else if( label.at( i ) == 'H' )
            oppositeLabel += "F";
        else if( label.at( i ) == 'F' )
            oppositeLabel += "H";
        else
            oppositeLabel += "?";
        i++;
    }
    return oppositeLabel;
}

void Q2DViewer::setTool( QString toolName )
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

OldTool *Q2DViewer::getTool( QString toolName )
{
    return m_toolManager->getTool( toolName );
}

QString Q2DViewer::getCurrentToolName()
{
    return m_toolManager->getCurrentToolName();
}

void Q2DViewer::setEnableTools( bool enable )
{
    if( enable )
        this->enableTools();
    else
        this->disableTools();
}

void Q2DViewer::enableTools()
{
    /// Això evita que es faci més d'un connect en cas que es cridi aquesta funció i ja s'hagi fet abans
    if(!m_enabledTools)
    {
        connect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
        m_enabledTools = true;
    }
}

void Q2DViewer::disableTools()
{
    if(m_enabledTools)
    {
        disconnect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
        m_enabledTools = false;
    }
}

void Q2DViewer::setupInteraction()
{
    m_picker = vtkPropPicker::New();
    // configurem l'Image Viewer i el qvtkWidget
    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetPicker( m_picker );
    m_viewer->SetupInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );
    m_viewer->GetInteractorStyle()->SetCurrentRenderer( this->getRenderer() );
    //\TODO això dóna un error de vtk perquè el viewer no té input, però no afecta a la execució de l'apicació
    m_vtkWidget->SetRenderWindow( m_viewer->GetRenderWindow() );
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
    this->getInteractor()->RemoveObservers( vtkCommand::MouseWheelForwardEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::MouseWheelBackwardEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::MiddleButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::CharEvent );
}

void Q2DViewer::setInput( Volume* volume )
{
    //al fer un nou input, les distàncies que guardava el drawer no tenen sentit, pertant s'esborren
    if( m_mainVolume )
        m_drawer->removeAllPrimitives();

    if( volume == 0 )
        return;
    m_mainVolume = volume;
    m_currentSlice = 0;
    m_currentPhase = 0;

    // Inicialització del thickSlab
    m_slabThickness = 1;
    m_firstSlabSlice = 0;
    m_lastSlabSlice = 0;
    m_thickSlabActive = false;
    // obtenim valors de gris i aquestes coses
    // aquí es crea tot el pieline del visualitzador
    this->computeInputGrayscalePipeline();
    this->applyGrayscalePipeline();

    int extent[6];
    double origin[3], spacing[3];
    m_mainVolume->getOrigin( origin );
    m_mainVolume->getSpacing( spacing );
    m_mainVolume->getWholeExtent( extent );
    m_rulerExtent[0] = origin[0];
    m_rulerExtent[1] = origin[0] + extent[1]*spacing[0];
    m_rulerExtent[2] = origin[1];
    m_rulerExtent[3] = origin[1] + extent[3]*spacing[1];
    m_rulerExtent[4] = origin[2];
    m_rulerExtent[5] = origin[2] + extent[5]*spacing[2];

    m_numberOfPhases = m_mainVolume->getNumberOfPhases();
    m_maxSliceValue = this->getMaximumSlice();

    // Això es fa per destruir el blender en cas que ja hi hagi algun input i es vulgui canviar
    if(m_blender!=0)
    {
        m_blender->Delete();
        m_blender = 0;
    }

    // Preparem el thickSlab // TODO cada cop que fem setInput resetejem els valors per defecte??
    m_thickSlabProjectionFilter->SetInput( m_mainVolume->getVtkData() );
    m_thickSlabProjectionFilter->SetProjectionDimension( m_lastView );
    m_thickSlabProjectionFilter->SetAccumulatorType( (AccumulatorFactory::AccumulatorType) m_slabProjectionMode );
    m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice );
    m_thickSlabProjectionFilter->SetNumberOfSlicesToProject( m_slabThickness );
    m_thickSlabProjectionFilter->SetStep( m_numberOfPhases );

    updateScalarBar();
    updatePatientAnnotationInformation();
    updateGrid();
    resetViewToAxial();
    this->enableAnnotation( m_enabledAnnotations );

    // actualitzem la informació de window level
    this->updateWindowLevelData();

    // \TODO això no sabem si serà del tot necessari
    //     m_picker->PickFromListOn();
    //     m_picker->AddPickList( m_viewer->GetImageActor() );
}

vtkInteractorStyle *Q2DViewer::getInteractorStyle()
{
    if( m_viewer )
        return m_viewer->GetInteractorStyle();
    else
        return 0;
}

void Q2DViewer::setOverlayInput( Volume* volume )
{
    m_overlayVolume = volume;

    // \TODO s'hauria d'eliminar aquests objectes en algun punt (veure cas del blend)
    vtkImageCheckerboard *imageCheckerBoard = vtkImageCheckerboard::New();
    vtkImageRectilinearWipe *wipe = vtkImageRectilinearWipe::New();
    // \TODO hauríem d'actualitzar valors que es calculen al setInput de la variable m_overlay!
    switch( m_overlay )
    {
    case None:

        // actualitzem el viewer
        m_windowLevelLUTMapper->RemoveAllInputs();
        m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
        // \TODO hauríem d'actualitzar valors que es calculen al setInput!
    break;

    case CheckerBoard:

        imageCheckerBoard->SetInput1( m_mainVolume->getVtkData() );
        imageCheckerBoard->SetInput2( m_overlayVolume->getVtkData() );
        imageCheckerBoard->SetNumberOfDivisions( m_divisions );
        // actualitzem el viewer
        m_viewer->SetInputConnection( imageCheckerBoard->GetOutputPort() ); // li donem el m_imageCheckerboard com a input
        // \TODO hauríem d'actualitzar valors que es calculen al setInput!
    break;

    case Blend:
        if (m_blender==0)
        {
            m_blender = vtkImageBlend::New();
            m_blender->SetInput(0, m_mainVolume->getVtkData());
        }
        m_blender->SetInput(1, m_overlayVolume->getVtkData());
        m_blender->SetOpacity( 1, 1.0 - m_opacityOverlay );
        // \TODO Revisar la manera de donar-li l'input d'un blending al visualitzador
        m_blender->Modified();
        m_windowLevelLUTMapper->SetInputConnection( m_blender->GetOutputPort() );

    break;

    case RectilinearWipe:
        wipe->SetInput( 0 , m_mainVolume->getVtkData() );
        wipe->SetInput( 1 , m_overlayVolume->getVtkData() );
        wipe->SetPosition(20,20);
        wipe->SetWipeToUpperLeft();
        m_viewer->SetInput( wipe->GetOutput() );
        // \TODO hauríem d'actualitzar valors que es calculen al setInput!
    break;
    }
}

void Q2DViewer::setOpacityOverlay ( double op )
{
    m_opacityOverlay=op;
}

void Q2DViewer::render()
{
    // si tenim dades
    if( m_mainVolume )
    {
       // Això és necessari perquè la imatge es rescali a les mides de la finestreta
       // Automatically set up the camera based on the visible actors. The camera will reposition itself to view the center point of the actors, and move along its initial view plane normal (i.e., vector defined from camera position to focal point) so that all of the actors can be seen.
        this->getRenderer()->ResetCamera();
        updateCamera();
    }
    else
    {
        DEBUG_LOG( "::render() : No hi ha cap volum per visualitzar" );
    }
}

void Q2DViewer::resetView( CameraOrientationType view )
{
    m_lastView = view;
    emit viewChanged( m_lastView );
    // thick Slab, li indiquem la direcció de projecció
    m_thickSlabProjectionFilter->SetProjectionDimension( m_lastView );
    //TODO això és necessari perquè encara depenem de m_viewer->SetSliceOrientationTo.... Quan ens fem amb el control total del pipeline això no serà necessari
    setupDefaultPipeline();
    // fins que no ens desfem del SetSliceOrientationTo... i d'alguns problemes del pipeline
    // hem de fer aquestes xapussilles
    if ( isThickSlabActive() )
        m_slabThickness = 2;
    else
        m_slabThickness = 1;
    resetCamera();
    if ( isThickSlabActive() )
    {
        // cada cop que fem reset d'una vista posarem el thickness al mínim, és a dir 2
        // TODO una altre alternativa és mantenir el thickness al màxim del que permet aquella vista si és que ens passem
        // per exemple, tenim un volum de 512x512x28, si en Sagital hem fet un thickness de 50 i passem a Axial, el màxim haurà de ser 28, ja que 50 es passa de rang
//         setSlabThickness(2);
        emit slabThicknessChanged( m_slabThickness );
        // TODO Existeix un altre problema amb els extents si anem canviant de vista i no li donem aquest thickness "segur"
        // cal mirar com fer perquè no hi hagi aquest problema per veure si és cosa del filtre, del mètode setSlabThickness,
        // o per alguna altra causa
        // necessari pel "todo" anterior del default pipeline i restaurar el sistema
        setupThickSlabPipeline();
    }
}

void Q2DViewer::resetViewToAxial()
{
    resetView( Q2DViewer::Axial );
}

void Q2DViewer::resetViewToCoronal()
{
    resetView( Q2DViewer::Coronal );
}

void Q2DViewer::resetViewToSagital()
{
    resetView( Q2DViewer::Sagital );
}

void Q2DViewer::updateCamera()
{
    if( m_viewer->GetInput() )
    {
        vtkCamera *camera;
        vtkRenderer *renderer;
        vtkRendererCollection* renderCollection = m_viewer->GetRenderWindow()->GetRenderers();
        int i = 0;

        switch( this->m_lastView )
        {
        case Axial:
            while( i < (renderCollection->GetNumberOfItems()) && i <= m_maxSliceValue )
            {
                renderer = vtkRenderer::SafeDownCast( renderCollection->GetItemAsObject( i ) );
                camera = renderer->GetActiveCamera();
                if ( camera )
                {
                    camera->SetRoll( -m_rotateFactor*90. + 180. );
                }
                i++;
            }
            emit rotationDegreesChanged( -m_rotateFactor*90. + 180. );
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[1];
        break;
        case Sagital:
            while( i < (renderCollection->GetNumberOfItems()) && i <= m_maxSliceValue )
            {
                renderer = vtkRenderer::SafeDownCast( renderCollection->GetItemAsObject( i ) );
                camera = renderer->GetActiveCamera();
                if ( camera )
                {
                    camera->SetRoll( -m_rotateFactor*90. -90. );
                }
                i++;
            }
            emit rotationDegreesChanged( -m_rotateFactor*90. - 90. );
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[1];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;

        case Coronal:
            while( i < (renderCollection->GetNumberOfItems()) && i <= m_maxSliceValue )
            {
                renderer = vtkRenderer::SafeDownCast( renderCollection->GetItemAsObject( i ) );
                camera = renderer->GetActiveCamera();
                if ( camera )
                {
                    camera->SetRoll( -m_rotateFactor*90. );
                }
                i++;
            }
            emit rotationDegreesChanged( -m_rotateFactor*90. );
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;
        }
        emit rotationFactorChanged( m_rotateFactor );
        //\TODO mirar bé com aplicar el flip,encara no és correcte
        if( m_applyFlip )
        {
            // funciona bé només en axial!
            // Caldria escollir quina és la manera que més convé de fer-ho
            // Alternativa 1)
            // així movem la càmera, però faltaria que la imatge no es mogués de lloc
            camera->Azimuth( 180 );

            // Alternativa 2)
            // D'aquesta manera només movem l'actor. Per contra, el sistema de coordenades queda "igual" cosa que es reflexa en els rulers que no indiquen les coordenades correctes de la imatge
//             double *center;
//             center = m_viewer->GetImageActor()->GetCenter();
//             m_viewer->GetImageActor()->SetOrigin( center );
//             m_viewer->GetImageActor()->RotateY( 180 );

            // Alternativa 3) La que ens proposen a la mailing list. Fa el mateix que azimuth
            // una manera d'arreglar això seria mirar la posició respecte el centre i llavors invertir el desplaçament que hi hagi tant sobre les Y com sobre les X
//             double cameraPosition[3];
//             camera->GetPosition(cameraPosition);
//             double cameraFocalPoint[3];
//             camera->GetFocalPoint(cameraFocalPoint);
//             for ( int i = 0; i < 3; ++i )
//             {
//                 cameraPosition[i] = 2.0*cameraFocalPoint[i] - cameraPosition[i];
//             }
//             camera->SetPosition(cameraPosition);
//             this->getRenderer()->ResetCameraClippingRange();
//             m_viewer->Render();
            m_applyFlip = false;
        }
        emit cameraChanged();
        mapOrientationStringToAnnotation();
        this->refresh();
    }
    else
    {
        WARN_LOG( "Intentant actualitzar rotació de càmera sense haver donat un input abans..." );
    }
}

void Q2DViewer::resetCamera()
{
    if( m_viewer->GetInput()  )
    {
        // en comptes de fer servir sempre this->getMaximumSlice(), actualitzem
        // aquest valor quan cal, és a dir, al posar input i al canviar de vista
        // estalviant-nos crides i crides
        m_maxSliceValue = this->getMaximumSlice();

        m_rotateFactor = 0;
        m_applyFlip = false;
        m_isImageFlipped = false;
        vtkCamera *camera;
        vtkRenderer *renderer;
        int i = 0;
        double *bounds = m_viewer->GetImageActor()->GetBounds();

        switch( m_lastView )
        {
        case Axial:
            m_viewer->SetSliceOrientationToXY();
            while( i < (m_rendererCollection->GetNumberOfItems()) && i <= m_maxSliceValue )
            {
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( i ) );
                camera = renderer->GetActiveCamera();
                if ( camera )
                {
                    camera->SetFocalPoint(0,0,0);
                    camera->SetViewUp(0,-1,0);
                    camera->SetPosition(0,0,-1);
                    camera->SetRoll( -m_rotateFactor*90. + 180. );
                    renderer->ResetCamera();
                    scaleToFit3D( bounds[1], bounds[3], 0.0, bounds[0], bounds[2], 0.0, 0.0 );
                    //scaleToFit( bounds[1], bounds[3], bounds[0], bounds[2] );
                    updateCamera();
                }
                i++;
            }
            emit rotationDegreesChanged( -m_rotateFactor*90. + 180. );
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[1];
        break;

        case Sagital:
            m_viewer->SetSliceOrientationToYZ();
            while( i < (m_rendererCollection->GetNumberOfItems()) && i <= m_maxSliceValue )
            {
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( i ) );
                camera = renderer->GetActiveCamera();
                if ( camera )
                {
                    camera->SetFocalPoint(0,0,0);
                    camera->SetPosition(1,0,0); // -1 if medical ?
                    camera->SetViewUp(0,0,1);
                    camera->SetRoll( -m_rotateFactor*90. -90. );
                    renderer->ResetCamera();
                    scaleToFit3D( 0.0, bounds[2], bounds[5], 0.0, bounds[3], bounds[4], 0.1 );

                    // TODO solucio inmediata per afrontar el ticket #355, pero s'hauria de fer d'una manera mes elegant i consistent
                    QString position = m_mainVolume->getSeries()->getPatientPosition();
                    if( position == "FFP" || position == "HFP" )
                        m_rotateFactor = (m_rotateFactor+2) % 4 ;

                    updateCamera();
                }
                emit rotationDegreesChanged( -m_rotateFactor*90. - 90. );
                i++;
            }
            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[1];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;

        case Coronal:
            m_viewer->SetSliceOrientationToXZ();
            while( i < (m_rendererCollection->GetNumberOfItems()) && i <= m_maxSliceValue )
            {
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( i ) );
                camera = renderer->GetActiveCamera();
                if ( camera )
                {
                    camera->SetFocalPoint(0,0,0);
                    camera->SetPosition(0,-1,0); // 1 if medical ?
                    camera->SetViewUp(0,0,1);
                    camera->SetRoll( -m_rotateFactor*90. );
                    renderer->ResetCamera();
                    scaleToFit3D( bounds[1], 0.0, bounds[4], bounds[0], 0.0, bounds[5], 0.1 );

                    // TODO solucio inmediata per afrontar el ticket #355, pero s'hauria de fer d'una manera mes elegant i consistent
                    QString position = m_mainVolume->getSeries()->getPatientPosition();
                    if( position == "FFP" || position == "HFP" )
                        m_rotateFactor = (m_rotateFactor+2) % 4 ;

                    updateCamera();
                }
                emit rotationDegreesChanged( -m_rotateFactor*90. );
                i++;
            }
            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;
        }

        if( m_lastView == Axial ) // en axial sempre començarem a visualitzar des de la llesca 0
            setSlice(0);
        else // posem la llesca del mig
            setSlice( m_maxSliceValue/2 );

        emit cameraChanged();
        mapOrientationStringToAnnotation();
        updateAnnotationsInformation( Q2DViewer::WindowInformationAnnotation );
        this->updateDisplayExtent();
        this->refresh();
    }
    else
    {
        WARN_LOG( "Intentant canviar de vista sense haver donat un input abans..." );
    }
}

vtkImageMapToWindowLevelColors *Q2DViewer::getWindowLevelMapper() const
{
    return m_windowLevelLUTMapper;
}

void Q2DViewer::setSlice( int value )
{
    if( this->m_mainVolume && this->m_currentSlice != value )
    {
//         this->projectCurrentDICOMPlaneToVTK();

        // thick slab
        if( value < 0 )
            m_currentSlice = 0;
        else if( value + m_slabThickness-1 > m_maxSliceValue )
            m_currentSlice = m_maxSliceValue - m_slabThickness + 1;
        else
            m_currentSlice = value;

        m_firstSlabSlice = m_currentSlice;

        if( isThickSlabActive() )
        {
            m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice );
            m_thickSlabProjectionFilter->SetNumberOfSlicesToProject( m_slabThickness );
            //si hi ha el thickslab activat, eliminem totes les roi's. És la decisió ràpida que s'ha près.
            this->getDrawer()->removeAllPrimitives();
        }
        // fi thick slab

        // Calcular si necessitarem més renders o menys, ja que al canviar de llesca ens trobem que potser tenim un grid per una llesca que no es veu i passem a veure o al revés
        int newNumberOfRenderers=0;
        int i;
        int slice = m_currentSlice;
        int *grid;
        QMap<int, int*>::iterator mapIterator;

        for( i = 0; i < m_numberOfSlicesWindows; i++ )
        {
            mapIterator = m_phaseGridMap.find( slice );
            if( mapIterator != m_phaseGridMap.end() )
            {
                grid = mapIterator.value();
                newNumberOfRenderers += ( grid[0]*grid[1] );
            }
            else newNumberOfRenderers += 1;

            if (slice == m_maxSliceValue) slice = 0;
            else slice += 1;
        }

        int renderersAnteriors = m_rendererCollection->GetNumberOfItems();
        newNumberOfRenderers -= renderersAnteriors;

        if( newNumberOfRenderers > 0 )
        {
            for( i = 0; i < newNumberOfRenderers; i++ )
            {
                addRenderScene();
            }
        }
        // eliminar renderers per fase
        else if( newNumberOfRenderers < 0 )
        {
            for( i = 0; i > newNumberOfRenderers; i-- )
            {
                removeRenderScene();
            }
        }
        this->updateDisplayExtent();
        mapOrientationStringToAnnotation();
        emit sliceChanged( m_currentSlice );
        this->refresh();
    }
}

void Q2DViewer::setPhase( int value )
{
    // comprovació de rang
    if( m_mainVolume )
    {
        if( value < 0 )
            value = 0;
        else if( value > m_numberOfPhases - 1 )
            value = m_numberOfPhases - 1;

        m_currentPhase = value;
        this->updateDisplayExtent();
        emit phaseChanged( m_currentPhase );
        this->refresh();
    }
}

void Q2DViewer::setOverlay( OverlayType overlay )
{
    m_overlay = overlay;
}

void Q2DViewer::setNoOverlay()
{
    setOverlay( Q2DViewer::None );
    DEBUG_LOG("Passem pel setNoOverlay()");
    m_windowLevelLUTMapper->RemoveAllInputs();
    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
}

void Q2DViewer::setOverlayToBlend()
{
    setOverlay( Q2DViewer::Blend );
}

void Q2DViewer::setOverlayToCheckerBoard()
{
    setOverlay( Q2DViewer::CheckerBoard );
}

void Q2DViewer::setOverlayToRectilinearWipe()
{
    setOverlay( Q2DViewer::RectilinearWipe );
}

void Q2DViewer::resizeEvent( QResizeEvent *resize )
{
    if( m_mainVolume )
    {
        // l'única info que cal actualitzar és la mida de finestra/viewport
        updateAnnotationsInformation( Q2DViewer::WindowInformationAnnotation );
        updateRulers();
    }
}

void Q2DViewer::setWindowLevel( double window , double level )
{
    if( m_mainVolume )
    {
        m_windowLevelLUTMapper->SetWindow( window );
        m_windowLevelLUTMapper->SetLevel( level );
        updateAnnotationsInformation( Q2DViewer::WindowInformationAnnotation );
        updateScalarBar();
        refresh();
        emit windowLevelChanged( window , level );
    }
    else
    {
        DEBUG_LOG( "::setWindowLevel() : No tenim input " );
    }
}

void Q2DViewer::getCurrentWindowLevel( double wl[2] )
{
    if( m_mainVolume )
    {
        wl[0] = m_windowLevelLUTMapper->GetWindow();
        wl[1] = m_windowLevelLUTMapper->GetLevel();
    }
    else
    {
        DEBUG_LOG( "::getCurrentWindowLevel() : No tenim input " );
    }
}

double Q2DViewer::getCurrentColorWindow()
{
    if( m_mainVolume )
    {
        return m_windowLevelLUTMapper->GetWindow();
    }
    else
    {
        DEBUG_LOG( "::getCurrentColorWindow() : No tenim input " );
        return 0;
    }
}

double Q2DViewer::getCurrentColorLevel()
{
    if( m_mainVolume )
    {
        return m_windowLevelLUTMapper->GetLevel();
    }
    else
    {
        DEBUG_LOG( "::getCurrentColorLevel() : No tenim input " );
        return 0;
    }
}

int Q2DViewer::getCurrentSlice() const
{
    return m_currentSlice;
}

int Q2DViewer::getCurrentPhase() const
{
    return m_currentPhase;
}

void Q2DViewer::resetWindowLevelToDefault()
{
    // això ens dóna un level/level "maco" per defecte
    // situem el level al mig i donem un window complet de tot el rang
    //\TODO aquí caldria tenir en compte el default del presentation state actual si l'hi ha
    if( m_mainVolume )
    {
        m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
        m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
        this->refresh();
        //\TODO fer updateAnnotationsInformation() en comptes d'aquest?
        updateWindowLevelAnnotation();
    }
    else
    {
        DEBUG_LOG( "::resetWindowLevelToDefault() : No tenim input" );
    }
}

void Q2DViewer::setModalityRescale( vtkImageShiftScale *rescale )
{
    m_modalityLUTRescale = rescale;
}

vtkImageActor *Q2DViewer::getImageActor()
{
    return m_viewer->GetImageActor();
}

void Q2DViewer::setPixelAspectRatio( double ratio )
{
    if( ratio != 1.0 && ratio > 0.0 )
    {
        m_mainVolume->updateInformation();
        double spacing[3];
        m_mainVolume->getSpacing( spacing );

        vtkImageChangeInformation *change = vtkImageChangeInformation::New();
        change->SetInput( m_viewer->GetImageActor()->GetInput() );

        if( ratio > 1.0 )
            change->SetOutputSpacing( spacing[0]*ratio, spacing[1], spacing[2] );
        else
            change->SetOutputSpacing( spacing[0], spacing[1]*ratio, spacing[2] );
        m_viewer->GetImageActor()->SetInput( change->GetOutput() );
    }
    else
    {
        DEBUG_LOG( QString("Ratio no aplicable: %1").arg(ratio) );
    }
}

void Q2DViewer::setPresentationPixelSpacing( double x , double y )
{
    m_presentationPixelSpacing[0] = x;
    m_presentationPixelSpacing[1] = y;
}

void Q2DViewer::setTrueSizeMode( bool on )
{
    if( on )
    {
        DEBUG_LOG("Presentation size mode: TRUE SIZE:: NO IMPLEMENTAT ENCARA!");
        // \TODO això encara no funciona
    }
}

void Q2DViewer::setMagnificationFactor( double factor )
{
    // presumiblement un SetScale(factor,factor,1) de l'ImageActor podria ser una solució alternativa
    if( factor > 0.0 && factor != 1.0 )
    {
        m_mainVolume->updateInformation();
        double spacing[3];
        m_mainVolume->getSpacing( spacing );
        vtkImageChangeInformation *change = vtkImageChangeInformation::New();
        change->SetInput( m_viewer->GetImageActor()->GetInput() );
        change->SetOutputSpacing( spacing[0]*factor, spacing[1]*factor, spacing[2] );
        vtkImageResample *resample = vtkImageResample::New();
        resample->SetInput( change->GetOutput() );
        resample->SetAxisMagnificationFactor( 0, factor );
        resample->SetAxisMagnificationFactor( 1, factor );
        resample->SetAxisMagnificationFactor( 2, 1.0 );
        resample->SetAxisOutputSpacing( 0, spacing[0]*factor );
        resample->SetAxisOutputSpacing( 1, spacing[1]*factor );
        resample->SetAxisOutputSpacing( 2, 1.0 );
        resample->Update();
        m_viewer->GetImageActor()->SetInput( resample->GetOutput() );
    }
    else
    {
        DEBUG_LOG( QString("Factor no aplicable: %1").arg(factor) );
    }
}

Image *Q2DViewer::getCurrentDisplayedImage() const
{
    Image *image = NULL;
    if( m_mainVolume )
    {
        if( m_mainVolume->getSeries() )
        {
            if( ( m_currentSlice*m_numberOfPhases + m_currentPhase ) < m_mainVolume->getSeries()->getImages().count() )
            {
                image = m_mainVolume->getSeries()->getImages().at( m_currentSlice*m_numberOfPhases + m_currentPhase );
            }
        }
    }
    return image;
}

Image *Q2DViewer::getImage( int sliceNumber, int phaseNumber ) const
{
    Image *image = NULL;
    if( m_mainVolume )
    {
        if( m_mainVolume->getSeries() )
        {
            if( ( sliceNumber*m_numberOfPhases + phaseNumber ) < m_mainVolume->getSeries()->getImages().count() )
            {
                image = m_mainVolume->getSeries()->getImages().at( sliceNumber*m_numberOfPhases + phaseNumber );
            }
        }
    }
    return image;
}

ImagePlane *Q2DViewer::getCurrentImagePlane()
{
    ImagePlane *imagePlane = 0;
    if( m_mainVolume )
    {
        int *dimensions = m_mainVolume->getDimensions();
        double *spacing = m_mainVolume->getSpacing();
        double *origin  = m_mainVolume->getOrigin();
        double *bounds = this->getImageActor()->GetBounds();

        switch( m_lastView )
        {
            case Axial: // XY
            {
                Image *image = this->getCurrentDisplayedImage();
                if( image )
                {
                    imagePlane = new ImagePlane();
                    imagePlane->fillFromImage( image );
                }
            }
            break;

            case Sagital: // YZ TODO encara no esta comprovat que aquest pla sigui correcte
            {
                Image *image = m_mainVolume->getSeries()->getImages().at(0);
                if( image )
                {
                    imagePlane = new ImagePlane();
                    const double *dirCosines = image->getImageOrientationPatient();

                    imagePlane->setRowDirectionVector( dirCosines[3], dirCosines[4], dirCosines[5] );
                    imagePlane->setColumnDirectionVector( dirCosines[6], dirCosines[7], dirCosines[8] );



                    imagePlane->setSpacing( spacing[0], spacing[2] );
                    imagePlane->setThickness( this->getThickness() );
                    imagePlane->setRows( dimensions[0] );
                    imagePlane->setColumns( dimensions[2] );
                    // TODO falta esbrinar si l'origen que estem donant es bo o no
    //                 imagePlane->setOrigin( image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2] );
    //                 imagePlane->setOrigin( bounds[0], bounds[2], bounds[4] );
                    imagePlane->setOrigin( origin[0] + spacing[0]*m_currentSlice, origin[1] /*+ spacing[1]*dimensions[1]*/, origin[2] /*+ spacing[2]*dimensions[2]*/ );


                    imagePlane->setOrigin( origin[0] + spacing[0]*m_currentSlice * dirCosines[3] +
                                                       spacing[0]*m_currentSlice * dirCosines[4] +
                                                       spacing[0]*m_currentSlice * dirCosines[5]
                        , origin[1] /*+ spacing[1]*dimensions[1]*/, origin[2] /*+ spacing[2]*dimensions[2]*/ );
                    // TODO comprovar que això sigui correcte
                    imagePlane->getOrigin( origin );
                    imagePlane->setSliceLocation( origin[0] );
                }
            }
            break;

            case Coronal: // XZ TODO encara no esta comprovat que aquest pla sigui correcte
            {
                Image *image = m_mainVolume->getSeries()->getImages().at(0);
                if( image )
                {
                    imagePlane = new ImagePlane();
                    const double *dirCosines = image->getImageOrientationPatient();

                    imagePlane->setRowDirectionVector( dirCosines[0], dirCosines[1], dirCosines[2] );
                    imagePlane->setColumnDirectionVector( dirCosines[6], dirCosines[7], dirCosines[8] );

                    imagePlane->setSpacing( spacing[1], spacing[2] );
                    imagePlane->setThickness( this->getThickness() );
                    imagePlane->setRows( dimensions[1] );
                    imagePlane->setColumns( dimensions[2] );

                    // TODO falta esbrinar si l'origen que estem donant es bo o no
    //                 imagePlane->setOrigin( image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2] );
    //                 imagePlane->setOrigin( bounds[0], bounds[2], bounds[4] );
    //                 imagePlane->setOrigin( bounds[0], bounds[3], bounds[5] );
                    imagePlane->setOrigin( origin[0], origin[1] + /*spacing[1]*/image->getPixelSpacing()[1]*m_currentSlice, origin[2] /*+ spacing[2]*dimensions[2]*/ );
                    // TODO comprovar que això sigui correcte
                    imagePlane->getOrigin( origin );
                    imagePlane->setSliceLocation( origin[1] );
                }
            }
            break;
        }
    }
    return imagePlane;
}

ImagePlane *Q2DViewer::getImagePlane( int sliceNumber , int phaseNumber )
{
    ImagePlane *imagePlane = 0;
    if( m_mainVolume )
    {
        int *dimensions = m_mainVolume->getDimensions();
        double *spacing = m_mainVolume->getSpacing();
        double *origin  = m_mainVolume->getOrigin();
        double *bounds = this->getImageActor()->GetBounds();

        switch( m_lastView )
        {
            case Axial: // XY
            {
                Image *image = this->getImage( sliceNumber, phaseNumber );
                if( image )
                {
                    imagePlane = new ImagePlane();
                    const double *dirCosines = image->getImageOrientationPatient();

                    imagePlane->setRowDirectionVector( dirCosines[0], dirCosines[1], dirCosines[2] );
                    imagePlane->setColumnDirectionVector( dirCosines[3], dirCosines[4], dirCosines[5] );
                    imagePlane->setSpacing( image->getPixelSpacing()[0], image->getPixelSpacing()[1] );
                    // TODO no estem
                    imagePlane->setThickness( this->getThickness() );
                    imagePlane->setRows( image->getRows() );
                    imagePlane->setColumns( image->getColumns() );
                    imagePlane->setOrigin( image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2] );
                }
            }
            break;

            case Sagital: // YZ TODO encara no esta comprovat que aquest pla sigui correcte
            {
                Image *image = m_mainVolume->getSeries()->getImages().at(0);
                if( image )
                {
                    imagePlane = new ImagePlane();
                    const double *dirCosines = image->getImageOrientationPatient();

                    imagePlane->setRowDirectionVector( dirCosines[3], dirCosines[4], dirCosines[5] );
                    imagePlane->setColumnDirectionVector( dirCosines[6], dirCosines[7], dirCosines[8] );
                    imagePlane->setSpacing( spacing[0], spacing[2] );
                    imagePlane->setThickness( this->getThickness() );
                    imagePlane->setRows( dimensions[1] );
                    imagePlane->setColumns( dimensions[2] );
                    // TODO falta esbrinar si l'origen que estem donant es bo o no
                    imagePlane->setOrigin( origin[0] + dirCosines[0]*sliceNumber*spacing[0], origin[1] + dirCosines[1]*sliceNumber*spacing[0], origin[2] + dirCosines[2]*sliceNumber*spacing[0]);
                }
            }
            break;

            case Coronal: // XZ TODO encara no esta comprovat que aquest pla sigui correcte
            {
                Image *image = m_mainVolume->getSeries()->getImages().at(0);
                if( image )
                {
                    imagePlane = new ImagePlane();
                    const double *dirCosines = image->getImageOrientationPatient();

                    imagePlane->setRowDirectionVector( dirCosines[0], dirCosines[1], dirCosines[2] );
                    imagePlane->setColumnDirectionVector( dirCosines[6], dirCosines[7], dirCosines[8] );
                    imagePlane->setSpacing( spacing[1], spacing[2] );
                    imagePlane->setThickness( this->getThickness() );
                    imagePlane->setRows( dimensions[2] );
                    imagePlane->setColumns( dimensions[0] );
                    // TODO falta esbrinar si l'origen que estem donant es bo o no
                    imagePlane->setOrigin( origin[0] + dirCosines[3]*sliceNumber*spacing[1], origin[1] + dirCosines[4]*sliceNumber*spacing[1], origin[2] + dirCosines[5]*sliceNumber*spacing[1]);
                }
            }
            break;
        }
    }
    return imagePlane;
}

void Q2DViewer::projectDICOMPointToCurrentDisplayedImage( const double pointToProject[3], double projectedPoint[3] )
{
    //
    // AQUÍ SUMEM L'origen TAL CUAL + L'ERROR DE DESPLAÇAMENT VTK
    //
    // La projecció es fa de la següent manera:
    // Primer es fa una  una projecció convencional del punt sobre el pla actual (DICOM)
    // Com que el mapeig de coordenades VTK va a la seva bola, necessitem corretgir el desplaçament
    // introduit per VTK respecte a les coordenades "reals" de DICOM
    // aquest desplaçament consistirà en tornar a sumar l'origen del primer pla del volum
    // en principi, fer-ho amb l'origen de m_mainVolume també seria correcte
    //
    ImagePlane *currentPlane = this->getCurrentImagePlane();
    if( currentPlane )
    {
        // recollim les dades del pla actual sobre el qual volem projectar el punt de l'altre pla
        double currentPlaneRowVector[3], currentPlaneColumnVector[3], currentPlaneNormalVector[3], currentPlaneOrigin[3];
        currentPlane->getRowDirectionVector( currentPlaneRowVector );
        currentPlane->getColumnDirectionVector( currentPlaneColumnVector );
        currentPlane->getNormalVector( currentPlaneNormalVector );
        currentPlane->getOrigin( currentPlaneOrigin );

        // a partir d'aquestes dades creem la matriu de projecció,
        // que projectarà el punt donat sobre el pla actual
        vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
        projectionMatrix->Identity();
        for( int column = 0; column < 3; column++ )
        {
            projectionMatrix->SetElement(0,column,currentPlaneRowVector[ column ]);
            projectionMatrix->SetElement(1,column,currentPlaneColumnVector[ column ]);
            projectionMatrix->SetElement(2,column,currentPlaneNormalVector[ column ]);
        }

        // un cop tenim la matriu podem fer la projeccio
        // necessitem el punt en coordenades homogenies
        double homogeneousPointToProject[4], homogeneousProjectedPoint[4];
        for( int i=0; i<3; i++ )
            homogeneousPointToProject[i] = pointToProject[i] - currentPlaneOrigin[i]; // desplacem el punt a l'origen del pla
        homogeneousPointToProject[3] = 1.0;

        // projectem el punt amb la matriu
        projectionMatrix->MultiplyPoint( homogeneousPointToProject, homogeneousProjectedPoint );

        //
        // CORRECIÓ VTK!
        //
        // a partir d'aquí cal corretgir l'error introduit pel mapeig que fan les vtk
        // cal sumar l'origen de la primera imatge, o el que seria el mateix, l'origen de m_mainVolume
        //

        // TODO provar si amb l'origen de m_mainVolume també funciona bé
        Image *firstImage = m_mainVolume->getImages().at(0);
        const double *ori = firstImage->getImagePositionPatient();

        for( int i = 0; i<3; i++ )
            projectedPoint[i] = homogeneousProjectedPoint[i] + ori[i];

    }
    else
        DEBUG_LOG("No hi ha cap pla actual valid");
}

Drawer *Q2DViewer::getDrawer() const
{
    return m_drawer;
}

bool Q2DViewer::getCurrentCursorPosition( double xyz[3] )
{
    bool found = false;
    if( !m_mainVolume )
        return found;
    // agafem el punt que està apuntant el ratolí en aquell moment \TODO podríem passar-li el 4t parèmatre opcional (vtkPropCollection) per indicar que només agafi de l'ImageActor, però no sembla que suigui necessari realment i que si fa pick d'un altre actor 2D no passa res
    m_picker->PickProp( this->getEventPositionX(), this->getEventPositionY(), m_viewer->GetRenderer() );
    // calculem el pixel trobat
    m_picker->GetPickPosition( xyz );

    // quan dona una posició de (0,0,0) és que estem fora de l'actor
    if( !( xyz[0] == 0 && xyz[1] == 0 && xyz[2] == 0) )
    {
        double tolerance;
        int subCellId;
        double parametricCoordinates[3], interpolationWeights[8];

        // Use tolerance as a function of size of source data
        tolerance = m_mainVolume->getVtkData()->GetLength();
        tolerance = tolerance ? tolerance*tolerance / 1000.0 : 0.001;

        // Find the cell that contains q and get it
        vtkCell *cell = m_mainVolume->getVtkData()->FindAndGetCell( xyz , NULL , -1 , tolerance , subCellId , parametricCoordinates , interpolationWeights );
        if ( cell )
        {
            found = true;
        }
    }
    if( !found )
    {
        xyz[0] = -1;
        xyz[1] = -1;
        xyz[2] = -1;
    }

    return found;
}

double Q2DViewer::getCurrentImageValue()
{
    double xyz[3];
    bool found = false;
    double imageValue;
    if( this->getCurrentCursorPosition(xyz) )
    {
        double tolerance;
        int subCellId;
        double parametricCoordinates[3], interpolationWeights[8];

        vtkPointData *pointData = m_mainVolume->getVtkData()->GetPointData();
        vtkPointData* outPointData = vtkPointData::New();
        outPointData->InterpolateAllocate( pointData , 1 , 1 );

        // Use tolerance as a function of size of source data
        tolerance = m_mainVolume->getVtkData()->GetLength();
        tolerance = tolerance ? tolerance*tolerance / 1000.0 : 0.001;

        // Find the cell that contains q and get it
        vtkCell *cell = m_mainVolume->getVtkData()->FindAndGetCell( xyz , NULL , -1 , tolerance , subCellId , parametricCoordinates , interpolationWeights );
        if ( cell )
        {
            // Interpolate the point data
            outPointData->InterpolatePoint( pointData , 0 , cell->PointIds , interpolationWeights );
            imageValue = outPointData->GetScalars()->GetTuple1(0);
            found = true;
        }
        outPointData->Delete();
    }
    if( !found )
    {
        DEBUG_LOG("No s'ha trobat valor");
        return -1.;
    }
    else
    {
        return imageValue;
    }
}

void Q2DViewer::updateWindowLevelAnnotation()
{
    updateAnnotationsInformation( Q2DViewer::WindowInformationAnnotation );
    emit windowLevelChanged( m_windowLevelLUTMapper->GetWindow() , m_windowLevelLUTMapper->GetLevel() );
    updateScalarBar();
}

Q2DViewer::CameraOrientationType Q2DViewer::getView() const
{
    return m_lastView;
}

vtkImageViewer2 *Q2DViewer::getImageViewer() const
{
    return m_viewer;
}

void Q2DViewer::reset()
{
    //\TODO: completar, encara és incomplert
    resetViewToAxial();
}

void Q2DViewer::setDivisions( int x , int y , int z )
{
    m_divisions[0] = x;
    m_divisions[1] = y;
    m_divisions[2] = z;
}

void Q2DViewer::setDivisions( int data[3] )
{
    m_divisions[0] = data[0];
    m_divisions[1] = data[1];
    m_divisions[2] = data[2];
}

int* Q2DViewer::getDivisions( void )
{
    return m_divisions;
}

void Q2DViewer::getDivisions( int data[3] )
{
    data[0] = m_divisions[0];
    data[1] = m_divisions[1];
    data[2] = m_divisions[2];
}

void Q2DViewer::setSeedPosition( double pos[3] )
{
    m_seedPosition[0] = pos[0];
    m_seedPosition[1] = pos[1];
    m_seedPosition[2] = pos[2];
    emit seedChanged();
}

void Q2DViewer::getSeedPosition( double pos[3] )
{
    pos[0] = m_seedPosition[0];
    pos[1] = m_seedPosition[1];
    pos[2] = m_seedPosition[2];
}

void Q2DViewer::saveAll( QString baseName , FileType extension )
{
    switch( extension )
    {
    case PNG:
    break;

    case JPEG:
    break;

    case TIFF:
    break;

    case DICOM:
    break;

    case META:
    break;

    case PNM:
    break;

    case BMP:
    break;
    }
}

void Q2DViewer::saveCurrent( QString baseName , FileType extension )
{
    m_windowToImageFilter->Update();
    m_windowToImageFilter->Modified();
    vtkImageData *image = m_windowToImageFilter->GetOutput();
    switch( extension )
    {
        case PNG:
        {
            vtkImageWriter *pngWriter = vtkPNGWriter::New();
            pngWriter->SetInput( image );
            pngWriter->SetFilePattern( "%s-%d.png" );
            pngWriter->SetFilePrefix( qPrintable(baseName) );
            pngWriter->Write();

            break;
        }
        case JPEG:
        {
            vtkImageWriter *jpegWriter = vtkJPEGWriter::New();
            jpegWriter->SetInput( image );
            jpegWriter->SetFilePattern( "%s-%d.jpg" );
            jpegWriter->SetFilePrefix( qPrintable(baseName) );
            jpegWriter->Write();

            break;
        }
        // \TODO el format tiff fa petar al desar, mirar si és problema de compatibilitat del sistema o de les pròpies vtk
        case TIFF:
        {
            vtkImageWriter *tiffWriter = vtkTIFFWriter::New();
            tiffWriter->SetInput( image );
            tiffWriter->SetFilePattern( "%s-%d.tif" );
            tiffWriter->SetFilePrefix( qPrintable(baseName) );
            tiffWriter->Write();

            break;
        }
        case PNM:
        {
            vtkImageWriter *pnmWriter = vtkPNMWriter::New();
            pnmWriter->SetInput( image );
            pnmWriter->SetFilePattern( "%s-%d.pnm" );
            pnmWriter->SetFilePrefix( qPrintable(baseName) );
            pnmWriter->Write();

            break;
        }
        case BMP:
        {
            vtkImageWriter *bmpWriter = vtkBMPWriter::New();
            bmpWriter->SetInput( image );
            bmpWriter->SetFilePattern( "%s-%d.bmp" );
            bmpWriter->SetFilePrefix( qPrintable(baseName) );
            bmpWriter->Write();

            break;
        }
        case DICOM:
        {
            break;
        }
        case META:
        {
            vtkMetaImageWriter *metaWriter = vtkMetaImageWriter::New();
            metaWriter->SetInput( m_mainVolume->getVtkData() );
            metaWriter->SetFileName( qPrintable(baseName) );
            metaWriter->Write();

            break;
        }
    }
}

void Q2DViewer::setRows( int rows )
{
    if( rows > 0 )
    {
        m_rows = rows;
        this->updateGrid();
    }
}

void Q2DViewer::addRows( int rows )
{
    if( rows > 0 )
        this->setRows( m_rows + rows );
}

void Q2DViewer::removeRows( int rows )
{
    if( (m_rows - rows) > 0 )
        this->setRows( m_rows - rows );
}

void Q2DViewer::setColumns( int columns )
{
    if( columns > 0 )
    {
        m_columns = columns;
        this->updateGrid();
    }
}

void Q2DViewer::removeColumns( int columns )
{
    if( (m_columns - columns) > 0 )
        this->setColumns( m_columns - columns );
}

void Q2DViewer::addColumns( int columns )
{

    if( columns > 0 )
        this->setColumns( m_columns + columns );
}

void Q2DViewer::setGrid( int rows, int columns )
{
    if( columns > 0 && rows > 0)
    {
        if (columns != m_columns || rows != m_rows)
        {
            m_columns = columns;
            m_rows = rows;
            this->updateGrid();
        }
    }
}

void Q2DViewer::setPhaseRows( int slice, int rows )
{
    if( rows > 0 && m_numberOfPhases > 1)
    {
        int *value;
        int renderersCount = 0;
        QMap<int, int*>::iterator mapIterator;
        mapIterator = m_phaseGridMap.find( slice );
        if( mapIterator != m_phaseGridMap.end() )
        {
            value = mapIterator.value();
            renderersCount = (rows * value[1]) - (value[0]*value[1]);
            value[0] = rows;
        }
        else
        {
            value = new int[2];
            value[0] = rows;
            value[1] = 1;
            renderersCount = rows - 1;

        }
        // afegir renderers per fase
        if( renderersCount > 0 )
        {
            for( int i = 0; i < renderersCount; i++ )
            {
                addRenderScene();
            }
        }
        // eliminar renderers per fase
        else if( renderersCount < 0 )
        {
            for( int i = 0; i > renderersCount; i-- )
            {
                removeRenderScene();
            }
        }
        // actualitzem o insertem un nou
        m_phaseGridMap.insert( slice, value );
        this->updateGrid();
    }
}

void Q2DViewer::addPhaseRows( int slice, int rows )
{
    if( rows > 0 )
    {
        QMap<int, int*>::iterator mapIterator;
        mapIterator = m_phaseGridMap.find( slice );
        if( mapIterator != m_phaseGridMap.end() )
        {
            // si ja en tenim, hem de sumar els actuals més al valor que donem
            int *value = mapIterator.value();
            rows += rows + value[0];
        }// sinó serà com fer un set tal qual i prou
        this->setRows( rows );
    }
}

void Q2DViewer::removePhaseRows( int slice, int rows )
{
    QMap<int, int*>::iterator mapIterator;
    mapIterator = m_phaseGridMap.find( slice );
    if( mapIterator != m_phaseGridMap.end() )
    {
        // si ja en tenim, hem de restar els actuals més al valor que donem
        int *value = mapIterator.value();
        if( (value[0] - rows) > 0 )
        {
            rows = value[0] - rows;
        }
        else // si ens passem de llarg hem de deixar un row com a mínim
        {
            rows = 1;
        }
        // assignem la quantitat de files corresponents
        this->setRows( rows );
    }
    // sinó en tenim per aquella llesca, no en podrem treure, per tant es queda igual
}

void Q2DViewer::setPhaseColumns( int slice, int columns )
{
    if( columns > 0 && m_numberOfPhases > 1)
    {
        int *value;
        int renderersCount = 0;
        QMap<int, int*>::iterator mapIterator;
        mapIterator = m_phaseGridMap.find( slice );
        if( mapIterator != m_phaseGridMap.end() )
        {
            value = mapIterator.value();
            renderersCount =  (value[0] * columns) - (value[0]*value[1]);
            value[1] = columns;
        }
        else
        {
            value = new int[2];
            value[0] = 1;
            value[1] = columns;
            renderersCount = columns - 1;
        }
        // afegir renderers per fase
        if( renderersCount > 0 )
        {
            for( int i = 0; i < renderersCount; i++ )
            {
                addRenderScene();
            }
        }
        // eliminar renderers per fase
        else if( renderersCount < 0 )
        {
            for( int i = 0; i > renderersCount; i-- )
            {
                removeRenderScene();
            }
        }
        // actualitzem o insertem un nou
        m_phaseGridMap.insert( slice, value );
        this->updateGrid();
    }
}

void Q2DViewer::removePhaseColumns( int slice, int columns )
{
    QMap<int, int*>::iterator mapIterator;
    mapIterator = m_phaseGridMap.find( slice );
    if( mapIterator != m_phaseGridMap.end() )
    {
        // si ja en tenim, hem de restar els actuals més al valor que donem
        int *value = mapIterator.value();
        if( (value[1] - columns) > 0 )
        {
            columns = value[1] - columns;
        }
        else // si ens passem de llarg hem de deixar una columna com a mínim
        {
            columns = 1;
        }
        // assignem la quantitat de columnes corresponents
        this->setColumns( columns );
    }
    // sinó en tenim per aquella llesca, no en podrem treure, per tant es queda igual
}

void Q2DViewer::addPhaseColumns( int slice, int columns )
{
    if( columns > 0 )
    {
        QMap<int, int*>::iterator mapIterator;
        mapIterator = m_phaseGridMap.find( slice );
        if( mapIterator != m_phaseGridMap.end() )
        {
            // si ja en tenim, hem de sumar els actuals més al valor que donem
            int *value = mapIterator.value();
            columns += columns + value[1];
        }// sinó serà com fer un set tal qual i prou

        this->setColumns( columns );
    }
}

void Q2DViewer::setPhaseGrid( int slice, int rows, int columns )
{
    if( columns > 0 && rows > 0)
    {
        //\TODO això és pot fer de manera més òptima sense recòrrer als mètodes setPhaseRows/Columns
        this->setPhaseRows( slice, rows );
        this->setPhaseColumns( slice, columns );
        this->updateGrid();
    }
}

void Q2DViewer::updateAnnotationsInformation( AnnotationFlags annotation )
{
    if( !m_mainVolume )
        return;

    // Informació que es mostra per cada viewport
    if( annotation & Q2DViewer::WindowInformationAnnotation )
    {
        vtkCornerAnnotation *sliceAnnotation;
        vtkRenderer *renderer;
        int i = 0;
        for ( i = 0; i < m_rendererCollection->GetNumberOfItems(); i++)
        {
            sliceAnnotation = vtkCornerAnnotation::SafeDownCast ( m_sliceAnnotationsCollection->GetItemAsObject ( i ) );
            renderer = vtkRenderer::SafeDownCast ( m_rendererCollection->GetItemAsObject( i ) );

            // informació de la finestra
            if( m_enabledAnnotations & Q2DViewer::WindowInformationAnnotation )
            {
                m_upperLeftText = tr("Image Size: %1 x %2\nView Size: %3 x %4\nWW: %5 WL: %6 ")
                    .arg( m_imageSizeInformation[0] )
                    .arg( m_imageSizeInformation[1] )
                    .arg( renderer->GetSize()[0] )
                    .arg( renderer->GetSize()[1] )
                    .arg( (int)vtkMath::Round( m_windowLevelLUTMapper->GetWindow() ) )
                    .arg( (int)vtkMath::Round( m_windowLevelLUTMapper->GetLevel() ) );
            }
            else
                m_upperLeftText = "";
            sliceAnnotation->SetText( 2 , qPrintable( m_upperLeftText ) );
        }
    }

    if( annotation & Q2DViewer::SliceAnnotation )
        this->updateSliceAnnotationInformation();

    this->refresh();
}

void Q2DViewer::updateSliceAnnotationInformation()
{
    if( !m_mainVolume )
        return;

    int i = 0;
    int rendererIndex = 0;
    int value = m_currentSlice*m_numberOfPhases + m_currentPhase;
    vtkCornerAnnotation *sliceAnnotation = NULL;

    for( i = 0; i < m_numberOfSlicesWindows; i++ )
    {
        QMap<int,int*>::iterator iterator = m_phaseGridMap.find( m_currentSlice + i );
        if( iterator != m_phaseGridMap.end() )
        {
            int *phaseGrid = iterator.value();
            for( int j = 0; j < phaseGrid[0]*phaseGrid[1]; j++  )
            {
                sliceAnnotation = vtkCornerAnnotation::SafeDownCast( m_sliceAnnotationsCollection->GetItemAsObject ( rendererIndex ) );

                if( sliceAnnotation )
                {
                    this->updateSliceAnnotation( sliceAnnotation, ((int)(value/m_numberOfPhases)) + 1, m_maxSliceValue+1, value+1, m_numberOfPhases );
                    if( value >=  m_maxSliceValue )
                        value = 0;
                    else
                        value++;
                }
                rendererIndex++;
            }
            // si no es mostren totes les fases ens ho haurem de saltar
            if( phaseGrid[0]*phaseGrid[1] < m_numberOfPhases )
            {
                value += m_numberOfPhases - phaseGrid[0]*phaseGrid[1];
            }

        }
        else
        {
            sliceAnnotation = vtkCornerAnnotation::SafeDownCast( m_sliceAnnotationsCollection->GetItemAsObject ( rendererIndex ) );

            if( sliceAnnotation )
            {
                if ( m_numberOfPhases > 1)
                {
                    this->updateSliceAnnotation( sliceAnnotation, (value/m_numberOfPhases) + 1, m_maxSliceValue + 1, m_currentPhase + 1, m_numberOfPhases );
                }
                else
                {
                    this->updateSliceAnnotation( sliceAnnotation, value+1, m_maxSliceValue+1 );
                }
                if( value >= m_maxSliceValue )
                    value = 0;
                else
                    value += m_numberOfPhases;
            }
            rendererIndex++;
        }
    }
    this->refresh();
}

void Q2DViewer::updatePatientAnnotationInformation()
{
    if( m_mainVolume )
    {
        // informació fixa
        m_upperRightText = tr("%1")
            .arg( m_mainVolume->getPatient()->getFullName() );

        m_upperRightText += tr("\n%1\n%2\n%3\nAcc:\n%4\n%5")
                    .arg( m_mainVolume->getSeries()->getInstitutionName() )
                    .arg( m_mainVolume->getPatient()->getID() )
                    .arg( m_mainVolume->getStudy()->getAccessionNumber() )
                    .arg( m_mainVolume->getStudy()->getDateAsString() )
                    .arg( m_mainVolume->getStudy()->getTimeAsString() );

        m_lowerRightText = tr("%1")
            .arg( m_mainVolume->getSeries()->getProtocolName() );

        m_serieInformationAnnotation->SetText( 3, qPrintable( m_upperRightText ) );
        m_serieInformationAnnotation->SetText( 1, qPrintable( m_lowerRightText ) );
        this->refresh();
    }
    else
    {
        DEBUG_LOG("No hi ha un volum vàlid. No es poden inicialitzar les annotacions de texte");
    }

}

void Q2DViewer::updatePatientOrientationAnnotationInformation()
{
    // TODO per implementar correctament
}

void Q2DViewer::addRenderScene()
{
    vtkRenderWindow* renderWindow = m_viewer->GetRenderWindow();
    vtkRenderer *renderer;
    vtkCamera *camera;
    vtkImageActor *actor;
    vtkCornerAnnotation *sliceAnnotation;
    vtkCornerAnnotation *serieInformationAnnotation;
    vtkAxisActor2D *sideRuler;
    vtkAxisActor2D *bottomRuler;
    vtkCoordinate *anchoredRulerCoordinates;
    vtkScalarBarActor *scalarBar;

    vtkCamera *referenceCamera = this->getRenderer()->GetActiveCamera();
    double position[3], focalPoint[3], viewUp[3];

    referenceCamera->GetPosition(position);
    referenceCamera->GetFocalPoint(focalPoint);
    referenceCamera->GetViewUp(viewUp);

    // li donem una càmera
    camera = vtkCamera::New();
    camera->SetPosition(position);
    camera->SetFocalPoint(focalPoint);
    camera->SetViewUp(viewUp);

    renderer = vtkRenderer::New();
    renderWindow->AddRenderer( renderer );
    renderer->SetActiveCamera( camera );

    // li creem el corresponent image actor
    actor = vtkImageActor::New();
    actor->SetInput( m_viewer->GetWindowLevel()->GetOutput() );
    renderer->AddActor( actor );
    m_sliceActorCollection->AddItem( actor );

    // li afegim les annotacions de texte corresponents
    sliceAnnotation = vtkCornerAnnotation::New();
    renderer->AddActor2D( sliceAnnotation );
    m_sliceAnnotationsCollection->AddItem( sliceAnnotation );

    // li afegim les annotacions fixes
//     renderer->AddActor2D( m_serieInformationAnnotation );
    // afegim una nova anotació per cada llesca perquè al mostrar sigui
    // d'acord amb el grid escollit
    serieInformationAnnotation = vtkCornerAnnotation::New();
    serieInformationAnnotation->SetText( 1,m_serieInformationAnnotation->GetText( 1 ) );
    serieInformationAnnotation->SetText( 3,m_serieInformationAnnotation->GetText( 3 ) );
    renderer->AddActor2D( serieInformationAnnotation );
    m_informationCollection->AddItem( serieInformationAnnotation );

    // afegim les annotacions d'orientació de pacient (per cada llesca)
    for( int j = 0; j < 4; j++ )
        renderer->AddActor2D( m_patientOrientationTextActor[j] );

    // li afegim els rulers
    sideRuler = this->createRuler();
    bottomRuler = this->createRuler();
    m_rulerActorCollection->AddItem( sideRuler );
    m_rulerActorCollection->AddItem( bottomRuler );
    renderer->AddActor2D( sideRuler );
    renderer->AddActor2D( bottomRuler );

    // Afegim a cada viewport el mateix scalar bar.\TODO tenir en compte que podria ser diferent, ara mateix afegim sempre el mateix actor
//     renderer->AddActor2D( m_scalarBar );

    // afegim l'scalarBar a cada llesca per tal que tingui el tamany corresponent amb el grid assignat.
    scalarBar = createScalarBar();
    renderer->AddActor2D( scalarBar );
    m_scalarBarCollection->AddItem( scalarBar );


    // coordenades fixes per ancorar els rulers al lateral i a la part inferior
    anchoredRulerCoordinates = vtkCoordinate::New();
    anchoredRulerCoordinates->SetCoordinateSystemToView();
    anchoredRulerCoordinates->SetValue( -0.95 , -0.9 , -0.95 );
    m_anchoredRulerCoordinatesCollection->AddItem( anchoredRulerCoordinates );

    // afegim el renderer a la col·lecció
    m_rendererCollection->AddItem(renderer);
    renderer->ResetCameraClippingRange();
}

void Q2DViewer::removeRenderScene()
{
    vtkRenderer *renderer;
    renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( m_rendererCollection->GetNumberOfItems()-1 ) );

    renderer->RemoveAllViewProps();
    m_viewer->GetRenderWindow()->RemoveRenderer( renderer );
    m_sliceActorCollection->RemoveItem( m_sliceActorCollection->GetLastProp() );
    m_rendererCollection->RemoveItem( renderer );
    m_sliceAnnotationsCollection->RemoveItem( m_sliceAnnotationsCollection->GetLastActor2D());
    m_informationCollection->RemoveItem( m_informationCollection->GetLastActor2D() );
    m_scalarBarCollection->RemoveItem( m_scalarBarCollection->GetLastActor2D() );
    m_rulerActorCollection->RemoveItem( m_rulerActorCollection->GetLastActor2D() );
    m_rulerActorCollection->RemoveItem( m_rulerActorCollection->GetLastActor2D() );

    renderer->Render();
    renderer->Delete();
}

void Q2DViewer::updateGrid()
{
    int i = 0;

    // Comprovar si s'han d'afegir o treure amb els rows/columns actuals
    if( m_numberOfSlicesWindows < m_columns*m_rows )
    {
        // afegim nous viewports
        i = m_numberOfSlicesWindows;
        while( i < (m_columns*m_rows) && i < m_maxSliceValue )
        {
            addRenderScene();
            i++;
        }
    }
    else if( (m_numberOfSlicesWindows > m_columns*m_rows || m_numberOfSlicesWindows > m_maxSliceValue ) && m_numberOfSlicesWindows > 1 )
    {
        i = m_numberOfSlicesWindows;
        while( i > (m_columns*m_rows) || i > m_maxSliceValue )
        {
            removeRenderScene();
            i--;
        }
    }

    m_numberOfSlicesWindows = m_columns*m_rows;

    if( m_viewer->GetInput() )
    {
        this->setSlice( m_currentSlice );
    }
    else
        updateViewports(); // Redistribuir viewports TODO perquè si no tinc input he de fer un updateViewports?
    m_viewer->Render();
}

void Q2DViewer::updateViewports()
{
    int i,j;
    double xmin = m_slicesViewportExtent[0];
    double ymin = m_slicesViewportExtent[1];
    double xmax = m_slicesViewportExtent[2];
    double ymax = m_slicesViewportExtent[3];
    double sizeHorizontal = (m_slicesViewportExtent[2]-m_slicesViewportExtent[0])/m_columns;
    double sizeVertical = (m_slicesViewportExtent[3]-m_slicesViewportExtent[1])/m_rows;
    double sizePhaseHorizontal;
    double sizePhaseVertical;
    double xPhaseMin, yPhaseMin;

    vtkRenderer* renderer;

    int rendererIndex = 0;
    int slice = m_currentSlice;

    for ( i = 0; i < m_rows; i++ )
    {
        ymin = ymax-sizeVertical;
        j=0;

        while (j < m_columns && ((i*m_columns + j)< m_maxSliceValue ) )
        {
            QMap<int,int*>::iterator iterator = m_phaseGridMap.find( slice );
            if( iterator != m_phaseGridMap.end() )
            {
                int *value = iterator.value();
                sizePhaseHorizontal = sizeHorizontal / value[1];
                sizePhaseVertical = sizeVertical / value[0];

                yPhaseMin = ymax - sizePhaseVertical;
                for( int k = 0; k < value[0]; k++ )
                {
                    xPhaseMin = xmin;
                    for( int l = 0; l < value[1]; l++ )
                    {
                        renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( rendererIndex ) );
                        renderer->SetViewport( xPhaseMin, yPhaseMin, xPhaseMin+sizePhaseHorizontal, yPhaseMin+sizePhaseVertical );
                        renderer->ResetCameraClippingRange();
                        xPhaseMin += sizePhaseHorizontal;
                        rendererIndex++;
                    }
                    yPhaseMin -= sizePhaseVertical;
                }
            }
            else
            {
                xmax = xmin+sizeHorizontal;
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( rendererIndex ) );
                renderer->SetViewport( xmin,ymin,xmax,ymax );
                renderer->ResetCameraClippingRange();
                rendererIndex++;
            }
            xmin += sizeHorizontal;
            j++;

            if( slice == m_maxSliceValue ) slice = 0;
            else slice += 1;
        }
        xmin = m_slicesViewportExtent[0];
        ymax = ymax - sizeVertical;
    }
}

void Q2DViewer::updateDisplayExtent()
{
    vtkImageData *input = m_viewer->GetInput();
    if( !input )
        return;

    // thick slab
    int sliceValue;
    if( isThickSlabActive() )
    {
        // en comptes de currentSlice podria ser m_firstSlabSlice, que és equivalent
        sliceValue = m_currentSlice; // Podria ser 0, dependent de l'extent de sortida del filtre
    }
    else
        sliceValue = m_currentSlice*m_numberOfPhases + m_currentPhase;

    int i = 0;
    int rendererIndex = 0;
    vtkRenderer *renderer;
    vtkImageActor *imageActor;
    vtkCornerAnnotation *sliceAnnotation;
    int *wholeExtent = input->GetWholeExtent();

    for( i = 0; i < m_numberOfSlicesWindows; i++ )
    {
        QMap<int,int*>::iterator iterator = m_phaseGridMap.find( m_currentSlice + i );
        if( iterator != m_phaseGridMap.end() )
        {
            int *phaseGrid = iterator.value();
            for( int j = 0; j < phaseGrid[0]*phaseGrid[1]; j++  )
            {
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( rendererIndex ) );
                imageActor = vtkImageActor::SafeDownCast( m_sliceActorCollection->GetItemAsObject( rendererIndex ) );
                sliceAnnotation = vtkCornerAnnotation::SafeDownCast( m_sliceAnnotationsCollection->GetItemAsObject ( rendererIndex ) );

                if( imageActor )
                {
                    switch( m_lastView )
                    {
                        case Axial:
                            imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], wholeExtent[2], wholeExtent[3], sliceValue, sliceValue );
                            break;

                        case Coronal:
                            imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], sliceValue, sliceValue, wholeExtent[4], wholeExtent[5] );
                            break;

                        case Sagital:
                            imageActor->SetDisplayExtent( sliceValue, sliceValue, wholeExtent[2], wholeExtent[3], wholeExtent[4], wholeExtent[5] );
                            break;
                    }
                    // TODO com que per aquí no es passa pràcticament mai, no ho hem tocat pas
                    this->updateSliceAnnotation( sliceAnnotation, ((int)(sliceValue/m_numberOfPhases)) + 1, m_maxSliceValue+1, sliceValue+1, m_numberOfPhases );
                    if( sliceValue >=  m_maxSliceValue )
                        sliceValue = 0;
                    else
                        sliceValue++;
                }
                rendererIndex++;
            }
            // si no es mostren totes les fases ens ho haurem de saltar
            if( phaseGrid[0]*phaseGrid[1] < m_numberOfPhases )
            {
                sliceValue += m_numberOfPhases - phaseGrid[0]*phaseGrid[1];
            }
        }
        else
        {
            renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( rendererIndex ) );
            imageActor = vtkImageActor::SafeDownCast( m_sliceActorCollection->GetItemAsObject( rendererIndex ) );
            sliceAnnotation = vtkCornerAnnotation::SafeDownCast( m_sliceAnnotationsCollection->GetItemAsObject ( rendererIndex ) );

            if( imageActor )
            {
                switch( m_lastView )
                {
                    case Axial:
                        imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], wholeExtent[2], wholeExtent[3], sliceValue, sliceValue );
                        break;

                    case Coronal:
                        imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], sliceValue, sliceValue, wholeExtent[4], wholeExtent[5] );
                        break;

                    case Sagital:
                        imageActor->SetDisplayExtent( sliceValue, sliceValue, wholeExtent[2], wholeExtent[3], wholeExtent[4], wholeExtent[5] );
                        break;
                }
                // TODO provar si cal fer això amb "value" o si amb sliceValue ja és correcte
                int value = m_currentSlice*m_numberOfPhases + m_currentPhase;
                if ( m_numberOfPhases > 1)
                {
                    this->updateSliceAnnotation( sliceAnnotation, (value/m_numberOfPhases) + 1, m_maxSliceValue + 1, m_currentPhase + 1, m_numberOfPhases );
                }
                else
                {
                    this->updateSliceAnnotation( sliceAnnotation, value+1, m_maxSliceValue+1 );
                }
//                 if ( m_numberOfPhases > 1)
//                 {
//                     this->updateSliceAnnotation( sliceAnnotation, (sliceValue/m_numberOfPhases) + 1, m_maxSliceValue + 1, m_currentPhase + 1, m_numberOfPhases );
//                 }
//                 else
//                 {
//                     this->updateSliceAnnotation( sliceAnnotation, sliceValue+1, m_maxSliceValue+1 );
//                 }
//
//                 if( sliceValue >=  m_maxSliceValue )
//                     sliceValue = 0;
//                 else
//                     sliceValue += m_numberOfPhases;
            }
            rendererIndex++;
        }
        renderer->ResetCameraClippingRange();
    }
}

void Q2DViewer::enableAnnotation( AnnotationFlags annotation, bool enable )
{
    if( enable )
        m_enabledAnnotations = m_enabledAnnotations | annotation;
    else
        m_enabledAnnotations =  m_enabledAnnotations & ~annotation ;

    refreshAnnotations();
}

void Q2DViewer::removeAnnotation( AnnotationFlags annotation )
{
    enableAnnotation( annotation, false );
}

void Q2DViewer::computeInputGrayscalePipeline()
{
    if( m_mainVolume )
    {
        // si llegim l'arxiu tal qual, la modality no cal aplicar-la perquè les pròpies gdcm ja ens apliquen la modality lut
        computeModalityLUT();
        computeVOILUT();
    }
}

void Q2DViewer::applyGrayscalePipeline()
{
    DEBUG_LOG( "*** Grayscale Transform Pipeline Begin ***" );
    DEBUG_LOG( QString("Image Information: Bits Allocated: %1, Bits Stored: %2, Pixel Range %3 to %4, SIGNED?Pixel Representation: %5, Photometric interpretation: %6")
    .arg( m_mainVolume->getImages()[m_currentSlice]->getBitsAllocated() )
    .arg( m_mainVolume->getImages()[m_currentSlice]->getBitsStored() )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[0] )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[1] )
    .arg( m_mainVolume->getImages().at(0)->getPixelRepresentation() )
    .arg( m_mainVolume->getImages().at(0)->getPhotometricInterpretation() )
                     );
//\TODO Això s'ha d'aplicar enfunció de si tenim presentationm state o no? mirar si s'ha de fer aquí o al presentation state attacher...

//     if( m_mainVolume->getImages().at(0)->getPhotometricInterpretation() =="MONOCHROME1" && m_presentationStateFilename != NULL )
//     {
//         DEBUG_LOG("La imatge és MONOCHROME1: ¿invertim les dades després de la VOI LUT i abans de la presentation LUT?... Si hi ha presentation state això no s'hauria de fer!");
//         if( m_presentationLut )
//         {
//             m_presentationLut->InverseVideoOn();
//             DEBUG_LOG("Inverteixo la PRESENTATION LUT");
//         }
//         else if( m_windowLevelLut )
//         {
//             m_windowLevelLut->InverseVideoOn();
//             DEBUG_LOG("Inverteixo la VOI LUT");
//         }
//         else if( m_modalityLut )
//         {
//             m_modalityLut->InverseVideoOn();
//             DEBUG_LOG("Inverteixo la MODALITY LUT");
//         }
//     }
    // crear el pipeline en funció de tot el que ens hem trobat
    if( m_modalityLut )
    {
        if( m_windowLevelLut )
        {
            if( m_presentationLut ) // modality lut + windowlevel lut + presentation lut
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> Window Level LUT -> Presentation LUT -> Output  :: FIRST TRY OF IMPLEMENTATION!");
                m_viewer->SetInput( m_mainVolume->getVtkData() );
                m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                m_windowLevelLUTMapper->SetLookupTable( m_presentationLut );
                m_viewer->GetImageActor()->SetInput( m_windowLevelLUTMapper->GetOutput() );
                // es dóna per fet que els paràmetres correctes de window level ja estan calculats, ja sigui per especificació explícita o per assignació automàtica
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
            }
            else // modality lut + windowlevel lut
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> Window Level LUT -> Output  :: NOT IMPLEMENTED YET!");
            }
        }
        else
        {
            if( m_presentationLut ) // modality lut + presentation lut [ + ww/wl ]
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> [Window Level] -> Presentation LUT -> Output  :: NOT IMPLEMENTED YET!");
            }
            else // modality [ + ww/wl ]
            {
                DEBUG_LOG("Grayscale pipeline: Source Data -> Modality LUT -> [Window Level] -> Output ");
                m_viewer->SetInput( m_mainVolume->getVtkData() );
                m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                m_windowLevelLUTMapper->SetLookupTable( m_modalityLut );
                m_viewer->GetImageActor()->SetInput( m_windowLevelLUTMapper->GetOutput() );
                // es dóna per fet que els paràmetres correctes de window level ja estan calculats, ja sigui per especificació explícita o per assignació automàtica
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
            }
        }
    }
    else
    {
        if( m_windowLevelLut )
        {
            if( m_presentationLut )
            {
                if( m_modalityLUTRescale ) // rescale slope + windowlevel lut + presentation lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> Window Level LUT -> Presentation LUT -> Output  :: NOT IMPLEMENTED YET!");
                }
                else // windowlevel lut + presentation lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> Window Level LUT -> Presentation LUT -> Output :: NOT IMPLEMENTED YET!");
                }
            }
            else // windowlevel lut
            {
                if( m_modalityLUTRescale ) // rescale slope + windowlevel lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> Window Level LUT -> Output ");
                    m_viewer->SetInput( m_modalityLUTRescale->GetOutput() );
                    m_windowLevelLUTMapper->SetInput( m_modalityLUTRescale->GetOutput() );
                }
                else // windowlevel lut
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> Window Level LUT -> Output ");
                    m_viewer->SetInput( m_mainVolume->getVtkData() );
                    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                }
                m_windowLevelLUTMapper->SetLookupTable( m_windowLevelLut );
                m_viewer->GetImageActor()->SetInput( m_windowLevelLUTMapper->GetOutput() );
                // es dóna per fet que el window level queda ajustat per defecte
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
            }
        }
        else
        {
            if( m_presentationLut )
            {
                if( m_modalityLUTRescale ) // rescale slope + [ww/wl +] presentation
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> [Window Level] -> Presentation LUT -> Output ");
                    m_modalityLUTRescale->SetInput( m_mainVolume->getVtkData() );
                    m_viewer->SetInput( m_modalityLUTRescale->GetOutput() );
                    m_windowLevelLUTMapper->SetInput( m_modalityLUTRescale->GetOutput() );
                }
                else // [ww/wl +] presentation
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> [Window Level] -> Presentation LUT -> Output ");
                    m_viewer->SetInput( m_mainVolume->getVtkData() );
                    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                }
                m_windowLevelLUTMapper->SetLookupTable( m_presentationLut );
                // es dóna per fet que els paràmetres correctes de window level ja estan calculats, ja sigui per especificació explícita o per assignació automàtica
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
                m_viewer->GetImageActor()->SetInput( m_windowLevelLUTMapper->GetOutput() );
            }
            else
            {
                if( m_modalityLUTRescale ) // rescale slope
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> RescaleSlope -> [Window Level] -> Output ");
//                     m_modalityLUTRescale->SetInput( m_mainVolume->getVtkData() );
                    m_viewer->SetInput( m_modalityLUTRescale->GetOutput() );
                    m_windowLevelLUTMapper->SetInput( m_modalityLUTRescale->GetOutput() );
                }
                else // res
                {
                    DEBUG_LOG("Grayscale pipeline: Source Data -> [Window Level] -> Output ");
                    m_viewer->SetInput( m_mainVolume->getVtkData() );
                    m_windowLevelLUTMapper->SetInput( m_mainVolume->getVtkData() );
                }
                m_windowLevelLUTMapper->SetWindow( m_defaultWindow );
                m_windowLevelLUTMapper->SetLevel( m_defaultLevel );
                m_viewer->GetImageActor()->SetInput( m_windowLevelLUTMapper->GetOutput() );
            }
        }
    }
}

void Q2DViewer::setSlabProjectionMode( int projectionMode )
{
    m_slabProjectionMode = projectionMode;
    m_thickSlabProjectionFilter->SetAccumulatorType( static_cast<AccumulatorFactory::AccumulatorType>( m_slabProjectionMode ) );
    updateDisplayExtent();
    this->refresh();
}

int Q2DViewer::getSlabProjectionMode() const
{
    return m_slabProjectionMode;
}

void Q2DViewer::setSlabThickness( int thickness )
{
    //primera aproximació per evitar error dades de primitives: a l'activar o desactivar l'slabthickness, esborrem primitives
    if ( thickness != m_slabThickness )
        this->getDrawer()->removeAllPrimitives();

    computeRangeAndSlice( thickness );
    // TODO comprovar aquest pipeline si és millor calcular ara o més tard
    if( m_slabThickness == 1  && isThickSlabActive() )
    {
        DEBUG_LOG( "desconnectar" );
        setupDefaultPipeline();
        m_thickSlabActive = false;
        updateDisplayExtent();
        this->refresh();
    }
    if ( m_slabThickness > 1 && !isThickSlabActive() ) // la comprovacio es per constuir el pipeline nomes el primer cop
    {
        DEBUG_LOG( "connectar" );
        setupThickSlabPipeline();
        m_thickSlabActive = true;
    }

    m_lastSlabSlice = m_currentSlice + m_slabThickness - 1;

    if( isThickSlabActive() )
    {
        m_thickSlabProjectionFilter->SetFirstSlice( m_firstSlabSlice );
        m_thickSlabProjectionFilter->SetNumberOfSlicesToProject( m_slabThickness );
        updateDisplayExtent();
        this->refresh();
    }

    emit slabThicknessChanged( m_slabThickness );
}

int Q2DViewer::getSlabThickness() const
{
    return m_slabThickness;
}

void Q2DViewer::enableThickSlab( bool enable )
{
    if(!enable)
        setSlabThickness(1);
    else
        setSlabThickness( m_slabThickness );
}

bool Q2DViewer::isThickSlabActive() const
{
    return m_thickSlabActive;
}

void Q2DViewer::computeModalityLUT()
{
    // If the Modality LUT or equivalent Attributes are part of both the Image and the Presentation State, then the Presentation State Modality LUT shall be used instead of the Image Modality LUT or equivalent Attributes in the Image. If the Modality LUT is not present in the Presentation State it shall be assumed to be an identity transformation. Any Modality LUT or equivalent Attributes in the Image shall not be used.

    // busquem si existeix una modality lut TODO eliminem el parseLookupTable, això hauria de ser feina de
    // la classe que apliqui les transformacions de grisos i  presentation states
    m_modalityLut = NULL;
    if( m_modalityLut )
    {
        m_modalityRange[0] = m_modalityLut->GetTableRange()[0];
        m_modalityRange[1] = m_modalityLut->GetTableRange()[1];
        if( m_modalityLUTRescale )
        {
            m_modalityLUTRescale->Delete();
            m_modalityLUTRescale = 0;
        }
    }
    // si no hi ha lut busquem els paràmetres de rescale
    else
    {
        // mirar el de la imatge, només per curiositat perquè les itk ja l'apliquen directament
        DEBUG_LOG( QString("Image Modality LUT Adjustment: Rescale Slope %1, Rescale Intercept %2")
        .arg( m_mainVolume->getImages().at(0)->getRescaleSlope() )
        .arg( m_mainVolume->getImages().at(0)->getRescaleIntercept() )
         );
        if( m_modalityLUTRescale )
        {
            m_modalityLUTRescale->Delete();
            m_modalityLUTRescale = 0;
        }
        m_modalityRange[0] = m_mainVolume->getVtkData()->GetScalarRange()[0];
        m_modalityRange[1] = m_mainVolume->getVtkData()->GetScalarRange()[1];
    }
}

void Q2DViewer::computeVOILUT()
{
    //
    // 3. VOI LUT
    //
    // If a VOI LUT is part of both the Image and the Presentation State then the Presentation State VOI LUT shall be used instead of the Image VOI LUT. If a VOI LUT (that applies to the Image) is not present in the Presentation State , it shall be assumed to be an identity transformation. Any VOI LUT or equivalent values in the Image shall not be used.

    // aquests canvis es poden aplicar a un subconjunt de imatges/frames. Per tant podem tenir diverses VOI LUT per una mateixa sèrie que s'apliquen a diverses imatges.

    // primer busquem voi lut, sinó busquem window level TODO eliminem el parseLookupTable, això hauria de ser feina de
    // la classe que apliqui les transformacions de grisos i  presentation states
    m_windowLevelLut = NULL;
    // si hi ha una VOI LUT
    if( m_windowLevelLut )
    {
        // Ara podem considerar els tres casos anteriors de la modality: hi ha rescale, hi ha lut o no hi ha res. el 1r i el tercer es tracten igual.
        if( m_modalityLut )
        {
            DEBUG_LOG("Encara no sabem què fer en aquest cas (concatenar modality + voi LUT)");
        }
        else
        {
            m_defaultWindow = m_windowLevelLut->GetTableRange()[1] - m_windowLevelLut->GetTableRange()[0];
            m_defaultLevel = m_defaultWindow / 2.0;
        }
    }
    else
    {
        // només mirem el del nostre propi volum

        if( m_mainVolume->getImages().at(0)->getNumberOfWindowLevels() > 0 )
        {
            // Encara que en tingui més d'un window level, agafarem el primer i prou. Si n'hi ha més s'escolliran desde l'extensió adequada
            m_defaultWindow = m_mainVolume->getImages().at(0)->getWindowLevel().first;
            m_defaultLevel = m_mainVolume->getImages().at(0)->getWindowLevel().second;
            if( m_defaultWindow == 0.0 && m_defaultLevel == 0.0 )
            {
                double *range = m_mainVolume->getVtkData()->GetScalarRange();
                m_defaultWindow = range[1] - range[0];
                m_defaultLevel = (m_defaultWindow / 2.) + range[0];
            }
            DEBUG_LOG( QString("Image VOI Adjustment: Window: %1, Level: %2")
                .arg( m_defaultWindow )
                .arg( m_defaultLevel )
                );
        }
        else
        {
            // ajustar un al rang de dades adequat
            m_defaultWindow = fabs( m_modalityRange[1] - m_modalityRange[0] );
            m_defaultLevel = ( m_modalityRange[1] + m_modalityRange[0] )/ 2.0;
            DEBUG_LOG( QString("No Image VOI Adjustment, creating a nice and automatic one: Window: %1, Level: %2")
            .arg( m_defaultWindow )
            .arg( m_defaultLevel )
            );
        }
    }
}

void Q2DViewer::computeRangeAndSlice( int newSlabThickness )
{
    // checking del nou valor
    if( newSlabThickness < 1 )
    {
        DEBUG_LOG(" valor invàlid de thickness. Ha de ser >= 1 !!!!!");
        return;
    }
    if( newSlabThickness == m_slabThickness )
    {
        DEBUG_LOG(" tenim el mateix slab thickness, no canviem res ");
        return;
    }
    if( newSlabThickness > m_maxSliceValue + 1 )
    {
        DEBUG_LOG(" el nou thickness supera el thickness màxim, tot queda igual ");
        // TODO podríem aplicar newSlabThickness=m_maxSliceValue+1
        return;
    }

    int difference = newSlabThickness - m_slabThickness;
    // si la diferència és positiva, augmentem el thickness
    if( difference > 0 )
    {
        m_firstSlabSlice -= difference / 2; // divisió entera!
        m_lastSlabSlice += difference / 2;

        // si la diferència és senar creix més per un dels límits
        if( (difference % 2) != 0 )
        {
            // si el thickness actual és parell creixem per sota
            if( (m_slabThickness % 2) == 0 )
                m_firstSlabSlice--;
            else // sinó creixem per dalt
                m_lastSlabSlice++;
        }
        //check per si ens passem de rang superior o inferior
        if( m_firstSlabSlice < this->getMinimumSlice() )
        {
            // si ens passem per sota, cal compensar creixent per dalt
            m_lastSlabSlice += this->getMinimumSlice() - m_firstSlabSlice;
            m_firstSlabSlice = this->getMinimumSlice(); // queda al límit inferior
        }
        else if( m_lastSlabSlice > m_maxSliceValue )
        {
            // si ens passem per dalt, cal compensar creixent per sota
            m_firstSlabSlice -= m_lastSlabSlice - m_maxSliceValue;
            m_lastSlabSlice = m_maxSliceValue;
        }
    }
    else // la diferència és negativa, decreix el thickness
    {
        // la convertim a positiva per conveniència
        difference *= -1;
        m_firstSlabSlice += difference / 2;
        m_lastSlabSlice -= difference / 2;

        // si la diferència és senar decreix més per un dels límits
        if( (difference % 2) != 0 )
        {
            // si el thickness actual és parell decreixem per amunt
            if( (m_slabThickness%2) == 0 )
                m_lastSlabSlice--;
            else // sinó decreixem per avall
                m_firstSlabSlice++;
        }
    }
    // actualitzem el thickness
    m_slabThickness = newSlabThickness;
    // actualitzem la llesca
    m_currentSlice = m_firstSlabSlice;
}

double *Q2DViewer::pointInModel( int screen_x, int screen_y )
{
    double *bounds = m_viewer->GetImageActor()->GetBounds();
    double position[4];
    computeDisplayToWorld( getRenderer(), screen_x, screen_y, 0, position );
    double *lastPointInModel = new double[3];

    lastPointInModel[0] = position[0];
    lastPointInModel[1] = position[1];
    lastPointInModel[2] = position[2];
    //Cas axial
    switch( m_lastView )
    {
        case Axial:

            if( bounds[0] < position[0] && bounds[1] > position[0] ) // La x cau dins del model
            {
                lastPointInModel[0] = position[0];
            }
            else {// La x cau fora del model
                if( bounds[0] > position[0] )
                    lastPointInModel[0] = bounds[0];
                else lastPointInModel[0] = bounds[1];
            }

            if( bounds[2] < position[1] && bounds[3] > position[1] ) // La y cau dins del model
            {
                lastPointInModel[1] = position[1];
            }
            else{
                if( bounds[2] > position[1] )
                    lastPointInModel[1] = bounds[2];
                else lastPointInModel[1] = bounds[3];
            }
            break;

        case Coronal:

            if( bounds[0] < position[0] && bounds[1] > position[0] )
            {
                lastPointInModel[0] = position[0];
            }
            else{
                if( bounds[0] > position[0] )
                    lastPointInModel[0] = bounds[0];
                else lastPointInModel[0] = bounds[1];
            }

            if( bounds[4] < position[2] && bounds[5] > position[2] )
            {
                lastPointInModel[2] = position[2];
            }
            else{
                if( bounds[4] > position[2] )
                    lastPointInModel[2] = bounds[4];
                else lastPointInModel[2] = bounds[5];
            }
            break;

        case Sagital:

            if( bounds[4] < position[2] && bounds[5] > position[2] )
            {
                lastPointInModel[2] = position[2];
            }
            else{
                if( bounds[4] > position[2] )
                    lastPointInModel[2] = bounds[4];
                else lastPointInModel[2] = bounds[5];
            }

            if( bounds[2] < position[1] && bounds[3] > position[1] )
            {
                lastPointInModel[1] = position[1];
            }
            else{
                if( bounds[2] > position[1] )
                    lastPointInModel[1] = bounds[2];
                else lastPointInModel[1] = bounds[3];
            }
            break;
    }
    return lastPointInModel;
}

vtkImageData* Q2DViewer::getCurrentSlabProjection()
{
    return m_thickSlabProjectionFilter->GetOutput();
}

int Q2DViewer::getNearestSlice( double projectedPosition[3], double * distance )
{
    int i;
    double actualDistance;
    double minimumDistance = -1.0;
    int minimumSlice = -1;
    double aux;
    double mod;
    double currentPlaneRowVector[3], currentPlaneColumnVector[3], currentPlaneOrigin[3], currentPerpendicularVector[3], currentNormalVector[3];
    ImagePlane *currentPlane;
    int maxSlice = this->getMaximumSlice();
    
    for( i = 0; i < maxSlice ; i++ )
    {
        currentPlane = this->getImagePlane( i, m_currentPhase );
        if( currentPlane )
        {
            currentPlane->getOrigin( currentPlaneOrigin );
            currentPlane->getNormalVector( currentNormalVector );

//             DEBUG_LOG( tr("Origen: [%1,%2,%3]").arg( currentPlaneOrigin[0] ).arg( currentPlaneOrigin[1] ).arg( currentPlaneOrigin[2] ) );
            
            actualDistance = vtkPlane::DistanceToPlane ( projectedPosition, currentNormalVector, currentPlaneOrigin );

//             DEBUG_LOG( tr("Distància a la llesca %1: %2 amb Origen: [%3,%4,%5] al punt: [%6,%7,%8] vector:[%9,%10,%11]").arg( i ).arg( actualDistance ).arg(currentPlaneOrigin[0]).arg(currentPlaneOrigin[1]).arg(currentPlaneOrigin[2]).arg(projectedPosition[0]).arg(projectedPosition[1]).arg(projectedPosition[2]).arg(currentNormalVector[0]).arg(currentNormalVector[1]).arg(currentNormalVector[2]) );
            
            if( ( actualDistance < minimumDistance ) || ( minimumDistance == -1.0 ))
            {
                minimumDistance = actualDistance;
                minimumSlice = i;
            }
        }
    }
    
    
//     DEBUG_LOG( tr("Em quedo amb la llesca: %1").arg( minimumSlice ) );
    return minimumSlice;
}

};  // end namespace udg

