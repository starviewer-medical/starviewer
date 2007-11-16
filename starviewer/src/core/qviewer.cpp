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
 : QWidget( parent ), m_contextMenuActive(true)
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
        this->refresh();
    }
    else
        DEBUG_LOG( "::zoom(double factor): El renderer és NUL!" );
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
        setInput( series->getFirstVolume() );
        render();
        emit volumeChanged( series->getFirstVolume() );
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
