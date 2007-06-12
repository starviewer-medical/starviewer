/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "q2dviewer.h"
#include "volume.h"
#include "volumesourceinformation.h"
#include "logging.h"

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
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToWindowLevelColors.h>
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
#include <vtkCaptionActor2D.h>
#include <vtkPropPicker.h>

#include <vtkPropCollection.h>
#include <vtkRendererCollection.h>
#include <vtkCollection.h>
#include <vtkActor2DCollection.h>
#include <vtkImageActor.h>

// displayed area
#include <vtkImageChangeInformation.h>
#include <vtkImageResample.h>

// grayscale pipeline
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageShiftScale.h>
#include <vtkWindowLevelLookupTable.h>

// dcmtk:
//\TODO hem de fer aquest define perquè sinó no compila. Caldria descobrir perquè passa això i si cal fer un altre include previ
#define HAVE_CONFIG_H 1
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h" // DcmTagKey
#include "dcmtk/dcmdata/dcsequen.h" // DcmSequenceOfItems

namespace udg {

Q2DViewer::Q2DViewer( QWidget *parent )
 : QViewer( parent ), m_serieInformationAnnotation(0), m_numberOfPhases(1), m_maxSliceValue(0), m_currentPhase(0), m_numberOfSlicesWindows(1), m_currentSlice(0), m_overlayVolume(0), m_rotateFactor(0), m_voxelInformationCaption(0), m_applyFlip(false), m_isImageFlipped(false), m_scalarBar(0), m_sideRuler(0), m_bottomRuler(0), m_defaultWindow(.0), m_defaultLevel(.0), m_toolManager(0), m_modalityLut(0), m_windowLevelLut(0), m_presentationLut(0)
{
    m_enabledAnnotations = Q2DViewer::AllAnnotation;
    m_lastView = Q2DViewer::Axial;
    m_currentSlice = 0;
    m_imageSizeInformation[0] = 0;
    m_imageSizeInformation[1] = 0;
    m_overlay = CheckerBoard; // per defecte
    updateCursor( -1, -1, -1, -1 );
    m_rotateFactor = 0; // per defecte no hi ha cap rotació adicional

    // inicialitzacions d'objectes
    // visor
    m_viewer = vtkImageViewer2::New();
    // preparem el picker
    m_picker = vtkPropPicker::New();
    // preparem el picker
    m_blender = 0;

    m_overlayVolume = 0;
    m_voxelInformationCaption = 0;
    m_scalarBar = 0;
    m_toolManager = 0;
    for( int i = 0; i < 4; i++ )
    {
        m_patientOrientationTextActor[i] = 0;
    }
    m_sideRuler = 0;
    m_bottomRuler = 0;

    // CheckerBoard
    // el nombre de divisions per defecte, serà de 2, per simplificar
    m_divisions[0] = m_divisions[1] = m_divisions[2] = 2;

    setupInteraction();
    m_toolManager = new Q2DViewerToolManager( this );
    this->enableTools();

    // anotacions
    createAnnotations();
    createActions();
    addActors();

    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );
    disableVoxelInformationCaption();

    // grayscale pipeline
    m_windowLevelLUTMapper = vtkImageMapToWindowLevelColors::New();
    m_windowLevelLUTMapper->SetOutputFormatToLuminance();

    m_modalityLUTRescale = 0; //vtkImageShiftScale::New();

    // inicialització viewport de les llesques
    m_slicesViewportExtent[0]=.0;
    m_slicesViewportExtent[1]=.0;
    m_slicesViewportExtent[2]=1.;
    m_slicesViewportExtent[3]=1.;

    m_rows = 1;
    m_columns = 1;
    m_currentSlice = 0;

    m_sliceActorCollection = vtkPropCollection::New();
    m_rendererCollection = vtkRendererCollection::New();
    m_informationCollection = vtkActor2DCollection::New();
    m_scalarBarCollection = vtkActor2DCollection::New();

    m_sliceActorCollection->AddItem( m_viewer->GetImageActor() );
    m_rendererCollection->AddItem( m_viewer->GetRenderer() );
    m_informationCollection->AddItem ( m_serieInformationAnnotation );
    m_scalarBarCollection->AddItem( m_scalarBar );

    this->updateGrid();

    m_anchoredRulerCoordinatesCollection = vtkCollection::New();
    m_anchoredRulerCoordinatesCollection->AddItem( m_anchoredRulerCoordinates );
    m_rulerActorCollection = vtkActor2DCollection::New();
    m_rulerActorCollection->AddItem( m_bottomRuler );
    m_rulerActorCollection->AddItem( m_sideRuler );

}

