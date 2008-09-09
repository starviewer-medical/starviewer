/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcardiac3dmprviewer.h"

//includes vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCellPicker.h>
#include <vtkProperty.h>
#include <vtkImagePlaneWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <QVTKWidget.h>
#include <vtkCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkLookupTable.h>

// Per crear la bounding box del model
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

// Per el marcador d'orientació
#include "q3dorientationmarker.h"

//includes propis
#include "volume.h"
#include "image.h"
#include "logging.h"

namespace udg {

QCardiac3DMPRViewer::QCardiac3DMPRViewer( QWidget *parent )
 : Q3DMPRViewer( parent )
{
    m_actualSubVolume = 0;
}

QCardiac3DMPRViewer::~QCardiac3DMPRViewer()
{
}

void QCardiac3DMPRViewer::createOutline()
{
    if( m_mainVolume )
    {
        // creem l'outline
        vtkOutlineFilter *outlineFilter = vtkOutlineFilter::New();
        outlineFilter->SetInput( m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData() );
        vtkPolyDataMapper *outlineMapper = vtkPolyDataMapper::New();
        outlineMapper->SetInput( outlineFilter->GetOutput() );
        m_outlineActor->SetMapper( outlineMapper );
    }
    else
    {
        DEBUG_LOG( "Intentant crear outline sense haver donat input abans" );
    }
}

void QCardiac3DMPRViewer::setInput( Volume *volume )
{
    m_mainVolume = volume->orderSlices();
    this->createOutline();
    // li proporcionem les dades als plans
    this->updatePlanesData();
    // ajustem els valors del window Level per defecte
    this->updateWindowLevelData();
    //li donem la orientació per defecte
    this->resetViewToAxial();
    render();
}

void QCardiac3DMPRViewer::updatePlanesData()
{
    if( m_mainVolume )
    {
        m_axialImagePlaneWidget->SetInput( m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData() );

        if( !m_axialResliced )
            m_axialResliced = new Volume( m_axialImagePlaneWidget->GetResliceOutput() );
        else
            m_axialResliced->setData( m_axialImagePlaneWidget->GetResliceOutput() );
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_axialResliced->setImages( m_mainVolume->getImages() );

        m_sagitalImagePlaneWidget->SetInput( m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData() );
        if( !m_sagitalResliced )
            m_sagitalResliced = new Volume( m_sagitalImagePlaneWidget->GetResliceOutput() );
        else
            m_sagitalResliced->setData( m_sagitalImagePlaneWidget->GetResliceOutput() );
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_sagitalResliced->setImages( m_mainVolume->getImages() );

        m_coronalImagePlaneWidget->SetInput( m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData() );
        if( !m_coronalResliced )
            m_coronalResliced = new Volume( m_coronalImagePlaneWidget->GetResliceOutput() );
        else
            m_coronalResliced->setData( m_coronalImagePlaneWidget->GetResliceOutput() );
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_coronalResliced->setImages( m_mainVolume->getImages() );
    }
    else
    {
        DEBUG_LOG( "No es poden inicialitzar les dades dels plans. No hi ha dades d'entrada" );
    }
}

void QCardiac3DMPRViewer::resetPlanes()
{
    if( m_mainVolume )
    {
        DEBUG_LOG("Reset Planes");
        int *size = m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData()->GetDimensions();
        DEBUG_LOG( QString("Mides: %1,%2,%3").arg(size[0]).arg(size[1]).arg(size[2]) );        m_axialImagePlaneWidget->SetPlaneOrientationToZAxes();
        m_axialImagePlaneWidget->SetSliceIndex(size[2]/2);

        m_sagitalImagePlaneWidget->SetPlaneOrientationToXAxes();
        m_sagitalImagePlaneWidget->SetSliceIndex(size[0]/2);

        m_coronalImagePlaneWidget->SetPlaneOrientationToYAxes();
        m_coronalImagePlaneWidget->SetSliceIndex(size[1]/2);

        if( m_axialPlaneVisible )
        {
            m_axialImagePlaneWidget->On();
        }
        if( m_sagitalPlaneVisible )
        {
            m_sagitalImagePlaneWidget->On();
        }
        if( m_coronalPlaneVisible )
        {
            m_coronalImagePlaneWidget->On();
        }
    }
}

void QCardiac3DMPRViewer::setSubVolume( int index )
{
    double point1_a[3], point2_a[3], orig_a[3];
    double point1_s[3], point2_s[3], orig_s[3];
    double point1_c[3], point2_c[3], orig_c[3];
    double wl[2],wl1[2],wl2[2];

    Volume * subVolume;

    m_axialImagePlaneWidget->GetPoint1( point1_a );
    m_axialImagePlaneWidget->GetPoint2( point2_a );
    m_axialImagePlaneWidget->GetOrigin( orig_a );

    m_sagitalImagePlaneWidget->GetPoint1( point1_s );
    m_sagitalImagePlaneWidget->GetPoint2( point2_s );
    m_sagitalImagePlaneWidget->GetOrigin( orig_s );

    m_coronalImagePlaneWidget->GetPoint1( point1_c );
    m_coronalImagePlaneWidget->GetPoint2( point2_c );
    m_coronalImagePlaneWidget->GetOrigin( orig_c );

    m_axialImagePlaneWidget->GetWindowLevel(wl);
    m_sagitalImagePlaneWidget->GetWindowLevel(wl1);
    m_coronalImagePlaneWidget->GetWindowLevel(wl2);

    int sagitalSliceIndex = m_sagitalImagePlaneWidget->GetSliceIndex();
    int coronalSliceIndex = m_coronalImagePlaneWidget->GetSliceIndex();
    int axialSliceIndex = m_axialImagePlaneWidget->GetSliceIndex();

    subVolume = m_mainVolume->getSubVolume( index );

    m_axialImagePlaneWidget->SetInput( subVolume->getVtkData() );
    m_sagitalImagePlaneWidget->SetInput( subVolume->getVtkData() );
    m_coronalImagePlaneWidget->SetInput( subVolume->getVtkData() );

    m_axialImagePlaneWidget->SetPoint1( point1_a );
    m_axialImagePlaneWidget->SetPoint2( point2_a );
    m_axialImagePlaneWidget->SetOrigin( orig_a );

    m_sagitalImagePlaneWidget->SetPoint1( point1_s );
    m_sagitalImagePlaneWidget->SetPoint2( point2_s );
    m_sagitalImagePlaneWidget->SetOrigin( orig_s );

    m_coronalImagePlaneWidget->SetPoint1( point1_c );
    m_coronalImagePlaneWidget->SetPoint2( point2_c );
    m_coronalImagePlaneWidget->SetOrigin( orig_c );

    m_axialImagePlaneWidget->SetSliceIndex(axialSliceIndex);
    m_sagitalImagePlaneWidget->SetSliceIndex(sagitalSliceIndex);
    m_coronalImagePlaneWidget->SetSliceIndex(coronalSliceIndex);

    m_axialImagePlaneWidget->UpdatePlacement();
    m_sagitalImagePlaneWidget->UpdatePlacement();
    m_coronalImagePlaneWidget->UpdatePlacement();

    m_axialImagePlaneWidget->SetWindowLevel(wl[0], wl[1]);
    m_sagitalImagePlaneWidget->SetWindowLevel(wl1[0], wl1[1]);
    m_coronalImagePlaneWidget->SetWindowLevel(wl2[0], wl2[1]);
}

}; // end namespace udg
