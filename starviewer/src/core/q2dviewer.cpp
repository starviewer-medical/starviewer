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

namespace udg {

Q2DViewer::Q2DViewer( QWidget *parent )
 : QViewer( parent ), m_serieInformationAnnotation(0), m_numberOfPhases(1), m_maxSliceValue(0), m_currentPhase(0), m_numberOfSlicesWindows(1)
{
    m_enabledAnnotations = Q2DViewer::AllAnnotation;
    m_lastView = None;
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

    m_sliceActorCollection->AddItem(m_viewer->GetImageActor());

    m_rendererCollection->AddItem( m_viewer->GetRenderer() );

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
    createScalarBar();
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

void Q2DViewer::createScalarBar()
{
    m_scalarBar = vtkScalarBarActor::New();
    m_scalarBar->SetOrientationToVertical();
    m_scalarBar->GetPositionCoordinate()->SetCoordinateSystemToView();
    m_scalarBar->SetPosition( 0.8 , -0.8 );
    m_scalarBar->SetWidth( 0.1 );
    m_scalarBar->SetHeight( 0.6 );
    m_scalarBar->SetLabelFormat( "%.2f" );
    m_scalarBar->SetNumberOfLabels( 3 );
    m_scalarBar->GetLabelTextProperty()->ItalicOff();
    m_scalarBar->GetLabelTextProperty()->BoldOff();
    m_scalarBar->GetLabelTextProperty()->SetJustificationToRight();
}

void Q2DViewer::updateScalarBar()
{
    if( m_mainVolume )
    {
        // \TODO HACK!!
        vtkLookupTable * lookup = vtkLookupTable::SafeDownCast( this->getImageViewer()->GetWindowLevel()->GetLookupTable() );
        if ( !lookup )
        {
            lookup = vtkLookupTable::New();
            double range[2];
            range[0] = m_viewer->GetColorLevel() - m_viewer->GetColorWindow()/2;
            range[1] = m_viewer->GetColorLevel() + m_viewer->GetColorWindow()/2;
            lookup->SetTableRange( range );
            lookup->SetSaturationRange( 0 , 0 );
            lookup->SetHueRange( 0 , 0 );
            lookup->SetValueRange( 0 , 1 );
            lookup->Build();
        }
        m_scalarBar->SetLookupTable( lookup );
    }
    else
        DEBUG_LOG( "No hi ha cap volum assignat. No podem donar LUT a l'escala de colors" );
}

void Q2DViewer::updateCameraRotation()
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
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[1];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;

        case Coronal:
            m_viewer->SetSliceOrientationToXZ();
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
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;
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

void Q2DViewer::rotateClockWise()
{
    m_rotateFactor = (m_rotateFactor+1) % 4 ;
    updateCameraRotation();
}

void Q2DViewer::rotateCounterClockWise()
{
    m_rotateFactor = (m_rotateFactor+3) % 4 ;
    updateCameraRotation();
}

void Q2DViewer::mapOrientationStringToAnnotation()
{
    QString orientation = m_mainVolume->getVolumeSourceInformation()->getPatientOrientationString() ;
    QString revertedOrientation = m_mainVolume->getVolumeSourceInformation()->getRevertedPatientOrientationString() ;

    QStringList list = orientation.split(",");
    QStringList revertedList = revertedOrientation.split(",");

    if( list.size() > 1 )
    {
        // 0:Esquerra , 1:Abaix , 2:Dreta , 3:A dalt
        if( m_lastView == Axial )
        {
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
                                 SLOT( eventHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*) )
                                 );
    // \TODO fer això aquí? o fer-ho en el tool manager?
    this->getInteractor()->RemoveObservers( vtkCommand::LeftButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::RightButtonPressEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::MouseWheelForwardEvent );
    this->getInteractor()->RemoveObservers( vtkCommand::MouseWheelBackwardEvent );

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
    m_viewer->SetInput( m_mainVolume->getVtkData() );
    // ajustem el window Level per defecte
    m_defaultWindow = m_mainVolume->getVolumeSourceInformation()->getWindow();
    m_defaultLevel = m_mainVolume->getVolumeSourceInformation()->getLevel();
    if( m_defaultWindow == 0.0 && m_defaultLevel == 0.0 )
    {
        double * range = m_mainVolume->getVtkData()->GetScalarRange();
        m_defaultWindow = fabs( range[1] - range[0] );
        m_defaultLevel = ( range[1] + range[0] )/ 2.0;
    }

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

    m_numberOfPhases = m_mainVolume->getVolumeSourceInformation()->getNumberOfPhases();
    if( m_numberOfPhases > 1 )
        m_maxSliceValue = m_mainVolume->getVolumeSourceInformation()->getNumberOfSlices();
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

        // \TODO hauríem d'actualitzar valors que es calculen al setInput!

    //std::cout<<"overlay: "<<m_overlay<<" [CB="<<CheckerBoard<<", BL="<<Blend<<" ,RW="<<RectilinearWipe<<"]" <<std::endl;
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
        }
        else
        {
            //\TODO: sembla que hi hauria una manera millor de fer-ho, però l'Anton no l'ha trobat
            m_blender->Delete();
            m_blender = vtkImageBlend::New();
        }
        m_blender->SetInput(m_mainVolume->getVtkData());
        m_blender->AddInput(m_overlayVolume->getVtkData());
        m_blender->SetOpacity( 1, 1.0 - m_opacityOverlay );
        m_blender->SetOpacity( 2, m_opacityOverlay );
        m_viewer->SetInputConnection( m_blender->GetOutputPort() ); // li donem el blender com a input
        // \TODO hauríem d'actualitzar valors que es calculen al setInput!
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
        updateView();
    }
    else
    {
        DEBUG_LOG( "::render() : No hi ha cap volum per visualitzar" );
    }
}

