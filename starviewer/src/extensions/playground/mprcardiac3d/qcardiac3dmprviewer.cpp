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
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyle.h>
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
#include "volumesourceinformation.h"
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
    m_mainVolume = volume->orderSlices( 1 );
    this->createOutline();
    // li proporcionem les dades als plans
    this->updatePlanesData();
    // ajustem els valors del window Level per defecte
    this->initializeWindowLevel();
    //li donem la orientació per defecte
    this->resetViewToAxial();
    render();
}

void QCardiac3DMPRViewer::updatePlanesData()
{
    if( m_mainVolume )
    {
        cout << "inici" << endl;
        m_axialImagePlaneWidget->SetInput( m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData() );
        
        if( !m_axialResliced )
        {
            m_axialResliced = new Volume( m_axialImagePlaneWidget->GetResliceOutput() );
        }
        else
            m_axialResliced->setData( m_axialImagePlaneWidget->GetResliceOutput() );
        m_axialResliced->setVolumeSourceInformation( m_mainVolume->getVolumeSourceInformation() );

        m_sagitalImagePlaneWidget->SetInput( m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData() );
        if( !m_sagitalResliced )
        {
            m_sagitalResliced = new Volume( m_sagitalImagePlaneWidget->GetResliceOutput() );
        }
        else
            m_sagitalResliced->setData( m_sagitalImagePlaneWidget->GetResliceOutput() );
        m_sagitalResliced->setVolumeSourceInformation( m_mainVolume->getVolumeSourceInformation() );

        m_coronalImagePlaneWidget->SetInput( m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData() );
        if( !m_coronalResliced )
        {
            m_coronalResliced = new Volume( m_coronalImagePlaneWidget->GetResliceOutput() );
        }
        else
            m_coronalResliced->setData( m_coronalImagePlaneWidget->GetResliceOutput() );
        m_coronalResliced->setVolumeSourceInformation( m_mainVolume->getVolumeSourceInformation() );
        cout << "fi" << endl;
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
        cout << "reset Planes " << endl;
        int *size = m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData()->GetDimensions();
        cout << "Mides: "<< size[0] << "," << size[1] << "," << size[2] << endl;

        m_axialImagePlaneWidget->SetPlaneOrientationToZAxes();
        m_axialImagePlaneWidget->SetSliceIndex(size[2]/2);

        m_sagitalImagePlaneWidget->SetPlaneOrientationToXAxes();
        m_sagitalImagePlaneWidget->SetSliceIndex(size[0]/2);

        m_coronalImagePlaneWidget->SetPlaneOrientationToYAxes();
        m_coronalImagePlaneWidget->SetSliceIndex(size[1]/2);

        if( m_axialPlaneVisible )
            m_axialImagePlaneWidget->On();
        if( m_sagitalPlaneVisible )
            m_sagitalImagePlaneWidget->On();
        if( m_coronalPlaneVisible )
            m_coronalImagePlaneWidget->On();
    }
}

void QCardiac3DMPRViewer::initializeWindowLevel()
{
    if( m_mainVolume )
    {
        m_defaultWindow = m_mainVolume->getVolumeSourceInformation()->getWindow();
        m_defaultLevel = m_mainVolume->getVolumeSourceInformation()->getLevel();
        if( m_defaultWindow == 0.0 && m_defaultLevel == 0.0 )
        {
            double *range = m_mainVolume->getSubVolume( m_actualSubVolume )->getVtkData()->GetScalarRange();
            m_defaultWindow = fabs(range[1] - range[0]);
            m_defaultLevel = ( range[1] + range[0] )/ 2.0;
        }
        this->resetWindowLevelToDefault();
    }
    else
    {
        DEBUG_LOG( "Intentant inicialitzar el window level sense haver donat input abans" );
    }
}




void QCardiac3DMPRViewer::setSubVolume( int index )
{
    double point1_a[3], point2_a[3], orig_a[3];
    double point1_s[3], point2_s[3], orig_s[3];
    double point1_c[3], point2_c[3], orig_c[3];
    double wl[2],wl1[2],wl2[2];

    Volume * subVolume;

    m_axialImagePlaneWidget->GetPoint1 ( point1_a );
    m_axialImagePlaneWidget->GetPoint2 ( point2_a );
    m_axialImagePlaneWidget->GetOrigin ( orig_a );

    m_sagitalImagePlaneWidget->GetPoint1 ( point1_s );
    m_sagitalImagePlaneWidget->GetPoint2 ( point2_s );
    m_sagitalImagePlaneWidget->GetOrigin ( orig_s );

    m_coronalImagePlaneWidget->GetPoint1 ( point1_c );
    m_coronalImagePlaneWidget->GetPoint2 ( point2_c );
    m_coronalImagePlaneWidget->GetOrigin ( orig_c );

    m_axialImagePlaneWidget->GetWindowLevel(wl);
    m_sagitalImagePlaneWidget->GetWindowLevel(wl1);
    m_coronalImagePlaneWidget->GetWindowLevel(wl2);

    subVolume = m_mainVolume->getSubVolume( index );

//     double * origen = subVolume->getVtkData()->GetOrigin();
//     double * space = subVolume->getVtkData()->GetSpacing();
//     double * origen = subVolume->getVtkData()->GetOrigin();
//     double * space = subVolume->getVtkData()->GetSpacing();
//     int * extent = subVolume->getVtkData()->GetExtent();
//     cout << "origen = " << origen[0] << "," << origen[1] << "," << origen[2] << endl;
//     cout << "spacing = " << space[0] << "," << space[1] << "," << space[2] << endl ;
//     cout << "extent x = " << extent[0] << "," << extent[1]  << endl ;
//     cout << "extent y = " << extent[2] << "," << extent[3]  << endl ;
//     cout << "extent z = " << extent[4] << "," << extent[5]  << endl ;

    m_axialImagePlaneWidget->SetInput( subVolume->getVtkData() );
//     m_sagitalImagePlaneWidget->SetInput( subVolume->getVtkData() );
//     m_coronalImagePlaneWidget->SetInput( subVolume->getVtkData() );


    m_axialImagePlaneWidget->SetPoint1 ( point1_a );
    m_axialImagePlaneWidget->SetPoint2 ( point2_a );
    m_axialImagePlaneWidget->SetOrigin ( orig_a );
/*
    m_sagitalImagePlaneWidget->SetPoint1 ( point1_s );
    m_sagitalImagePlaneWidget->SetPoint2 ( point2_s );
    m_sagitalImagePlaneWidget->SetOrigin ( orig_s );

    m_coronalImagePlaneWidget->SetPoint1 ( point1_c );
    m_coronalImagePlaneWidget->SetPoint2 ( point2_c );
    m_coronalImagePlaneWidget->SetOrigin ( orig_c );*/

    m_axialImagePlaneWidget->UpdatePlacement();
//     m_sagitalImagePlaneWidget->UpdatePlacement();
//     m_coronalImagePlaneWidget->UpdatePlacement();

    m_axialImagePlaneWidget->SetWindowLevel(wl[0], wl[1]);
//     m_sagitalImagePlaneWidget->SetWindowLevel(wl1[0], wl1[1]);
//     m_coronalImagePlaneWidget->SetWindowLevel(wl2[0], wl2[1]);

}



}; // end namespace udg
