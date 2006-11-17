/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qviewer.h"
#include "volume.h"

// Qt
#include <QHBoxLayout>

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

namespace udg {

QViewer::QViewer( QWidget *parent )
 : QWidget( parent )
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

    m_modelPointFromCursor.setValues( -1, -1 , -1 );

    // 2x buffer
    m_vtkWidget->GetRenderWindow()->DoubleBufferOn();

    m_currentCursorPosition[0] = 0;
    m_currentCursorPosition[1] = 0;
    m_currentCursorPosition[2] = 0;

    m_currentImageValue = -1;

    m_windowToImageFilter = vtkWindowToImageFilter::New();
    this->setMouseTracking( true );

    m_vtkQtConnections = 0;
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

vtkRenderWindow *QViewer::getRenderWindow()
{
    return m_vtkWidget->GetRenderWindow();
}

void QViewer::eventHandler( vtkObject *obj, unsigned long event, void *client_data, void *call_data, vtkCommand *command )
{
    emit eventReceived( event );
}

void QViewer::computeDisplayToWorld( vtkRenderer *renderer , double x , double y , double z , double worldPoint[3] )
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

bool QViewer::saveGrabbedViews( const char *baseName , FileType extension )
{
    if( !m_grabList.empty() )
    {
        vtkImageWriter *writer;
        char pattern[12] = "%s-%d.";
        switch( extension )
        {
        case PNG:
            writer = vtkPNGWriter::New();
            strcat( pattern , "png" );
        break;

        case JPEG:
            writer = vtkJPEGWriter::New();
            strcat( pattern , "jpg" );
        break;

        // \TODO el format tiff fa petar al desar, mirar si és problema de compatibilitat del sistema o de les pròpies vtk
        case TIFF:
            writer = vtkTIFFWriter::New();
            strcat( pattern , "tif" );
        break;

        case PNM:
            writer = vtkPNMWriter::New();
            strcat( pattern , "pnm" );
        break;

        case BMP:
            writer = vtkBMPWriter::New();
            strcat( pattern , "bmp" );
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
            sprintf( fileName , pattern , baseName , i );
            writer->SetFileName( fileName );
            writer->Write();
            i++;
        }
        return true;
    }
    else
        return false;
}

void QViewer::grabCurrentView()
{
    m_windowToImageFilter->Update();
    m_windowToImageFilter->Modified();

    vtkImageData *image = vtkImageData::New();
    image->ShallowCopy( m_windowToImageFilter->GetOutput() );
    m_grabList.push_back( image );
}

};  // end namespace udg