void Q2DViewer::setView( ViewType view )
{
    m_lastView = view;
    m_rotateFactor = 0;
    updateView();
}

void Q2DViewer::updateView()
{
    if( m_viewer->GetInput() )
    {
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
//                     this->getRenderer()->ResetCamera();
                    renderer->ResetCamera();
                }
                i++;
            }
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
                i++;
            }
            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;

        default:
        // podem posar en Axial o no fer res
            this->setView( Axial );
        break;
        }
        // cada cop que canviem de llesca posarem per defecte la llesca a 0 si té més d'una fase i la del mig si només en té 1
        if( m_numberOfPhases > 1 )
        {
            setSlice( 0 );
        }
        else
        {
            setSlice( m_viewer->GetSliceRange()[1] /2  );
            m_maxSliceValue = m_viewer->GetSliceRange()[1];
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
        m_viewer->SetColorLevel( level );
        m_viewer->SetColorWindow( window );
        updateInformation();
        updateScalarBar();
        getInteractor()->Render();
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
        wl[0] = m_viewer->GetColorWindow();
        wl[1] = m_viewer->GetColorLevel();
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
        return m_viewer->GetColorWindow();
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
        return m_viewer->GetColorLevel();
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
    if( m_mainVolume )
    {
        m_viewer->SetColorWindow( m_defaultWindow );
        m_viewer->SetColorLevel( m_defaultLevel );
        this->refresh();
        updateWindowLevelAnnotation();
    }
    else
    {
        DEBUG_LOG( "::resetWindowLevelToDefault() : No tenim input" );
    }
}

void Q2DViewer::updateWindowLevelAnnotation()
{
    updateInformation();
    emit windowLevelChanged( m_viewer->GetColorWindow() , m_viewer->GetColorLevel() );
    updateScalarBar();
}

void Q2DViewer::reset()
{
    //\TODO: completar, encara és incomplert
    updateView();
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
                    .arg( m_viewer->GetColorWindow() )
                    .arg( m_viewer->GetColorLevel() );
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
    vtkAxisActor2D *sideRuler;
    vtkAxisActor2D *bottomRuler;
    vtkCoordinate *anchoredRulerCoordinates;

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
    renderer->AddActor2D( m_serieInformationAnnotation );

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
    renderer->AddActor2D( m_scalarBar );

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
    vtkCornerAnnotation *sliceAnnotation;

    int rendererIndex = 0;

    for ( i = 0; i < m_rows; i++ )
    {
        ymin = ymax-sizeVertical;
        j=0;

        while (j < m_columns && ((i*m_columns + j)< maxSlice) )
        {
            QMap<int,int*>::iterator iterator = m_phaseGridMap.find( i*m_rows + j );
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
                        .arg( i + 1 )
                        .arg( m_maxSliceValue + 1 )
                        .arg( value + 1 )
                        .arg( m_numberOfPhases );
                    sliceAnnotation->SetText( 0 , lowerLeftText.toAscii() );
                    //renderer->ResetCamera();
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

                lowerLeftText = tr("Slice: %1/%2 Phase: %3/%4")
                    .arg( i + 1 )
                    .arg( m_maxSliceValue + 1 )
                    .arg( value + 1 )
                    .arg( m_numberOfPhases );
                sliceAnnotation->SetText( 0 , lowerLeftText.toAscii() );

                //renderer->ResetCamera();
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

};  // end namespace udg

