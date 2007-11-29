/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qviewer.h"
#include "volume.h"
#include "series.h"
#include "toolproxy.h"
#include "patientbrowsermenu.h"

//TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

// Qt
#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMessageBox>

// include's vtk
#include <QVTKWidget.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkBMPWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkEventQtSlotConnect.h>
// necessari pel zoom
#include <vtkInteractorStyle.h>
#include <vtkCamera.h>

namespace udg {

QViewer::QViewer( QWidget *parent )
 : QWidget( parent ), m_contextMenuActive(true), m_mouseHasMoved(false)
{
    m_vtkWidget = new QVTKWidget( this );

    //Afegim el layout
    QHBoxLayout* viewerLayout = new QHBoxLayout( this );
    viewerLayout->setSpacing( 0 );
    viewerLayout->setMargin( 0 );
    viewerLayout->addWidget( m_vtkWidget );

    // això ho fem perquè la finestra buida quedi en negre en un principi
    m_vtkWidget->GetRenderWindow()->Render();

    // inicialitzem el punter del volum
    m_mainVolume = 0;

    // 2x buffer
    m_vtkWidget->GetRenderWindow()->DoubleBufferOn();

    m_windowToImageFilter = vtkWindowToImageFilter::New();
    this->setMouseTracking( true );

    m_vtkQtConnections = 0;
    m_toolProxy = new ToolProxy(this);
    connect( this, SIGNAL(eventReceived(unsigned long)), m_toolProxy, SLOT(forwardEvent(unsigned long)) );
}

QViewer::~QViewer()
{
    m_windowToImageFilter->Delete();
    delete m_vtkWidget;
}

vtkRenderWindowInteractor *QViewer::getInteractor()
{
    return m_vtkWidget->GetRenderWindow()->GetInteractor();
}

vtkInteractorStyle *QViewer::getInteractorStyle()
{
    return vtkInteractorStyle::SafeDownCast( this->getInteractor()->GetInteractorStyle() );
}

vtkRenderWindow *QViewer::getRenderWindow()
{
    return m_vtkWidget->GetRenderWindow();
}

ToolProxy *QViewer::getToolProxy() const
{
    return m_toolProxy;
}

void QViewer::eventHandler( vtkObject *obj, unsigned long event, void *client_data, void *call_data, vtkCommand *command )
{
    // quan la finestra sigui "seleccionada" s'emetrà un senyal indicant-ho. Entenem seleccionada quan s'ha clicat o mogut la rodeta per sobre del visor. \TODO ara resulta ineficient perquè un cop seleccionat no caldria re-enviar aquesta senyal. Cal millorar el sistema
    switch( event )
    {
    case QVTKWidget::ContextMenuEvent:
    case vtkCommand::LeftButtonPressEvent:
    case vtkCommand::RightButtonPressEvent:
    case vtkCommand::MiddleButtonPressEvent:
    case vtkCommand::MouseWheelForwardEvent:
    case vtkCommand::MouseWheelBackwardEvent:
        m_mouseHasMoved = false;
        emit selected();
    break;

    case vtkCommand::MouseMoveEvent:
        m_mouseHasMoved = true;
    break;

    case vtkCommand::RightButtonReleaseEvent:
        if( !m_mouseHasMoved )
            contextMenuRelease();
    break;

    }
    emit eventReceived( event );
}

#ifdef VTK_QT_5_0_SUPPORT
void QViewer::eventHandler( vtkObject * obj, unsigned long event, void * client_data, vtkCommand * command )
{
    this->eventHandler(obj, event, client_data, NULL, command);
}
#endif

void QViewer::computeDisplayToWorld( vtkRenderer *renderer , double x , double y , double z , double worldPoint[4] )
{
    if( renderer )
    {
        renderer->SetDisplayPoint( x, y, z );
        renderer->DisplayToWorld();
        renderer->GetWorldPoint( worldPoint );
        if ( worldPoint[3] )
        {
            worldPoint[0] /= worldPoint[3];
            worldPoint[1] /= worldPoint[3];
            worldPoint[2] /= worldPoint[3];
            worldPoint[3] = 1.0;
        }
    }
}

void QViewer::computeWorldToDisplay( vtkRenderer *renderer , double x , double y , double z , double displayPoint[3] )
{
    if( renderer )
    {
        renderer->SetWorldPoint(x, y, z, 1.0);
        renderer->WorldToDisplay();
        renderer->GetDisplayPoint( displayPoint );
    }
}

bool QViewer::saveGrabbedViews( QString baseName , FileType extension )
{
    if( !m_grabList.empty() )
    {
        vtkImageWriter *writer;
        QString fileType;
        switch( extension )
        {
        case PNG:
            writer = vtkPNGWriter::New();
            fileType = "png";
        break;

        case JPEG:
            writer = vtkJPEGWriter::New();
            fileType = "jpg";
        break;

        // \TODO el format tiff fa petar al desar, mirar si és problema de compatibilitat del sistema o de les pròpies vtk
        case TIFF:
            writer = vtkTIFFWriter::New();
            fileType = "tif";
        break;

        case PNM:
            writer = vtkPNMWriter::New();
            fileType = "pnm";
        break;

        case BMP:
            writer = vtkBMPWriter::New();
            fileType = "bmp";
        break;

        case DICOM:
        break;

        case META:
        break;
        }
        char fileName[128];
        int i = 0;
        for( m_grabListIterator = m_grabList.begin(); m_grabListIterator != m_grabList.end(); m_grabListIterator++ )
        {
            writer->SetInput( *m_grabListIterator );
            writer->SetFileName( qPrintable( QString("%1-%2.%3").arg( baseName ).arg( i ).arg( fileType ) ) );
            writer->Write();
            i++;
        }
        return true;
    }
    else
        return false;
}

void QViewer::refresh()
{
    this->getInteractor()->Render();
}

void QViewer::zoom( double factor )
{
    // TODO potser caldria una comprovació de seguretat pel que torna cadascuna d'aquestes crides
    vtkRenderer *renderer = this->getInteractor()->GetInteractorStyle()->GetCurrentRenderer();
    if( renderer )
    {
        // codi extret de void vtkInteractorStyleTrackballCamera::Dolly(double factor)
        vtkCamera *camera = renderer->GetActiveCamera();
        if ( camera->GetParallelProjection() )
        {
            camera->SetParallelScale(camera->GetParallelScale() / factor );
        }
        else
        {
            camera->Dolly(factor);
            if( vtkInteractorStyle::SafeDownCast( this->getInteractor()->GetInteractorStyle() )->GetAutoAdjustCameraClippingRange() )
            {
                // TODO en principi sempre ens interessarà fer això? ens podriem enstalviar l'if??
                renderer->ResetCameraClippingRange();
            }
        }
        if ( this->getInteractor()->GetLightFollowCamera() )
        {
            renderer->UpdateLightsGeometryToFollowCamera();
        }

        emit cameraChanged();
        emit zoomFactorChanged( factor );
        this->refresh();
    }
    else
        DEBUG_LOG( "::zoom(double factor): El renderer és NUL!" );
}

void QViewer::pan( double motionVector[3] )
{
    double viewFocus[4], viewPoint[3];

    vtkRenderer *renderer = this->getInteractor()->GetInteractorStyle()->GetCurrentRenderer();
    vtkCamera *camera = renderer->GetActiveCamera();

    camera->GetFocalPoint( viewFocus );
    camera->GetPosition( viewPoint );
    camera->SetFocalPoint( motionVector[0] + viewFocus[0],
                            motionVector[1] + viewFocus[1],
                            motionVector[2] + viewFocus[2] );

    camera->SetPosition( motionVector[0] + viewPoint[0],
                        motionVector[1] + viewPoint[1],
                        motionVector[2] + viewPoint[2] );

    if( this->getInteractor()->GetLightFollowCamera() )
    {
        renderer->UpdateLightsGeometryToFollowCamera();
    }
    emit cameraChanged();
    emit panChanged( motionVector );
    this->refresh();
}

void QViewer::scaleToFit( double topLeftX, double topLeftY, double bottomRightX, double bottomRightY )
{
    if( !m_mainVolume )
        return;

    double width, height;
    width = fabs( topLeftX - bottomRightX );
    height = fabs( topLeftY - bottomRightY );

    int *size = this->getRenderer()->GetSize();
    int *rendererOrigin = this->getRenderer()->GetOrigin();
    vtkCamera *camera = this->getRenderer()->GetActiveCamera();

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

    double position[3], focalPoint[3];
    camera->GetPosition( position );
    camera->GetFocalPoint( focalPoint );

    position[0] += translation[0];
    position[1] += translation[1];
    position[2] += translation[2];
    focalPoint[0] += translation[0];
    focalPoint[1] += translation[1];
    focalPoint[2] += translation[2];

    camera->SetPosition( position );
    camera->SetFocalPoint( focalPoint );

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
        this->zoom( size[0] / (float)width );
    else
        this->zoom( size[1] / (float)height );

    this->getRenderer()->ResetCameraClippingRange();
}

void QViewer::grabCurrentView()
{
    m_windowToImageFilter->Update();
    m_windowToImageFilter->Modified();

    vtkImageData *image = vtkImageData::New();
    image->ShallowCopy( m_windowToImageFilter->GetOutput() );
    m_grabList.push_back( image );
}

void QViewer::setSeries(Series *series)
{
    QString modality = series->getModality();
    if( modality == "KO" || modality == "PR" || modality == "SR" )
    {
        QMessageBox::information( this , tr( "Viewer" ) , tr( "The selected item is not a valid image format" ) );
    }
    else
    {
        if( series->getFirstVolume() != m_mainVolume )
        {
            setInput( series->getFirstVolume() );
            render();
            emit volumeChanged( series->getFirstVolume() );
        }
    }
}

void QViewer::enableContextMenu()
{
    m_contextMenuActive = true;
}

void QViewer::disableContextMenu()
{
    m_contextMenuActive = false;
}

void QViewer::contextMenuRelease()
{
    // Extret dels exemples de vtkEventQtSlotConnect

    // Obtenim la posició de l'event
    int eventPosition[2];
    this->getInteractor()->GetEventPosition( eventPosition );
    int* size = this->getInteractor()->GetSize();
    // remember to flip y
    QPoint point = QPoint( eventPosition[0], size[1]-eventPosition[1] );

    // map to global
    QPoint globalPoint = this->mapToGlobal( point );
//     emit showContextMenu( globalPoint );
    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point, globalPoint));
}

void QViewer::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_contextMenuActive)
    {
        PatientBrowserMenu *patientMenu = new PatientBrowserMenu(this);
        patientMenu->setAttribute(Qt::WA_DeleteOnClose);
        patientMenu->setPatient( QApplicationMainWindow::getActiveApplicationMainWindow()->getCurrentPatient() );

        connect(patientMenu, SIGNAL( selectedSeries(Series*) ), this, SLOT( setSeries(Series*) ));

        QString seriesUID;
        if( m_mainVolume )
            seriesUID = m_mainVolume->getSeries()->getInstanceUID();
        patientMenu->popup( event->globalPos(), seriesUID  ); //->globalPos() ?

    }
}

};  // end namespace udg
