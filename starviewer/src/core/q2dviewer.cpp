/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <vtkMetaImageWriter.h>
#include "q2dviewer.h"
#include "volume.h"
#include "volumesourceinformation.h"
#include "logging.h"

// include's qt
#include <QResizeEvent>
#include <QSize>
#include <QMenu>
#include <QAction>

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
// desar imatges
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkBMPWriter.h>
#include <vtkCamera.h>
// voxel information
#include <vtkPointData.h>
#include <vtkCell.h>
#include <vtkCaptionActor2D.h>
#include <vtkPropPicker.h>

namespace udg {

Q2DViewer::Q2DViewer( QWidget *parent , unsigned int annotations )
 : QViewer( parent )
{
    m_enabledAnnotations = annotations;
    m_lastView = None;
    m_currentSlice = 0;
    m_imageSizeInformation[0] = 0;
    m_imageSizeInformation[1] = 0;
    m_overlay = CheckerBoard; // per defecte
    updateCursor( -1, -1, -1, -1 );
    
    // inicialitzacions d'objectes
    // visor
    m_viewer = vtkImageViewer2::New();
    // preparem el picker
    m_picker = vtkPropPicker::New();
    m_pickedAxisActor = 0;
    m_overlayVolume = 0;
    m_voxelInformationCaption = 0;
    m_textAnnotation = 0;
    m_scalarBar = 0;
    for( int i = 0; i < 4; i++ )
    {
        m_patientOrientationTextActor[i] = 0;
    }
    m_sideRuler = 0;
    m_bottomRuler = 0;
    
    // CheckerBoard
    // el nombre de divisions per defecte, serà de 2, per simplificar
    m_divisions[0] = m_divisions[1] = m_divisions[2] = 2;
    
    // anotacions
    createAnnotations();
    createActions();    
    createTools();
    addActors();
    
    m_windowToImageFilter->SetInput( this->getRenderer()->GetRenderWindow() );
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
    if( m_pickedAxisActor )
        m_pickedAxisActor->Delete();
    m_textAnnotation->Delete();
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

void Q2DViewer::createTools()
{
    m_toolManager = new Q2DViewerToolManager( this );
    this->enableTools();
}

void Q2DViewer::createAnnotations()
{
    // escala de colors
    createScalarBar();
    // anotacions de l'orientació del pacient
    createOrientationAnnotations();
    // Llegenda amb informació del voxel
    createVoxelInformationCaption();
    // Marcadors d'escala
    createRulers();
    // actualització dels valors de les anotacions
    updateAnnotations();
}

void Q2DViewer::createVoxelInformationCaption()
{
    m_voxelInformationCaption = vtkCaptionActor2D::New();
    m_voxelInformationCaption->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->SetAttachmentPoint( m_currentCursorPosition );
    m_voxelInformationCaption->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->BorderOff();
    m_voxelInformationCaption->LeaderOn();
    m_voxelInformationCaption->ThreeDimensionalLeaderOff();
    m_voxelInformationCaption->GetProperty()->SetColor( 1.0 , 0 , 0 );
    m_voxelInformationCaption->SetPadding( 1 );
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
    // \TODO separar el textAnnotation en un altre mètode?
    // anotacions de texte que van als racons de la finestra
    m_textAnnotation = vtkCornerAnnotation::New();
    // informació de referència de la orientació del pacient
    for( int i = 0; i < 4; i++ )
    {
        m_patientOrientationTextActor[i] = vtkTextActor::New();
        m_patientOrientationTextActor[i]->ScaledTextOff();
        m_patientOrientationTextActor[i]->GetTextProperty()->SetFontSize( 12 );
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

void Q2DViewer::updateRulers()
{
    double *anchoredCoordinates = m_anchoredRulerCoordinates->GetComputedWorldValue( this->getRenderer() );
    switch( m_lastView )
    {
    case Axial:
        m_sideRuler->GetPositionCoordinate()->SetValue( anchoredCoordinates[0] , m_rulerExtent[3] , 0.0 );
        m_sideRuler->GetPosition2Coordinate()->SetValue( anchoredCoordinates[0] , m_rulerExtent[2] , 0.0 );
        m_sideRuler->SetRange( m_rulerExtent[3] , m_rulerExtent[2] );

        m_bottomRuler->GetPositionCoordinate()->SetValue( m_rulerExtent[1] , anchoredCoordinates[1]  , 0.0 );
        m_bottomRuler->GetPosition2Coordinate()->SetValue( m_rulerExtent[0] , anchoredCoordinates[1] , 0.0  );
        m_bottomRuler->SetRange( m_rulerExtent[1] , m_rulerExtent[0] );
    break;

    case Sagittal:
        m_sideRuler->GetPositionCoordinate()->SetValue( 0.0 , anchoredCoordinates[1] , m_rulerExtent[4] );
        m_sideRuler->GetPosition2Coordinate()->SetValue( 0.0 , anchoredCoordinates[1] , m_rulerExtent[5] );
        m_sideRuler->SetRange( m_rulerExtent[4] , m_rulerExtent[5] );
        
        m_bottomRuler->GetPositionCoordinate()->SetValue( 0.0 , m_rulerExtent[3] , anchoredCoordinates[2] );
        m_bottomRuler->GetPosition2Coordinate()->SetValue( 0.0 , m_rulerExtent[2] , anchoredCoordinates[2] );
        m_bottomRuler->SetRange( m_rulerExtent[3] , m_rulerExtent[2] );
    break;

    case Coronal:
        m_sideRuler->GetPositionCoordinate()->SetValue( anchoredCoordinates[0] , 0.0 , m_rulerExtent[4] );
        m_sideRuler->GetPosition2Coordinate()->SetValue( anchoredCoordinates[0] , 0.0 , m_rulerExtent[5] );
        m_sideRuler->SetRange( m_rulerExtent[4] , m_rulerExtent[5] );

        m_bottomRuler->GetPositionCoordinate()->SetValue( m_rulerExtent[1] , 0.0 , anchoredCoordinates[2] );
        m_bottomRuler->GetPosition2Coordinate()->SetValue( m_rulerExtent[0] , 0.0 , anchoredCoordinates[2] );
        m_bottomRuler->SetRange( m_rulerExtent[1] , m_rulerExtent[0] );
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
        vtkLookupTable *lookup = vtkLookupTable::New();
        double range[2];
        range[0] = m_viewer->GetColorLevel() - m_viewer->GetColorWindow()/2;
        range[1] = m_viewer->GetColorLevel() + m_viewer->GetColorWindow()/2;
        lookup->SetTableRange( range );
        lookup->SetSaturationRange( 0 , 0 );
        lookup->SetHueRange( 0 , 0 );
        lookup->SetValueRange( 0 , 1 );
        lookup->Build();
        m_scalarBar->SetLookupTable( lookup );
    }
    else
        DEBUG_LOG( "No hi ha cap volum assignat. No podem donar LUT a l'escala de colors" );
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
            m_patientOrientationTextActor[0]->SetInput( qPrintable( revertedList.at(0) ) );
            m_patientOrientationTextActor[2]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[1]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[3]->SetInput( qPrintable( revertedList.at(1) ) );
        }
        else if( m_lastView == Sagittal )
        {
            m_patientOrientationTextActor[0]->SetInput( qPrintable( revertedList.at(1) ) );
            m_patientOrientationTextActor[2]->SetInput( qPrintable( list.at(1) ) );
            m_patientOrientationTextActor[1]->SetInput( qPrintable( revertedList.at(2) ) );
            m_patientOrientationTextActor[3]->SetInput( qPrintable( list.at(2) ) );
        }
        else if( m_lastView == Coronal )
        {
            m_patientOrientationTextActor[0]->SetInput( qPrintable( revertedList.at(0) ) );
            m_patientOrientationTextActor[2]->SetInput( qPrintable( list.at(0) ) );
            m_patientOrientationTextActor[1]->SetInput( qPrintable( revertedList.at(2) ) );
            m_patientOrientationTextActor[3]->SetInput( qPrintable( list.at(2) ) );
        }
    }
    else
    {
        // la info no existeix
    }
}

void Q2DViewer::updateAnnotations()
{
    if( m_enabledAnnotations & Q2DViewer::PatientOrientationAnnotation )
    {
        displayPatientOrientationOn();
    }
    else
    {
        displayPatientOrientationOff();
    }

    if( m_enabledAnnotations & Q2DViewer::RulersAnnotation )
    {
        displayRulersOn();
    }
    else
    {
        displayRulersOff();
    }
    
    if( m_enabledAnnotations & Q2DViewer::WindowInformationAnnotation )
    {
        displayWindowInformationOn();
    }
    else
    {
        displayWindowInformationOff();
    }
}

void Q2DViewer::addActors()
{
    if( m_voxelInformationCaption )
        this->getRenderer()->AddActor( m_voxelInformationCaption );
    else
    {
        DEBUG_LOG( "No s'ha creat l'actor d'informació de voxel; no es pot afegir a l'escena" );
    }
    if( m_textAnnotation )
        this->getRenderer()->AddActor( m_textAnnotation );
    else
    {
        DEBUG_LOG( "No s'ha creat l'actor d'informació devolum; no es pot afegir a l'escena" );
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
    if( m_scalarBar )
        this->getRenderer()->AddActor2D( m_scalarBar );
    else
    {
        DEBUG_LOG( "No s'ha creat l'actor d'escala de colors; no es pot afegir a l'escena" );
    }
}

void Q2DViewer::initInformationText()
{
    // informació de llesca
    updateSliceAnnotation();
    // informació de la imatge: mides i window level
    updateWindowInformationAnnotation();
    // informació de la sèrie
    updateSerieInformationAnnotation();
    // nom del protocol
    updateProtocolNameAnnotation();
}

void Q2DViewer::updateSliceAnnotation()
{
    m_lowerLeftText = tr("Slice: %1/%2")
                .arg( m_currentSlice + 1 )
                .arg( m_viewer->GetSliceMax() + 1 );
    m_textAnnotation->SetText( 0 , m_lowerLeftText.toAscii() );
}

void Q2DViewer::updateWindowInformationAnnotation()
{
    m_upperLeftText = tr("Image Size: %1 x %2\nView Size: %3 x %4\nWW: %5 WL: %6 ")
                .arg( m_imageSizeInformation[0] )
                .arg( m_imageSizeInformation[1] )
                .arg( m_viewer->GetRenderWindow()->GetSize()[0] )
                .arg( m_viewer->GetRenderWindow()->GetSize()[1] )
                .arg( m_viewer->GetColorWindow() )
                .arg( m_viewer->GetColorLevel() );
    m_textAnnotation->SetText( 2 , m_upperLeftText.toAscii() );
}
    
void Q2DViewer::updateSerieInformationAnnotation()
{
    if( m_mainVolume )
    {
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
        
        m_upperRightText = tr("%1\n%2\n%3\nAcc:%4\n%5\n%6")
                    .arg( m_mainVolume->getVolumeSourceInformation()->getInstitutionName() )
                    .arg( m_mainVolume->getVolumeSourceInformation()->getPatientName() )
                    .arg( m_mainVolume->getVolumeSourceInformation()->getPatientID() )
                    .arg( m_mainVolume->getVolumeSourceInformation()->getAccessionNumber() )
                    .arg( studyDate )
                    .arg( studyTime );
        m_textAnnotation->SetText( 3 , m_upperRightText.toAscii() );
    }
    else
        DEBUG_LOG( "::updateSerieInformationAnnotation() : No s'ha donat cap input, no hi ha informació disponible" );
}

void Q2DViewer::updateProtocolNameAnnotation()
{
    if( m_mainVolume )
    {
        m_lowerRightText = tr("%1")
                        .arg( m_mainVolume->getVolumeSourceInformation()->getProtocolName() );
    
        m_textAnnotation->SetText( 1 , m_lowerRightText.toAscii() );
    }
    else
        DEBUG_LOG( "::updateProtocolNameAnnotation() : No s'ha donat cap input, no hi ha informació disponible" );
}

void Q2DViewer::displayInformationText( bool display )
{
    if( display )
    {
        m_textAnnotation->VisibilityOn();
        connect( this , SIGNAL( infoChanged() ) , this , SLOT( updateInformationText() ) );
    }
    else
    {
        m_textAnnotation->VisibilityOff();
        disconnect( this , SIGNAL( infoChanged() ) , this , SLOT( updateInformationText() ) );
    }
}

void Q2DViewer::displaySliceOn()
{
    m_textAnnotation->SetText( 0 , m_lowerLeftText.toAscii() );
}

void Q2DViewer::displaySliceOff()
{
    m_textAnnotation->SetText( 0 , "" );
}

void Q2DViewer::displayProtocolNameOn()
{
    m_textAnnotation->SetText( 1 , m_lowerRightText.toAscii() );
}

void Q2DViewer::displayProtocolNameOff()
{
    m_textAnnotation->SetText( 1 , "" );
}

void Q2DViewer::displayWindowInformationOn()
{
    m_textAnnotation->SetText( 2 , m_upperLeftText.toAscii() );
}

void Q2DViewer::displayWindowInformationOff()
{
    m_textAnnotation->SetText( 2 , "" );
}

void Q2DViewer::displaySerieInformationOn()
{
    m_textAnnotation->SetText( 3 , m_upperRightText.toAscii() );
}

void Q2DViewer::displaySerieInformationOff()
{
    m_textAnnotation->SetText( 3 , "" );
}

void Q2DViewer::displayRulersOn()
{
    m_sideRuler->VisibilityOn();
    m_bottomRuler->VisibilityOn();
}

void Q2DViewer::displayRulersOff()
{
    m_sideRuler->VisibilityOff();
    m_bottomRuler->VisibilityOff();
}

void Q2DViewer::displayRulersLabelsOn()
{
    m_sideRuler->LabelVisibilityOn();
    m_bottomRuler->LabelVisibilityOn();
}

void Q2DViewer::displayRulersLabelsOff()
{
    m_sideRuler->LabelVisibilityOff();
    m_bottomRuler->LabelVisibilityOff();
}

void Q2DViewer::displayPatientOrientationOn()
{
    for( int i = 0; i < 4; i++ )
        m_patientOrientationTextActor[i]->VisibilityOn();
}

void Q2DViewer::displayPatientOrientationOff()
{
    for( int i = 0; i < 4; i++ )
        m_patientOrientationTextActor[i]->VisibilityOff();
}

void Q2DViewer::displayScalarBarOn()
{
    m_scalarBar->VisibilityOn();
}

void Q2DViewer::displayScalarBarOff()
{
    m_scalarBar->VisibilityOff();
}

void Q2DViewer::updateVoxelInformation()
{   
    vtkRenderWindowInteractor* interactor = m_vtkWidget->GetRenderWindow()->GetInteractor();
    // agafem el punt que està apuntant el ratolí en aquell moment \TODO podríem passar-li el 4t parèmatre opcional (vtkPropCollection) per indicar que només agafi de l'ImageActor, però no sembla que suigui necessari realment i que si fa pick d'un altre actor 2D no passa res
    m_picker->PickProp( interactor->GetEventPosition()[0], interactor->GetEventPosition()[1], m_viewer->GetRenderer() );
    // calculem el pixel trobat
    double q[3], imageValue;    
    m_picker->GetPickPosition( q );
    //     this->m_modelPointFromCursor.setValues( q );
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
        m_voxelInformationCaption->VisibilityOn();
        m_voxelInformationCaption->SetAttachmentPoint( q );
        m_voxelInformationCaption->SetCaption( qPrintable( QString("(%1,%2,%3):%4").arg(m_currentCursorPosition[0],0,'f',2).arg(m_currentCursorPosition[1],0,'f',2).arg(m_currentCursorPosition[2],0,'f',2).arg(m_currentImageValue) ) );
    }
    this->getInteractor()->Render();
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

void Q2DViewer::setEnableTools( bool enable )
{
    if( enable )
        connect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
    else
        disconnect( this , SIGNAL( eventReceived(unsigned long) ) , m_toolManager , SLOT( forwardEvent(unsigned long) ) );
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
    // aquesta crida obliga a que hi hagi un input abans, sinó el pipeline del vtkImageViewer ens dóna error perquè no té cap actor creat
    //\TODO aquesta crida hauria d'anar aquí o només després del primer setInput?
    m_vtkWidget->SetRenderWindow( m_viewer->GetRenderWindow() );
    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetPicker( m_picker );
    m_viewer->SetupInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );
    
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
    updateRulers();
    updateScalarBar();
    initInformationText();

    // \TODO s'ha de cridar cada cop que posem dades noves o nomès el primer cop?
    setupInteraction();
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
    
    vtkImageCheckerboard *imageCheckerBoard = vtkImageCheckerboard::New();
    vtkImageBlend *blender;
    
    vtkImageRectilinearWipe *wipe = vtkImageRectilinearWipe::New();
    
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
        blender = vtkImageBlend::New();
        blender->SetInput(m_mainVolume->getVtkData());
        blender->AddInput(m_overlayVolume->getVtkData());
        blender->SetOpacity( 1, 0.5 );
        blender->SetOpacity( 2, 0.5 );
        m_viewer->SetInputConnection( blender->GetOutputPort() ); // li donem el blender com a input
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

void Q2DViewer::render()
{
    // si tenim dades
    if( m_mainVolume )
    {        
       // Això és necessari perquè la imatge es rescali a les mides de la finestreta
        m_viewer->GetRenderer()->ResetCamera();
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
    updateView();
}

void Q2DViewer::updateView()
{
    if( m_viewer->GetInput() )
    {
        switch( m_lastView )
        {
        case Axial:
            m_viewer->SetSliceOrientationToXY();
            vtkCamera *cam;
            cam = this->getRenderer() ? this->getRenderer()->GetActiveCamera() : NULL;
            if ( cam )
            {
                cam->SetFocalPoint(0,0,0);
                cam->SetViewUp(0,-1,0);
                cam->SetPosition(0,0,-1);
                this->getRenderer()->ResetCamera();
            }
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[1];
        break;
        
        case Sagittal:
            m_viewer->SetSliceOrientationToYZ();
            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[1];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;
    
        case Coronal:
            m_viewer->SetSliceOrientationToXZ();
            //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
            m_imageSizeInformation[0] = m_mainVolume->getDimensions()[0];
            m_imageSizeInformation[1] = m_mainVolume->getDimensions()[2];
        break;
    
        default:
        // podem posar en Axial o no fer res
            m_viewer->SetSliceOrientationToXY();
        break;
        }
        // cada cop que canviem de llesca posarem per defecte la llesca del mig d'aquella vista
        setSlice( m_viewer->GetSliceRange()[1]/2 );
        mapOrientationStringToAnnotation();
        updateWindowInformationAnnotation();
        updateRulers();
        this->getInteractor()->Render();
    }
    else
    {
        WARN_LOG( "Intentant canviar de vista sense haver donat un input abans..." );
    }
}

void Q2DViewer::setSlice( int value )
{
    if( value <= m_viewer->GetSliceRange()[1] && value >= m_viewer->GetSliceRange()[0] )
    {
        m_viewer->SetSlice( m_currentSlice );
        m_currentSlice = value;
        emit sliceChanged( m_currentSlice );
        updateSliceAnnotation();
        this->getInteractor()->Render();
    }
}

void Q2DViewer::resizeEvent( QResizeEvent *resize )
{
    updateWindowInformationAnnotation();
}

void Q2DViewer::setWindowLevel( double window , double level )
{
    if( m_mainVolume )
    {
        m_viewer->SetColorLevel( level );
        m_viewer->SetColorWindow( window );
        updateWindowInformationAnnotation();
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
        this->getInteractor()->Render();
        updateWindowLevelAnnotation();
    }
    else
    {
        DEBUG_LOG( "::resetWindowLevelToDefault() : No tenim input" );
    }
}

void Q2DViewer::updateWindowLevelAnnotation()
{
    updateWindowInformationAnnotation();
    emit windowLevelChanged( m_viewer->GetColorWindow() , m_viewer->GetColorLevel() );
    updateScalarBar();
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

};  // end namespace udg 

