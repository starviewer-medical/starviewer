/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewer.h"
#include "volume.h"

// include's qt
#include <QResizeEvent>
#include <QSize>
#include <QMenu>
#include <QAction>

// Tools
#include "distancetool.h"

// include's vtk
#include <QVTKWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageViewer2.h>
#include <vtkImageCheckerboard.h>
#include <vtkImageBlend.h> // per composar les imatges
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
#include <vtkImageRectilinearWipe.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleUser.h>
#include <vtkCornerAnnotation.h>
// interacció
#include <vtkCell.h>
#include <vtkPointData.h>


namespace udg {


Q2DViewer::Q2DViewer( QWidget *parent )
 : QViewer( parent )
{
    m_lastView = None; 
    m_viewer = vtkImageViewer2::New();
    m_currentSlice = 0;
    m_overlay = CheckerBoard; // per defecte
    m_overlayVolume = 0;

// paràmetres específics
    
    // CheckerBoard
    // el nombre de divisions per defecte, serà de 2, per simplificar
    m_divisions[0] = m_divisions[1] = m_divisions[2] = 2;
    
    // preparem el picker
    m_cellPicker = vtkCellPicker::New();
    m_cellPicker->SetTolerance( 0.005 );
    
    // ANOTACIONS
    m_textAnnotation = vtkCornerAnnotation::New();
    initInformationText();

    m_currentTool = NoTool;
    
    createActions();    
    createTools();
    
    setupInteraction();

}

Q2DViewer::~Q2DViewer()
{
}

vtkRenderer *Q2DViewer::getRenderer()
{
    if( m_viewer )
        return m_viewer->GetRenderer();
    else
        return NULL;
}

vtkRenderWindowInteractor *Q2DViewer::getInteractor()
{
    return m_vtkWidget->GetRenderWindow()->GetInteractor();
}

void Q2DViewer::createActions()
{
    m_resetAction = new QAction( this );
    m_resetAction->setText(tr("&Reset"));
    m_resetAction->setShortcut( tr("Ctrl+R") );
    m_resetAction->setStatusTip(tr("Reset initial parameters"));
    connect( m_resetAction, SIGNAL( activated() ), this, SLOT( reset()) );
}

void Q2DViewer::createTools()
{
    m_distanceTool = new DistanceTool( this );
}


void Q2DViewer::initInformationText()
{
/*    m_formatedUpperLeftString = tr("Image Size: %d x %d\nView Size: %d x %d\nX: %4g px Y: %4g px Value: %g\nWW: %.1f WL: %.1f ");
    
    m_formatedUpperLeftOffImageString = tr("Image Size: %d x %d\nView Size: %d x %d\nWW: %.1f WL: %.1f ");
        
    m_formatedUpperRightString = "%s";
    m_formatedLowerLeftString = tr("Slice: %d/%d\nZoom: XXX%%  Angle: XXX\nThickness: XXX mm Location: XXX ");
    m_formatedLowerRightString = "%s";*/
        
        
    m_upperLeftText = tr("No Info");
    m_upperRightText = tr("No Info");
    m_lowerLeftText = tr("No Info");
    m_lowerRightText = tr("No Info");
    
    m_textAnnotation->SetImageActor( m_viewer->GetImageActor() );
    m_textAnnotation->SetWindowLevel( m_viewer->GetWindowLevel() );
    m_textAnnotation->ShowSliceAndImageOn();

    m_viewer->GetRenderer()->AddActor2D( m_textAnnotation );

    // \FIXME problema amb aquest connect, quan es dispara el signal no crida a la funció que toca i peta ?problema de qt4 +qt3¿
//     connect( this , SIGNAL( infoChanged() ) , this , SLOT( updateInformationText() ) );

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

void Q2DViewer::anyEvent()
{
    // std::cout << "any event " << std::endl; 
}

void Q2DViewer::updateInformationText()
{
    int width, height , depth;
    switch( m_lastView )
    {
    case Axial:
    //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
        width = m_mainVolume->getDimensions()[0];
        height = m_mainVolume->getDimensions()[1];
        depth = m_mainVolume->getDimensions()[2];
    break;
    
    case Sagittal:
        //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
        width = m_mainVolume->getDimensions()[1];
        height = m_mainVolume->getDimensions()[2];
        depth = m_mainVolume->getDimensions()[0];
    break;
    
    case Coronal:
        //\TODO hauria de ser a partir de main_volume o a partir de l'output del viewer
        width = m_mainVolume->getDimensions()[0];
        height = m_mainVolume->getDimensions()[2];
        depth = m_mainVolume->getDimensions()[1];
    break;
    
    case None:
    break;
    
    default:
    break;
    }
        
    if( m_currentCursorPosition[0] == -1 )
    {    
//         m_upperLeftText.sprintf( m_formatedUpperLeftOffImageString , width , height , m_viewer->GetRenderWindow()->GetSize()[0] , m_viewer->GetRenderWindow()->GetSize()[1] , m_viewer->GetColorWindow() , m_viewer->GetColorLevel() );
        m_upperLeftText = tr("Image Size: %1 x %2\nView Size: %3 x %4\nOff Image\nWW: %5 WL: %6 ")
                .arg( width )
                .arg( height )
                .arg( m_viewer->GetRenderWindow()->GetSize()[0] )
                .arg( m_viewer->GetRenderWindow()->GetSize()[1] )
                .arg( m_viewer->GetColorWindow() )
                .arg( m_viewer->GetColorLevel() );
    }
    else
    {
//         m_upperLeftText.sprintf( m_formatedUpperLeftString , width , height , m_viewer->GetRenderWindow()->GetSize()[0] , m_viewer->GetRenderWindow()->GetSize()[1], m_currentCursorPosition[0] , m_currentCursorPosition[1], m_currentImageValue , m_viewer->GetColorWindow() , m_viewer->GetColorLevel() );

        m_upperLeftText = tr("Image Size: %1 x %2\nView Size: %3 x %4\nX: %5 px Y: %6 px Value: %7\nWW: %8 WL: %9 ")
                .arg( width )
                .arg( height )
                .arg( m_viewer->GetRenderWindow()->GetSize()[0] )
                .arg( m_viewer->GetRenderWindow()->GetSize()[1] )
                .arg( m_currentCursorPosition[0] )
                .arg( m_currentCursorPosition[1] )
                .arg( m_currentImageValue )
                .arg( m_viewer->GetColorWindow() )
                .arg( m_viewer->GetColorLevel() );
    }
//     m_lowerLeftText.sprintf( m_formatedLowerLeftString , m_currentSlice , depth );
    m_lowerLeftText = tr("Slice: %1/%2\nZoom: XXX%%  Angle: XXX\nThickness: XXX mm Location: XXX ")
                .arg( m_currentSlice )
                .arg( depth );
    
    m_upperRightText = tr("No info");
    m_lowerRightText = tr("No info");
    
    m_textAnnotation->SetText( 0 , m_lowerLeftText );
    m_textAnnotation->SetText( 2 , m_upperLeftText );
}

void Q2DViewer::onMouseMove()
{
    vtkRenderWindowInteractor* interactor = m_vtkWidget->GetRenderWindow()->GetInteractor();
    // agafem el punt que està apuntant el ratolí en aquell moment
    interactor->GetPicker()->Pick( interactor->GetEventPosition()[0], 
                interactor->GetEventPosition()[1], 
                m_viewer->GetSlice(), 
                m_viewer->GetRenderer()
                );
    
    // calculem el pixel trobat
    double tolerance;
    int subId;
    double pointCoordinates[3], weights[8];
    double q[3], imageValue;
    
    interactor->GetPicker()->GetPickPosition( q );

    this->m_modelPointFromCursor.setValues( q );

    vtkPointData *pointData = m_mainVolume->getVtkData()->GetPointData();
    vtkPointData* outPointData = vtkPointData::New();
    outPointData->InterpolateAllocate( pointData , 1 , 1 );

    // Use tolerance as a function of size of source data
    tolerance = m_mainVolume->getVtkData()->GetLength();
    tolerance = tolerance ? tolerance*tolerance / 1000.0 : 0.001;

    // Find the cell that contains q and get it
    vtkCell *cell = m_mainVolume->getVtkData()->FindAndGetCell( q , NULL , -1 , tolerance , subId , pointCoordinates , weights );
    int found = 0;
    if ( cell )
    {
        // Interpolate the point data
        outPointData->InterpolatePoint( pointData , 0 , cell->PointIds , weights );
        imageValue = outPointData->GetScalars()->GetTuple1(0);
        found = 1;
    }
    outPointData->Delete();
//     std::cout << "EEEEEEEEEEEEEEEEI" << std::endl;
    if( found )
    {
        updateCursor( q[0], q[1], q[2], imageValue );
//         std::cout << "Update cursor with " << q[0] << "," << q[1] << "," << q[2] << " :: " << imageValue << std::endl;
    }
    else
        updateCursor( -1, -1, -1, -1 );
    
    // actualitzem la resta de valors \TODO: hauria d'haver alguna manera de connectar l'event del window level amb algun slot
    
    updateWindowLevel();
    interactor->Render();

}

void Q2DViewer::eventHandler( vtkObject *obj, unsigned long event, void *client_data, void * call_data, vtkCommand *command )
{
    // el primer que s'hauria de fer és executar l'acció que es faci en aquell estat indistintament de la tool com és el mostrar en la pantalla el valor del pixel actual
   anyEvent();
    // fer el que calgui per cada tipus d'event
    switch( event )
    {
    
    case vtkCommand::MouseMoveEvent:
        onMouseMove();
    break;
    
    default:
    break;
    
    }
    
    // ara de mentres serà així segons la tool activa analitzarem uns events o uns altres
    switch( m_currentTool )
    {
    // ----------------------------------------------------
    // ZOOM
    // ----------------------------------------------------
    case Zoom:
    break;
    
    // ----------------------------------------------------
    // PICK
    // ----------------------------------------------------
    case Pick:
        // estats?
        // Si l'event és botó esquerre pressionat -> escollir seed candidata : mostrar valor en pantalla
        // ""                            ""        + MouseMove : mostrar valor en pantalla
        // ""                          released -> confirmar seed -> posar-la a la llista , afegir punt en pantalla
    break;
    
    // ----------------------------------------------------
    // DISTANCE
    // ----------------------------------------------------
    case Distance:   
        m_distanceTool->dispatchEvent( DistanceTool::vtkCommandEventToToolEvent( event ) );
//         m_distanceTool->dispatchEvent( event );
    break;
    
    // ----------------------------------------------------
    // CURSOR : mostra cursor sobre la imatge
    // ----------------------------------------------------
    case Cursor:
        // estats?
        // si mouseMove , mostrar cursor sobre el punt en el que estem
    break;
    
    // ----------------------------------------------------
    // DEFAULT
    // ----------------------------------------------------
    default:
    break;
    }
 
}


void Q2DViewer::contextMenuRelease( vtkObject* object , unsigned long event, void *client_data, vtkCommand *command )
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

void Q2DViewer::updateWindowLevel( )
{
    emit infoChanged();
}

void Q2DViewer::setupInteraction()
{   
    // configurem l'Image Viewer i el qvtkWidget
    m_vtkWidget->SetRenderWindow( m_viewer->GetRenderWindow() );
    m_viewer->SetupInteractor( m_vtkWidget->GetRenderWindow()->GetInteractor() );

    m_vtkWidget->GetRenderWindow()->GetInteractor()->SetPicker( m_cellPicker );
    
    m_vtkQtConnections = vtkEventQtSlotConnect::New(); 
    // menú contextual
//     m_vtkQtConnections->Connect( m_vtkWidget->GetRenderWindow()->GetInteractor(),
//                       QVTKWidget::ContextMenuEvent,//vtkCommand::RightButtonPressEvent,
//                        this,
//                        SLOT( contextMenuRelease(vtkObject*,unsigned long,void*, vtkCommand *) ) );

    // despatxa qualsevol event-> tools                       
    m_vtkQtConnections->Connect( m_vtkWidget->GetRenderWindow()->GetInteractor(), vtkCommand::AnyEvent, this, SLOT( eventHandler(vtkObject*,unsigned long,void *,void *, vtkCommand *) ) );
    
//     displayTextLabels();
//     setupLookupTable();
//     displayScalarBar();
}

void Q2DViewer::setInput( Volume* volume )
{
    m_mainVolume = volume;    
    m_viewer->SetInput( m_mainVolume->getVtkData() );
}

void Q2DViewer::setOverlayInput( Volume* volume )
{
    m_overlayVolume = volume;
    
    vtkImageCheckerboard* imageCheckerBoard = vtkImageCheckerboard::New();
    vtkLookupTable* table = vtkLookupTable::New();
    vtkImageMapToColors* mapColors = vtkImageMapToColors::New();
    vtkImageBlend* blender = vtkImageBlend::New();
    
    vtkImageRectilinearWipe *wipe = vtkImageRectilinearWipe::New();
    
    switch( m_overlay )
    {
    case CheckerBoard:
        
        imageCheckerBoard->SetInput1( m_mainVolume->getVtkData() );
        imageCheckerBoard->SetInput2( m_overlayVolume->getVtkData() );
        imageCheckerBoard->SetNumberOfDivisions( m_divisions );
        // actualitzem el viewer
        m_viewer->SetInputConnection( imageCheckerBoard->GetOutputPort() ); // li donem el m_imageCheckerboard com a input
    break;
    
    case Blend:
        // first build the lookup table
        
        table->SetValueRange(0,1);
        table->SetSaturationRange(0,0);
        table->SetAlphaRange(1,1);
        table->SetRampToLinear();
        table->Build();
        //  set alpha of first value to 0 instead of 1
        table->SetTableValue(0, 0,0,0,0);
        // 
        mapColors->SetLookupTable( table );
        mapColors->SetInput( m_mainVolume->getVtkData() );
    
        // creem el blender
        blender->SetInput( 0 , m_mainVolume->getVtkData()/*mapColors->GetOutput()*/ );
        blender->SetInput( 1 , m_overlayVolume->getVtkData() );
        blender->SetOpacity( 0, 0.5 );
        blender->SetOpacity( 1, 0.5 );
    // actualitzem el viewer
        m_viewer->SetInputConnection( blender->GetOutputPort() ); // li donem el blender com a input
    break;
    
    case RectilinearWipe:
        wipe->SetInput( 0 , m_mainVolume->getVtkData() );
        wipe->SetInput( 1 , m_overlayVolume->getVtkData() );
        wipe->SetPosition(20,20);
        wipe->SetWipeToUpperLeft();        
        m_viewer->SetInput( wipe->GetOutput() );
    break;    
    
    }
}

void Q2DViewer::render()
{
    // si tenim dades
    if( m_mainVolume )
    {        
        // li donem el window/level correcte
        resetWindowLevel();    
        // Això és necessari perquè la imatge es rescali a les mides de la finestreta
        m_viewer->GetRenderer()->ResetCamera();
        updateView();
    }
    // mostrar error/avís si no hi ha dades per visualitzar?
}

void Q2DViewer::setView( ViewType view )
{    
    m_lastView = view;
}

void Q2DViewer::updateView()
{
    switch( m_lastView )
    {
    case Axial:
        m_viewer->SetSliceOrientationToXY();        
    break;
    case Sagittal:
        m_viewer->SetSliceOrientationToYZ();
    break;
    case Coronal:
        m_viewer->SetSliceOrientationToXZ();
    break;
    default:
    // podem posar en Axial o no fer res
        m_viewer->SetSliceOrientationToXY();
    break;
    }
    // cada cop que canviem de llesca posarem per defecte la llesca del mig d'aquella vista
    setSlice( m_viewer->GetSliceRange()[1]/2 );
    // també farem update del nombre de llesques
    emit infoChanged();
}

void Q2DViewer::setSlice( int value )
{
    m_currentSlice = value;
    emit sliceChanged( m_currentSlice );

    if( m_currentSlice <= m_viewer->GetSliceRange()[1] && m_currentSlice >= m_viewer->GetSliceRange()[0] )
    {
        m_viewer->SetSlice( m_currentSlice );
    }
    emit infoChanged();
    
}

void Q2DViewer::resizeEvent( QResizeEvent *resize )
{
    emit infoChanged();
}

void Q2DViewer::resetWindowLevel()
{
// això ens dóna un level/level "maco" per defecte
    if( m_mainVolume )
    {
        double * range = m_mainVolume->getVtkData()->GetScalarRange();
        double window = fabs(range[1] - range[0]);
        double level = ( range[1] + range[0] )/ 2.0;
        m_viewer->SetColorLevel( level );
        m_viewer->SetColorWindow( window );
        
        emit infoChanged();
    }
    // mostrar avís/error si no hi ha volum?
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

};  // end namespace udg {