Q2DViewer::~Q2DViewer()
{
    m_scalarBar->Delete();
    m_voxelInformationCaption->Delete();
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

void Q2DViewer::createActions()
{
    m_resetAction = new QAction( this );
    m_resetAction->setText(tr("&Reset"));
    m_resetAction->setShortcut( tr("Ctrl+R") );
    m_resetAction->setStatusTip(tr("Reset initial parameters"));
    connect( m_resetAction, SIGNAL( triggered() ), this, SLOT( reset() ) );
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
    // Llegenda amb informació del voxel
    createVoxelInformationCaption();
    // Marcadors d'escala
    createRulers();
}

void Q2DViewer::createVoxelInformationCaption()
{
    m_voxelInformationCaption = vtkCaptionActor2D::New();
    m_voxelInformationCaption->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->SetAttachmentPoint( m_currentCursorPosition );
    m_voxelInformationCaption->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->BorderOff();
    m_voxelInformationCaption->LeaderOff();
    m_voxelInformationCaption->ThreeDimensionalLeaderOff();
    m_voxelInformationCaption->GetProperty()->SetColor( 1.0 , 0 , 0 );
    m_voxelInformationCaption->SetPadding( 0 );
    m_voxelInformationCaption->SetPosition( -1.0 , -1.0 );
    m_voxelInformationCaption->SetHeight( 0.05 );
    m_voxelInformationCaption->SetWidth( 0.3 );
    // propietats del texte
    m_voxelInformationCaption->GetCaptionTextProperty()->SetColor( 1. , 0.7 , 0.0 );
    m_voxelInformationCaption->GetCaptionTextProperty()->ShadowOn();
    m_voxelInformationCaption->GetCaptionTextProperty()->ItalicOff();
    m_voxelInformationCaption->GetCaptionTextProperty()->BoldOff();
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

    case Sagittal:
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

vtkScalarBarActor* Q2DViewer::createScalarBar()
{
    vtkScalarBarActor *scalarBar = vtkScalarBarActor::New();
    scalarBar->SetOrientationToVertical();
    scalarBar->GetPositionCoordinate()->SetCoordinateSystemToView();
    scalarBar->SetPosition( 0.8 , -0.8 );
    scalarBar->SetWidth( 0.1 );
    scalarBar->SetHeight( 0.6 );
    scalarBar->SetLabelFormat( "%.2f" );
    scalarBar->SetNumberOfLabels( 3 );
    scalarBar->GetLabelTextProperty()->ItalicOff();
    scalarBar->GetLabelTextProperty()->BoldOff();
    scalarBar->GetLabelTextProperty()->SetJustificationToRight();
    return scalarBar;
}

void Q2DViewer::updateScalarBar()
{
    // Així és més ràpid i senzill

    vtkScalarBarActor *scalarBar;
    int i;

    if( m_mainVolume )
    {
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
    //\TODO CAL APLICAR ELS FLIPS!!!!
    QString orientation = m_mainVolume->getVolumeSourceInformation()->getPatientOrientationString() ;
    QString revertedOrientation = m_mainVolume->getVolumeSourceInformation()->getRevertedPatientOrientationString() ;

    QStringList list = orientation.split(",");
    QStringList revertedList = revertedOrientation.split(",");

    if( list.size() > 1 )
    {
        // 0:Esquerra , 1:Abaix , 2:Dreta , 3:A dalt
        if( m_lastView == Axial )
        {
            //\TODO experimental, encara no sabem si ho estem fem bé el flip
            m_patientOrientationTextActor[ (0 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( revertedList.at(0) ) );
            m_patientOrientationTextActor[ (2 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[ (1 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[ (3 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( revertedList.at(1) ) );
        }
        else if( m_lastView == Sagittal )
        {
            m_patientOrientationTextActor[ (0 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( revertedList.at(1) ) );
            m_patientOrientationTextActor[ (2 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[ (1 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( revertedList.at(2) ) );
            m_patientOrientationTextActor[ (3 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(2) ) );
        }
        else if( m_lastView == Coronal )
        {
            m_patientOrientationTextActor[ (0 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( revertedList.at(0) ) );
            m_patientOrientationTextActor[ (2 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[ (1 + (4-m_rotateFactor)) % 4 ]->SetInput( qPrintable( revertedList.at(2) ) );
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
        // la info no existeix
    }
}

void Q2DViewer::updateAnnotations()
{
}

void Q2DViewer::addActors()
{
    // anotacions de texte FIXE
    this->getRenderer()->AddActor2D( m_serieInformationAnnotation );

    // anotacions de texte variable (window/level, window size, etc)
    vtkCornerAnnotation *sliceAnnotation = vtkCornerAnnotation::New();
    this->getRenderer()->AddActor2D( sliceAnnotation );
    m_sliceAnnotationsCollection->AddItem( sliceAnnotation );

    if( m_voxelInformationCaption )
    {
        this->getRenderer()->AddActor( m_voxelInformationCaption );
    }
    else
    {
        DEBUG_LOG( "No s'ha creat l'actor d'informació de voxel; no es pot afegir a l'escena" );
    }

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

void Q2DViewer::initTextAnnotations()
{
    if( m_mainVolume )
    {
        // informació fixa
        m_upperRightText = tr("%1")
                        .arg( m_mainVolume->getVolumeSourceInformation()->getPatientName() );
        // formatat de la data i hora de l'estudi
        QString studyDate = m_mainVolume->getVolumeSourceInformation()->getStudyDate();
        QString year = studyDate.mid( 0 , 4 );
        QString month = studyDate.mid( 4 , 2 );
        QString day = studyDate.mid( 6 , 2 );
        studyDate = day + QString( "/" ) + month + QString( "/" ) + year;

        QString studyTime = m_mainVolume->getVolumeSourceInformation()->getStudyTime();
        QString hour = studyTime.mid( 0 , 2 );
        QString minute = studyTime.mid( 2 , 2 );
        QString second = studyTime.mid( 4 , 2 );
        studyTime = hour + QString( ":" ) + minute + QString( ":" ) + second;

        m_upperRightText += tr("\n%1\n%2\n%3\nAcc:\n%4\n%5")
                    .arg( m_mainVolume->getVolumeSourceInformation()->getInstitutionName() )
                    .arg( m_mainVolume->getVolumeSourceInformation()->getPatientID() )
                    .arg( m_mainVolume->getVolumeSourceInformation()->getAccessionNumber() )
                    .arg( studyDate )
                    .arg( studyTime );

        m_lowerRightText = tr("%1")
                        .arg( m_mainVolume->getVolumeSourceInformation()->getProtocolName() );

        m_serieInformationAnnotation->SetText( 3, qPrintable( m_upperRightText ) );
        m_serieInformationAnnotation->SetText( 1, qPrintable( m_lowerRightText ) );
    }
    else
    {
        DEBUG_LOG("No hi ha un volum vàlid. No es poden inicialitzar les annotacions de texte");
    }
}

void Q2DViewer::setVoxelInformationCaptionEnabled( bool enable )
{
    enable ?  enableVoxelInformationCaption() : disableVoxelInformationCaption();
}

void Q2DViewer::enableVoxelInformationCaption()
{
    m_voxelInformationEnabled = true;
}

void Q2DViewer::disableVoxelInformationCaption()
{
    m_voxelInformationEnabled = false;
    m_voxelInformationCaption->VisibilityOff();
}

void Q2DViewer::updateVoxelInformation()
{
    vtkRenderWindowInteractor* interactor = m_vtkWidget->GetRenderWindow()->GetInteractor();
    // agafem el punt que està apuntant el ratolí en aquell moment \TODO podríem passar-li el 4t parèmatre opcional (vtkPropCollection) per indicar que només agafi de l'ImageActor, però no sembla que suigui necessari realment i que si fa pick d'un altre actor 2D no passa res
    m_picker->PickProp( interactor->GetEventPosition()[0], interactor->GetEventPosition()[1], m_viewer->GetRenderer() );
    // calculem el pixel trobat
    double q[3], imageValue;
    m_picker->GetPickPosition( q );
    int found = 0;
    // quan dona una posició de (0,0,0) és que estem fora de l'actor
    if( !( q[0] == 0 && q[1] == 0 && q[2] == 0) )
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
        vtkCell *cell = m_mainVolume->getVtkData()->FindAndGetCell( q , NULL , -1 , tolerance , subCellId , parametricCoordinates , interpolationWeights );
        if ( cell )
        {
            // Interpolate the point data
            outPointData->InterpolatePoint( pointData , 0 , cell->PointIds , interpolationWeights );
            imageValue = outPointData->GetScalars()->GetTuple1(0);
            found = 1;
        }
        outPointData->Delete();
    }
    if( !found )
    {
        updateCursor( -1, -1, -1, -1 );
        m_voxelInformationCaption->VisibilityOff();
    }
    else
    {
        updateCursor( q[0], q[1], q[2], imageValue );
        if( m_voxelInformationEnabled )
            m_voxelInformationCaption->VisibilityOn();
        m_voxelInformationCaption->SetAttachmentPoint( q );
        m_voxelInformationCaption->SetCaption( qPrintable( QString("(%1,%2,%3):%4").arg(m_currentCursorPosition[0],0,'f',2).arg(m_currentCursorPosition[1],0,'f',2).arg(m_currentCursorPosition[2],0,'f',2).arg(m_currentImageValue) ) );
    }
    this->refresh();
}

void Q2DViewer::setTool( QString toolName )
{
    if( m_toolManager->setCurrentTool( toolName ) )
    {
        ///\Todo per implementar
        DEBUG_LOG( qPrintable( QString("OK, hem activat la tool: ") + toolName ) );
    }
    else
    {
        ///\Todo per implementar
        DEBUG_LOG( qPrintable( QString(":/ no s'ha pogut activar la tool: ") + toolName ) );
    }
}

Tool *Q2DViewer::getTool( QString toolName )
{
    return m_toolManager->getTool( toolName );
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
    connect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
}

void Q2DViewer::disableTools()
{
    disconnect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
}

void Q2DViewer::eventHandler( vtkObject *obj, unsigned long event, void *client_data, void *call_data, vtkCommand *command )
{
    updateRulers();
    switch( event )
    {
    case vtkCommand::MouseMoveEvent:
        updateVoxelInformation();
    break;

    case vtkCommand::EnterEvent:
    break;

    case vtkCommand::LeaveEvent:
        m_voxelInformationCaption->VisibilityOff();
        this->refresh();
    break;

    default:
    break;
    }
    // fer el que calgui per cada tipus d'event
    emit eventReceived( event );
}

void Q2DViewer::contextMenuRelease( vtkObject* object , unsigned long event, void *client_data, void *call_data, vtkCommand *command )
{
    // Extret dels exemples de vtkEventQtSlotConnect
    // get interactor
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(object);
    // consume event so the interactor style doesn't get it
    command->AbortFlagOn();
    // Obtenim la posició de l'event (moure el mouse, en aquest cas)
    int eventPosition[2];
    iren->GetEventPosition( eventPosition );
    int* size = iren->GetSize();
    // remember to flip y
    QPoint pt = QPoint( eventPosition[0], size[1]-eventPosition[1]);

    // aquesta posició no és del tot bona ja que no són les coordenades globals, sin o de finestra
    QMenu contextMenu( this );
    contextMenu.addAction( m_resetAction );

    // map to global
    QPoint global_pt = contextMenu.parentWidget()->mapToGlobal( pt );
    contextMenu.exec( global_pt );
}

void Q2DViewer::setupInteraction()
{
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

// menú contextual TODO el farem servir???
//     m_vtkQtConnections->Connect( m_vtkWidget->GetRenderWindow()->GetInteractor(),
//                       QVTKWidget::ContextMenuEvent,//vtkCommand::RightButtonPressEvent,
//                        this,
//                        SLOT( contextMenuRelease(vtkObject*,unsigned long,void*, vtkCommand *) ) );
}

void Q2DViewer::setInput( Volume* volume )
{
    if( volume == 0 )
        return;
    m_mainVolume = volume;

//     m_viewer->SetInput( m_mainVolume->getVtkData() );
//     // ajustem el window Level per defecte
//     m_defaultWindow = m_mainVolume->getVolumeSourceInformation()->getWindow();
//     m_defaultLevel = m_mainVolume->getVolumeSourceInformation()->getLevel();
//     if( m_defaultWindow == 0.0 && m_defaultLevel == 0.0 )
//     {
//         double * range = m_mainVolume->getVtkData()->GetScalarRange();
//         m_defaultWindow = fabs( range[1] - range[0] );
//         m_defaultLevel = ( range[1] + range[0] )/ 2.0;
//     }

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

// \TODO això no sabem si serà del tot necessari
//     m_picker->PickFromListOn();
//     m_picker->AddPickList( m_viewer->GetImageActor() );

    m_numberOfPhases = m_mainVolume->getVolumeSourceInformation()->getNumberOfPhases();
    if( m_numberOfPhases > 1 )
        m_maxSliceValue = m_mainVolume->getVolumeSourceInformation()->getNumberOfSlices() - 1;
    else
    {
        m_maxSliceValue = m_viewer->GetSliceMax();
        m_numberOfPhases = 1;
    }

    if( m_numberOfPhases > 1 )
    {
        DEBUG_LOG( qPrintable( QString("Nombre de fases: %1, nombre de llesques per fase: %2").arg( m_numberOfPhases).arg( m_maxSliceValue) ) );
    }

    updateGrid();
    updateRulers();
    updateScalarBar();
    initTextAnnotations();
    updateInformation();
}

vtkInteractorStyleImage *Q2DViewer::getInteractorStyle()
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

void Q2DViewer::setView( ViewType view )
{
    m_lastView = view;
    resetCamera();
}

void Q2DViewer::updateCamera()
{
    if( m_viewer->GetInput() )
    {
        vtkCamera *cam;
        vtkRenderer *renderer;
        vtkRendererCollection* renderCollection = m_viewer->GetRenderWindow()->GetRenderers();
        int i = 0;

        switch( this->m_lastView )
        {
        case Axial:
            while( i < (renderCollection->GetNumberOfItems()) && i < m_viewer->GetSliceMax())
            {
                renderer = vtkRenderer::SafeDownCast( renderCollection->GetItemAsObject( i ) );
                cam = renderer->GetActiveCamera();
                if ( cam )
                {
                    cam->SetRoll( -m_rotateFactor*90. + 180. );
                }
                i++;
            }
            emit rotationDegreesChanged( -m_rotateFactor*90. + 180. );
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[1];
        break;
        case Sagittal:
            while( i < (renderCollection->GetNumberOfItems()) && i < m_viewer->GetSliceMax())
            {
                renderer = vtkRenderer::SafeDownCast( renderCollection->GetItemAsObject( i ) );
                cam = renderer->GetActiveCamera();
                if ( cam )
                {
                    cam->SetRoll( -m_rotateFactor*90. -90. );
                }
                i++;
            }
            emit rotationDegreesChanged( -m_rotateFactor*90. - 90. );
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[1];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;

        case Coronal:
            while( i < (renderCollection->GetNumberOfItems()) && i < m_viewer->GetSliceMax())
            {
                renderer = vtkRenderer::SafeDownCast( renderCollection->GetItemAsObject( i ) );
                cam = renderer->GetActiveCamera();
                if ( cam )
                {
                    cam->SetRoll( -m_rotateFactor*90. );
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
            cam->Azimuth( 180 );

            // Alternativa 2)
            // D'aquesta manera només movem l'actor. Per contra, el sistema de coordenades queda "igual" cosa que es reflexa en els rulers que no indiquen les coordenades correctes de la imatge
//             double *center;
//             center = m_viewer->GetImageActor()->GetCenter();
//             m_viewer->GetImageActor()->SetOrigin( center );
//             m_viewer->GetImageActor()->RotateY( 180 );

            // Alternativa 3) La que ens proposen a la mailing list. Fa el mateix que azimuth
            // una manera d'arreglar això seria mirar la posició respecte el centre i llavors invertir el desplaçament que hi hagi tant sobre les Y com sobre les X
//             double cameraPosition[3];
//             cam->GetPosition(cameraPosition);
//             double cameraFocalPoint[3];
//             cam->GetFocalPoint(cameraFocalPoint);
//             for ( int i = 0; i < 3; ++i )
//             {
//                 cameraPosition[i] = 2.0*cameraFocalPoint[i] - cameraPosition[i];
//             }
//             cam->SetPosition(cameraPosition);
//             this->getRenderer()->ResetCameraClippingRange();
//             m_viewer->Render();
            m_applyFlip = false;
        }
        mapOrientationStringToAnnotation();
        this->updateInformation();
        this->updateRulers();
        this->refresh();
    }
    else
    {
        WARN_LOG( "Intentant actualitzar rotació de càmera sense haver donat un input abans..." );
    }
}

void Q2DViewer::resetCamera()
{
    if( m_viewer->GetInput() )
    {
        m_rotateFactor = 0;
        m_applyFlip = false;
        m_isImageFlipped = false;
        vtkCamera *cam;
        vtkRenderer *renderer;
        int i = 0;
        switch( m_lastView )
        {
        case Axial:
            m_viewer->SetSliceOrientationToXY();
            while( i < (m_rendererCollection->GetNumberOfItems()) && i < m_viewer->GetSliceMax())
            {
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( i ) );
                cam = renderer->GetActiveCamera();
                if ( cam )
                {
                    cam->SetFocalPoint(0,0,0);
                    cam->SetViewUp(0,-1,0);
                    cam->SetPosition(0,0,-1);
                    cam->SetRoll( -m_rotateFactor*90. + 180. );
                    renderer->ResetCamera();
                }
                i++;
            }
            emit rotationDegreesChanged( -m_rotateFactor*90. + 180. );
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[1];
        break;

        case Sagittal:
            m_viewer->SetSliceOrientationToYZ();
            while( i < (m_rendererCollection->GetNumberOfItems()) && i < m_viewer->GetSliceMax())
            {
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( i ) );
                cam = renderer->GetActiveCamera();
                if ( cam )
                {
                    cam->SetFocalPoint(0,0,0);
                    cam->SetPosition(1,0,0); // -1 if medical ?
                    cam->SetViewUp(0,0,1);
                    cam->SetRoll( -m_rotateFactor*90. -90. );
                    renderer->ResetCamera();
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
            while( i < (m_rendererCollection->GetNumberOfItems()) && i < m_viewer->GetSliceMax())
            {
                renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( i ) );
                cam = renderer->GetActiveCamera();
                if ( cam )
                {
                    cam->SetFocalPoint(0,0,0);
                    cam->SetPosition(0,-1,0); // 1 if medical ?
                    cam->SetViewUp(0,0,1);
                    cam->SetRoll( -m_rotateFactor*90. );
                    renderer->ResetCamera();
                }
                emit rotationDegreesChanged( -m_rotateFactor*90. );
                i++;
            }
            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;
        }
        this->refresh();
        // cada cop que canviem de llesca posarem per defecte la llesca a 0 si té més d'una fase i la del mig si només en té 1
        if( m_numberOfPhases > 1 )
        {
            setSlice( 0 );
        }
        else
        {
            m_maxSliceValue = m_viewer->GetSliceRange()[1];
            setSlice( m_viewer->GetSliceRange()[1] /2  );
        }
        mapOrientationStringToAnnotation();
        updateRulers();
        this->updateDisplayExtent();
        this->refresh();
    }
    else
    {
        WARN_LOG( "Intentant canviar de vista sense haver donat un input abans..." );
    }
}

void Q2DViewer::setSlice( int value )
{
    if( value < 0 )
        value = 0;
    else if( value > m_maxSliceValue )
        value = m_maxSliceValue;

    this->m_currentSlice = value;

    if( this->m_mainVolume )
    {
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
    }
    this->updateDisplayExtent();

    emit sliceChanged( m_currentSlice );
    this->refresh();
}

void Q2DViewer::setPhase( int value )
{
    if( value < 0 )
        value = 0;
    else if( value > m_numberOfPhases )
        value = m_numberOfPhases;

    m_currentPhase = value;

    this->updateDisplayExtent();
}

void Q2DViewer::resizeEvent( QResizeEvent *resize )
{
    //\TODO l'única info que cal actualitzar és la mida de finestra/viewport
    updateInformation();
}

void Q2DViewer::setWindowLevel( double window , double level )
{
    if( m_mainVolume )
    {
        m_windowLevelLUTMapper->SetWindow( window );
        m_windowLevelLUTMapper->SetLevel( level );
        updateInformation();
        updateScalarBar();
        refresh();
    }
    else
    {
        DEBUG_LOG( "::setWindowLevel() : No tenim input " );
    }
}

void Q2DViewer::getDefaultWindowLevel( double wl[2] )
{
    if( m_mainVolume )
    {
        wl[0] = m_defaultWindow;
        wl[1] = m_defaultLevel;
    }
    else
    {
        DEBUG_LOG( "::getDefaultWindowLevel() : No tenim input " );
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
        //\TODO fer updateInformation() en comptes d'aquest?
        updateWindowLevelAnnotation();
    }
    else
    {
        DEBUG_LOG( "::resetWindowLevelToDefault() : No tenim input" );
    }
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
        DEBUG_LOG( qPrintable( QString("Ratio no aplicable: %1").arg(ratio) ) );
    }
}

void Q2DViewer::setPresentationPixelSpacing( double x , double y )
{
    m_presentationPixelSpacing[0] = x;
    m_presentationPixelSpacing[1] = y;
}

void Q2DViewer::scaleToFit( double topLeftX, double topLeftY, double bottomRightX, double bottomRightY )
{
    double width, height;
    width = fabs( topLeftX - bottomRightX );
    height = fabs( topLeftY - bottomRightY );

    int *size = this->getRenderer()->GetSize();
    int *rendererOrigin = this->getRenderer()->GetOrigin();
    vtkCamera *cam = this->getRenderer()->GetActiveCamera();

    double min[2];
    double rbcenter[4];
    min[0] = bottomRightX < topLeftX ?
        bottomRightX : topLeftX;
    min[1] = bottomRightY < topLeftY ?
        bottomRightY : topLeftY;

    rbcenter[0] = min[0] + 0.5*width;
    rbcenter[1] = min[1] + 0.5*height;
    rbcenter[2] = 0.0;
    rbcenter[3] = 1.0;

    // \TODO aquesta normalització potser no és necessària
    double invw;
    double winCenter[3];
    winCenter[0] = rendererOrigin[0] + 0.5*size[0];
    winCenter[1] = rendererOrigin[1] + 0.5*size[1];
    winCenter[2] = 0;

    this->getRenderer()->SetDisplayPoint( winCenter );
    this->getRenderer()->DisplayToView();
    this->getRenderer()->ViewToWorld();

    double worldWinCenter[4];
    this->getRenderer()->GetWorldPoint( worldWinCenter );
    invw = 1.0/worldWinCenter[3];
    worldWinCenter[0] *= invw;
    worldWinCenter[1] *= invw;
    worldWinCenter[2] *= invw;

    double translation[3];
    translation[0] = rbcenter[0] - worldWinCenter[0];
    translation[1] = rbcenter[1] - worldWinCenter[1];
    translation[2] = rbcenter[2] - worldWinCenter[2];

    double pos[3], fp[3];
    cam->GetPosition( pos );
    cam->GetFocalPoint( fp );

    pos[0] += translation[0];
    pos[1] += translation[1];
    pos[2] += translation[2];
    fp[0] += translation[0];
    fp[1] += translation[1];
    fp[2] += translation[2];

    cam->SetPosition( pos );
    cam->SetFocalPoint( fp );

    // ara cal calcular la width i height en coordenades de display
    double displayTopLeft[3], displayBottomRight[3];
    this->computeWorldToDisplay( this->getRenderer(), topLeftX, topLeftY, 0.0, displayTopLeft );
    this->computeWorldToDisplay( this->getRenderer(), bottomRightX, bottomRightY, 0.0, displayBottomRight );
    // recalculem ara tenint en compte el display
    width = fabs( displayTopLeft[0] - displayBottomRight[0] );
    height = fabs( displayTopLeft[1] - displayBottomRight[1] );
    //\TODO caldria considerar l'opció d'afegir un marge per si no volem que la regió escollida mantingui una distància amb les vores de la finestra
    // Ajustem la imatge segons si la finestra és més estreta per ample o per alçada. Si volem que es vegi tota la regió que em escollit, ajustarem per el que sigui més estret, si ajustèssim pel més ample perderiem imatge per l'altre part
    if( size[0] < size[1] )
        cam->Zoom( size[0] / (float)width );
    else
        cam->Zoom( size[1] / (float)height );

    this->getRenderer()->ResetCameraClippingRange();
    this->refresh();
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
        DEBUG_LOG( qPrintable( QString("Factor no aplicable: %1").arg(factor) ) );
    }
}

void Q2DViewer::updateWindowLevelAnnotation()
{
    updateInformation();
    emit windowLevelChanged( m_windowLevelLUTMapper->GetWindow() , m_windowLevelLUTMapper->GetLevel() );
    updateScalarBar();
}

void Q2DViewer::reset()
{
    //\TODO: completar, encara és incomplert
    setViewToAxial();
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

void Q2DViewer::saveAll( const char *baseName , FileType extension )
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

void Q2DViewer::saveCurrent( const char *baseName , FileType extension )
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
            pngWriter->SetFilePrefix( baseName );
            pngWriter->Write();

            break;
        }
        case JPEG:
        {
            vtkImageWriter *jpegWriter = vtkJPEGWriter::New();
            jpegWriter->SetInput( image );
            jpegWriter->SetFilePattern( "%s-%d.jpg" );
            jpegWriter->SetFilePrefix( baseName );
            jpegWriter->Write();

            break;
        }
        // \TODO el format tiff fa petar al desar, mirar si és problema de compatibilitat del sistema o de les pròpies vtk
        case TIFF:
        {
            vtkImageWriter *tiffWriter = vtkTIFFWriter::New();
            tiffWriter->SetInput( image );
            tiffWriter->SetFilePattern( "%s-%d.tif" );
            tiffWriter->SetFilePrefix( baseName );
            tiffWriter->Write();

            break;
        }
        case PNM:
        {
            vtkImageWriter *pnmWriter = vtkPNMWriter::New();
            pnmWriter->SetInput( image );
            pnmWriter->SetFilePattern( "%s-%d.pnm" );
            pnmWriter->SetFilePrefix( baseName );
            pnmWriter->Write();

            break;
        }
        case BMP:
        {
            vtkImageWriter *bmpWriter = vtkBMPWriter::New();
            bmpWriter->SetInput( image );
            bmpWriter->SetFilePattern( "%s-%d.bmp" );
            bmpWriter->SetFilePrefix( baseName );
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
            metaWriter->SetFileName( baseName );
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

void Q2DViewer::updateInformation()
{
    vtkCornerAnnotation *sliceAnnotation;
    vtkRenderer *renderer;
    vtkAxisActor2D *ruler;
    int i = 0, numRuler = 0;

    //\TODO mirar perquè cal posar aquesta crida, sino al canviar de llesca d'avegades es deixa de veure l'actor
    updateViewports();

    // Informació que es mostra per cada viewport
    for ( i = 0; i < m_rendererCollection->GetNumberOfItems(); i++)
    {
        sliceAnnotation = vtkCornerAnnotation::SafeDownCast ( m_sliceAnnotationsCollection->GetItemAsObject ( i ) );
        renderer = vtkRenderer::SafeDownCast ( m_rendererCollection->GetItemAsObject( i ) );

        // Mostrem o amaguem els rulers del volum
        ruler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( numRuler ));
        if ( m_enabledAnnotations & Q2DViewer::RulersAnnotation )
            ruler->VisibilityOn();
        else
            ruler->VisibilityOff();

        ruler = vtkAxisActor2D::SafeDownCast ( m_rulerActorCollection->GetItemAsObject( numRuler+1 ));
        if ( m_enabledAnnotations & Q2DViewer::RulersAnnotation )
            ruler->VisibilityOn();
        else
            ruler->VisibilityOff();

        numRuler += 2;

        // informació de la finestra
        if( m_enabledAnnotations & Q2DViewer::WindowInformationAnnotation )
        {
            m_upperLeftText = tr("Image Size: %1 x %2\nView Size: %3 x %4\nWW: %5 WL: %6 ")
                    .arg( m_imageSizeInformation[0] )
                    .arg( m_imageSizeInformation[1] )
                    .arg( renderer->GetSize()[0] )
                    .arg( renderer->GetSize()[1] )
                    .arg( m_windowLevelLUTMapper->GetWindow() )
                    .arg( m_windowLevelLUTMapper->GetLevel() );
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

    this->refresh();
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
        while( i < (m_columns*m_rows) && i < m_viewer->GetSliceMax())
        {
            addRenderScene();
            i++;
        }
    }
    else if( (m_numberOfSlicesWindows > m_columns*m_rows || m_numberOfSlicesWindows > m_viewer->GetSliceMax()) && m_numberOfSlicesWindows > 1 )
    {
        i = m_numberOfSlicesWindows;
        while( i > (m_columns*m_rows) || i > m_viewer->GetSliceMax() )
        {
            removeRenderScene();
            i--;
        }
    }

    m_numberOfSlicesWindows = m_columns*m_rows;

    if( m_viewer->GetInput() )
    {
        this->setSlice( m_currentSlice );
        // Actualitzar els rulers
        updateRulers();
    }
    else
        updateViewports(); // Redistribuir viewports
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

    int maxSlice = m_viewer->GetSliceMax();
    vtkRenderer* renderer;

    int rendererIndex = 0;
    int slice = m_currentSlice;

    for ( i = 0; i < m_rows; i++ )
    {
        ymin = ymax-sizeVertical;
        j=0;

        while (j < m_columns && ((i*m_columns + j)< maxSlice) )
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
    int i = 0;
    int rendererIndex = 0;
    int value = m_currentSlice*m_numberOfPhases + m_currentPhase;
    vtkRenderer *renderer;
    vtkImageActor *imageActor;
    vtkImageData *input = m_viewer->GetInput();
    vtkCornerAnnotation *sliceAnnotation;
    int *wholeExtent = input->GetWholeExtent();
    QString lowerLeftText;

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
                    //\TODO aquest switch no hauria de dependre de l'm_viewer, hauria d'anar en funció de les vistes que controlem nosaltres ab m_lastView
                    switch( m_viewer->GetSliceOrientation() )
                    {
                        case vtkImageViewer2::SLICE_ORIENTATION_XY:
                            imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], wholeExtent[2], wholeExtent[3], value, value );
                            break;

                        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                            imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], value, value, wholeExtent[4], wholeExtent[5] );
                            break;

                        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                            imageActor->SetDisplayExtent( value, value, wholeExtent[2], wholeExtent[3], wholeExtent[4], wholeExtent[5] );
                            break;
                    }

                    lowerLeftText = tr("Slice: %1/%2 Phase: %3/%4")
                        .arg( ((int)(value/m_numberOfPhases)) + 1 )
                        .arg( m_maxSliceValue +1 )
                        .arg( value + 1 )
                        .arg( m_numberOfPhases );
                    sliceAnnotation->SetText( 0 , qPrintable(lowerLeftText) );
                    if( value >=  m_viewer->GetSliceMax() )
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
            renderer = vtkRenderer::SafeDownCast( m_rendererCollection->GetItemAsObject( rendererIndex ) );
            imageActor = vtkImageActor::SafeDownCast( m_sliceActorCollection->GetItemAsObject( rendererIndex ) );
            sliceAnnotation = vtkCornerAnnotation::SafeDownCast( m_sliceAnnotationsCollection->GetItemAsObject ( rendererIndex ) );

            if( imageActor )
            {
                switch( m_viewer->GetSliceOrientation() )
                {
                    case vtkImageViewer2::SLICE_ORIENTATION_XY:
                        imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], wholeExtent[2], wholeExtent[3], value, value );
                        break;

                    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                        imageActor->SetDisplayExtent( wholeExtent[0], wholeExtent[1], value, value, wholeExtent[4], wholeExtent[5] );
                        break;

                    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                        imageActor->SetDisplayExtent( value, value, wholeExtent[2], wholeExtent[3], wholeExtent[4], wholeExtent[5] );
                        break;
                }

                if ( m_numberOfPhases > 1)
                {
                    lowerLeftText = tr("Slice: %1/%2 Phase: %3/%4")
                        .arg( (value/m_numberOfPhases) + 1 )
                        .arg( m_maxSliceValue + 1 )
                        .arg( m_currentPhase + 1 )
                        .arg( m_numberOfPhases );
                }
                else
                {
                    lowerLeftText = tr("Slice: %1/%2")
                    .arg( value + 1 )
                    .arg( m_maxSliceValue + 1 );
                }
                sliceAnnotation->SetText( 0 , qPrintable(lowerLeftText) );

                if( value >=  m_viewer->GetSliceMax() )
                    value = 0;
                else
                    value += m_numberOfPhases;
            }
            rendererIndex++;
        }
    }
    this->updateInformation();
}

void Q2DViewer::enableAnnotation( AnnotationFlags annotation, bool enable )
{
    if( enable )
        m_enabledAnnotations = m_enabledAnnotations | annotation;
    else
        m_enabledAnnotations =  m_enabledAnnotations & ~annotation ;
}

void Q2DViewer::removeAnnotation( AnnotationFlags annotation )
{
    enableAnnotation( annotation, false );
}

void Q2DViewer::computeInputGrayscalePipeline()
{
    // si llegim l'arxiu tal qual, la modality no cal aplicar-la perquè les pròpies gdcm ja ens apliquen la modality lut
    computeModalityLUT();
//     applyMaskSubstraction();
    computeVOILUT();
}

void Q2DViewer::applyGrayscalePipeline()
{
    DEBUG_LOG( "*** Grayscale Transform Pipeline Begin ***" );
    DEBUG_LOG( qPrintable( QString("Image Information: Bits Allocated: %1, Bits Stored: %2, Pixel Range %3 to %4, SIGNED?Pixel Representation: %5, Photometric interpretation: %6")
    .arg( m_mainVolume->getVolumeSourceInformation()->getBitsAllocated() )
    .arg( m_mainVolume->getVolumeSourceInformation()->getBitsStored() )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[0] )
    .arg( m_mainVolume->getVtkData()->GetScalarRange()[1] )
    .arg( m_mainVolume->getVolumeSourceInformation()->getPixelRepresentation() )
    .arg( m_mainVolume->getVolumeSourceInformation()->getPhotometricInterpretationAsString() )
                    ) );


//\TODO Això s'ha d'aplicar enfunció de si tenim presentationm state o no? mirar si s'ha de fer aquí o al presentation state attacher...

//     if( m_mainVolume->getVolumeSourceInformation()->isMonochrome1() && m_presentationStateFilename != NULL )
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
    updateWindowLevelAnnotation();
}

void Q2DViewer::computeModalityLUT()
{
    // If the Modality LUT or equivalent Attributes are part of both the Image and the Presentation State, then the Presentation State Modality LUT shall be used instead of the Image Modality LUT or equivalent Attributes in the Image. If the Modality LUT is not present in the Presentation State it shall be assumed to be an identity transformation. Any Modality LUT or equivalent Attributes in the Image shall not be used.

    // busquem si existeix una modality lut
    m_modalityLut = this->parseLookupTable( 0 );
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
        DEBUG_LOG( qPrintable( QString("Image Modality LUT Adjustment: Rescale Slope %1, Rescale Intercept %2")
        .arg( m_mainVolume->getVolumeSourceInformation()->getRescaleSlope() )
        .arg( m_mainVolume->getVolumeSourceInformation()->getRescaleIntercept() )
        ) );
        if( m_modalityLUTRescale )
        {
            m_modalityLUTRescale->Delete();
            m_modalityLUTRescale = 0;
        }
        m_modalityRange[0] = m_mainVolume->getVtkData()->GetScalarRange()[0];
        m_modalityRange[1] = m_mainVolume->getVtkData()->GetScalarRange()[1];
    }
}

void Q2DViewer::applyMaskSubstraction()
{
    //
    // 2. Mask Substraction
    //
    //             The mask transformation may be applied in the case of multi-frame images for which other frames at a fixed frame position or time interval relative to the current frame may be subtracted from the current frame. Multiple mask frames may be averaged, and sub-pixel shifted before subtraction.
    //             The result will be a signed value with a bit length one longer than the source frames.
    // When there is no difference between corresponding pixel values, the subtracted image pixel will have a value of 0.
    // If a pixel in the current frame has a greater value than in the mask frame, then the resulting frame shall have a positive value. If it has a lesser value, then the resulting frame shall have a negative value.

    // aquests canvis s'apliquen a totes les imatges
}

void Q2DViewer::computeVOILUT()
{
    //
    // 3. VOI LUT
    //
    // If a VOI LUT is part of both the Image and the Presentation State then the Presentation State VOI LUT shall be used instead of the Image VOI LUT. If a VOI LUT (that applies to the Image) is not present in the Presentation State , it shall be assumed to be an identity transformation. Any VOI LUT or equivalent values in the Image shall not be used.

    // aquests canvis es poden aplicar a un subconjunt de imatges/frames. Per tant podem tenir diverses VOI LUT per una mateixa sèrie que s'apliquen a diverses imatges.

    // primer busquem voi lut, sinó busquem window level
    m_windowLevelLut = this->parseLookupTable(1);
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

        if( m_mainVolume->getVolumeSourceInformation()->hasWindowLevel() )
        {
            // Encara que en tingui més d'un window level, agafarem el primer i prou. Si n'hi ha més s'escolliran desde l'extensió adequada
            m_defaultWindow = m_mainVolume->getVolumeSourceInformation()->getWindow();
            m_defaultLevel = m_mainVolume->getVolumeSourceInformation()->getLevel();
            DEBUG_LOG( qPrintable( QString("Image VOI Adjustment: Window: %1, Level: %2")
                .arg( m_defaultWindow )
                .arg( m_defaultLevel )
                ) );
        }
        else
        {
            // ajustar un al rang de dades adequat
            m_defaultWindow = fabs( m_modalityRange[1] - m_modalityRange[0] );
            m_defaultLevel = ( m_modalityRange[1] + m_modalityRange[0] )/ 2.0;
            DEBUG_LOG( qPrintable( QString("No Image VOI Adjustment, creating a nice and automatic one: Window: %1, Level: %2")
            .arg( m_defaultWindow )
            .arg( m_defaultLevel )
            ) );
        }
    }
}

vtkWindowLevelLookupTable *Q2DViewer::parseLookupTable( int type )
{
    DcmTagKey lutType;
    vtkWindowLevelLookupTable *vtkLut = 0;
    QString lutDescription;
    bool signedRepresentation = false;
    if( m_mainVolume->getVolumeSourceInformation()->getPixelRepresentation() == VolumeSourceInformation::SignedPixelRepresentation )
            signedRepresentation = true;
    switch( type )
    {
    case 0:
        lutType = DCM_ModalityLUTSequence;
        lutDescription = "Modality LUT";
    break;

    case 1:
        lutType = DCM_VOILUTSequence;
        lutDescription = "Window Level LUT";
    break;

    case 2:
        lutType = DCM_PresentationLUTSequence;
        lutDescription = "Presentation LUT";
    break;
    }
    DcmStack stack;
    bool ok = true;

    DcmDataset *data = m_mainVolume->getVolumeSourceInformation()->getDicomDataset();
    if( data->search( lutType, stack ).bad() )
        ok = false;
    else
        DEBUG_LOG( qPrintable( QString("Parsing [%1] from dicom dataset").arg( lutDescription ) ) );

    if( ok )
    {
        DcmSequenceOfItems *lutSequence = NULL;
        lutSequence = OFstatic_cast( DcmSequenceOfItems *,stack.top() );
        DcmItem *item = lutSequence->getItem( 0 );
        // obtenim la descripció de la lut que ens especifica el format d'aquesta
        const Uint16 *lutDescriptor;
        OFCondition status = item->findAndGetUint16Array( DcmTagKey( DCM_LUTDescriptor ) , lutDescriptor  );
        if( status.good() )
        {
            // 0: # d'entrades a la lookup table. Quan val 0 equival a 2^16 ( 65536 )

            // 1: FirstStoredPixelValueMapped: El valor del primer valor de pixel de la imatge mapejat. Aquest valor es mapeja a la primera entrada de la lut. Tots els valors de pixel de la imatge menors que aquest es mapejen amb el valor de la primera entrada de la lut. Un pixel amb valor FirstStoredPixelValueMapped + 1, es mapejarà amb la següent entrada de la lut i així fins arribar a la última entrada de la lut. Els valors per sobre de la última entrada també es mapejen amb el valor de l'última entrada de la lut. Aquest valor estableix la relació entre els valors de la imatge i les entrades de la lut.
            // Per exemple, FirstStoredPixelValueMapped = 63488. Els valors de la imatge que estiguin per sota d'aquest se'ls assignarà el valor de la primera entrada de la lut (lut[0]). FirstStoredPixelValueMapped+1 es correspondrà amb lut[1], etc fins FirstStoredPixelValueMapped+n=> lut[n-1], on n seran el nombre d'entrades de la lut. Els valors de pixel més grans que FirstStoredPixelValueMapped+n-1 es mapejaran amb el valor de lut[n-1].

            // 2: Especifica el nombre de bits per cada entrada a la lut, que podran ser 8 o 16, corresponent amb el rang de valors de la lut que podrà ser de 256 o 65536 respectivament

            // allotjem la memòria per les dades de la lut
            int numberOfEntries;
            if( lutDescriptor[0] == 0 )
                numberOfEntries = 65535;
            else
                numberOfEntries =  lutDescriptor[0];
            signed int firstStored;
            if( signedRepresentation )
                firstStored = static_cast<signed short>( lutDescriptor[1] );
            else
                firstStored = lutDescriptor[1];

            DEBUG_LOG( qPrintable( QString("LUT Descriptor: %1\\%2\\%3")
            .arg( numberOfEntries )
            .arg( firstStored )
            .arg( lutDescriptor[2] )
            ) );

            vtkLut = vtkWindowLevelLookupTable::New();
            vtkLut->SetNumberOfTableValues( numberOfEntries );
            vtkLut->SetTableRange( firstStored , firstStored + numberOfEntries );

            // Encara que lutDescriptor[2] pugui ser 8,10,12 o 16 bits, sempre llegirem en un vector de components de 16 bits per comoditat
            const Uint16 *lutData16;
            lutData16 = new Uint16[ numberOfEntries ];
            // obtenim les dades de la lut
            status = item->findAndGetUint16Array( DcmTagKey( DCM_LUTData ) , lutData16  );
            if( status.good() )
            {
                unsigned int min = 65535, max = 0;
                for( int i = 0; i < numberOfEntries; i++ )
                {
                    if( lutData16[i] > max )
                        max = lutData16[i];
                    if( lutData16[i] < min )
                        min = lutData16[i];
                }
//                 std::cout << std::endl << std::endl << "************ COMENÇUT **************" << std::endl << std::endl;
                for( int i =0; i < numberOfEntries; i++ )
                {
                    double value;
                    if( m_windowLevelLut ) // ens precedeix una VOI lut
                        value = m_windowLevelLut->GetTableValue( i )[0];
                        //value = m_windowLevelLut->GetLuminance( i );
                    else if( m_modalityLut ) // només ens precedeix una modality
                        value = m_modalityLut->GetTableValue( i )[0];
                        //value = m_modalityLut->GetLuminance( i );
                    else // no hi ha cap lut precedent
                        value = (double)lutData16[ i ]/max;
//                     std::cout << "Value que coloco a l'índex[" << i << "]: " << value << std::endl;
                    vtkLut->SetTableValue( i , value , value , value , 1.0 );
                }
//                 std::cout << std::endl << std::endl << "************ ACABUT **************" << std::endl << std::endl;
            }
            else
                std::cout << "Error message:: " << status.text() << std::endl;

// experiment
//             unsigned char *outTable = new unsigned char[ numberOfEntries ];
//             void *inPtr = m_mainVolume->getVtkData()->GetScalarPointer();
//             vtkLut->MapScalarsThroughTable2( inPtr, outTable, m_mainVolume->getVtkData()->GetScalarType(), numberOfEntries, 1, 1 );
//             for( int i = 0; i < numberOfEntries; i++ )
//                 std::cout << "value: " << (int)outTable[i] << std::endl;

//             vtkLut->SetRampToLinear();
        }
        else
            std::cout << "Error message:: " << status.text() << std::endl;
    }
    return vtkLut;
}

};  // end namespace udg

