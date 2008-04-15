/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "cursor3dtool.h"

#include "cursor3dtool.h"
#include "cursor3dtooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "series.h"
#include "imageplane.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawerline.h"
#include "mathtools.h"
// vtk
#include <vtkPlane.h>
#include <vtkCommand.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

namespace udg {

Cursor3DTool::Cursor3DTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "Cursor3DTool";
    m_hasSharedData = true;

    m_myData = new Cursor3DToolData;
    m_toolData = m_myData;
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectionLines()) );

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className() );

//     refreshReferenceViewerData();

    // cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *) ), SLOT( refreshReferenceViewerData() ) );
    connect( m_2DViewer, SIGNAL(selected()),SLOT(refreshReferenceViewerData()) );

    //Esfera per veure quin punt origen s'està calculant
    // TODO S'ha de canviar per un objecte VTK amb forma de creu que tingui en compte la mida del voxel
    m_sphereActor = vtkActor::New();
    m_sphere = vtkSphereSource::New();
    m_sphere->SetRadius( 8 );
    m_sphereActor->GetProperty()->SetColor( 1.0, 0.13, 0.26 );

    m_sphereMapper = vtkPolyDataMapper::New();
    m_sphereMapper->SetInputConnection( m_sphere->GetOutputPort() );
    m_sphereActor->SetMapper( m_sphereMapper );

    m_sphereActor->VisibilityOff();
    m_2DViewer->getRenderer()->AddActor( m_sphereActor );

}

Cursor3DTool::~Cursor3DTool()
{
}

void Cursor3DTool::setToolData(ToolData * data)
{
    m_toolData = data;
    m_myData = qobject_cast<Cursor3DToolData *>(data);
    // quan canvïn les dades (ImagePlane), actualitzem les línies de projecció
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectionLines()) );
}

void Cursor3DTool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            if( m_2DViewer->getInput() )
            {
                initializePosition();
            }
            break;

        case vtkCommand::MouseMoveEvent:
            if( m_state == CALCULATING )
            {
                updatePosition();
            }
            break;
        case vtkCommand::LeftButtonReleaseEvent:
                removePosition();
            break;
    }
}

void Cursor3DTool::initializePosition()
{
    m_viewer->setCursor( QCursor(QPixmap(":/images/cross.png")) );
    m_state = CALCULATING;

    double position[4];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer(), m_2DViewer->getEventPositionX(), m_2DViewer->getEventPositionY(), 0, position );

//     DEBUG_LOG( tr("POSICIO: [%1,%2,%3]").arg(position[0]).arg(position[1]).arg(position[2]) );

    // TODO Quan sapiga calcular bé la posició, també s'haurà de fer aqui!!!

    int slice = m_2DViewer->getCurrentSlice();
    double *spacing = m_2DViewer->getInput()->getSpacing();
    double *origin = m_2DViewer->getInput()->getOrigin();

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            position[2] = origin[2] + (slice * spacing[2]);
            break;
        case Q2DViewer::Sagital:
            position[0] = origin[0] + (slice * spacing[0]);
            break;
        case Q2DViewer::Coronal:
            position[1] = origin[1] + (slice * spacing[1]);
            break;
    }

//     DEBUG_LOG( tr("Original point: [%1,%2,%3]").arg(position[0]).arg(position[1]).arg(position[2]) );

    m_sphere->SetCenter( position );
    m_myData->setOriginPointPosition( position );
    m_sphereActor->VisibilityOn();
    m_2DViewer->refresh();
}

void Cursor3DTool::updatePosition()
{

    // en cas que no sigui el viewer que estem modificant
    if( m_2DViewer->isActive() )
    {
        double position[4];
        m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer(), m_2DViewer->getEventPositionX(), m_2DViewer->getEventPositionY(), 0, position );

        int slice = m_2DViewer->getCurrentSlice();
        double *spacing = m_2DViewer->getInput()->getSpacing();
        double *origin = m_2DViewer->getInput()->getOrigin();

        switch( m_2DViewer->getView() )
        {
            case Q2DViewer::Axial:
                position[2] = origin[2] + (slice * spacing[2]);
                break;
            case Q2DViewer::Sagital:
                position[0] = origin[0] + (slice * spacing[0]);
                break;
            case Q2DViewer::Coronal:
                position[1] = origin[1] + (slice * spacing[1]);
                break;
        }

        DEBUG_LOG( tr("Original point: [%1,%2,%3]").arg(position[0]).arg(position[1]).arg(position[2]) );

        /*TODO Aqui cal fer els càlculs per passar del món VTK al mon que té el DICOM
        *   1.- Trobar el punt correcte en el món VTK
        *   2.- Trobar l'índex del vòxel en el DICOM
        *   3.- Passar aquest punt al món REAL (DICOM) amb la matriu
        *   4.- Modificar les dades compartides del punt per tal que els altres s'actualitzin
        */

//         double xyz[3];
//         m_2DViewer->getCurrentCursorPosition( xyz );
//
//         ImagePlane * currentPlane = m_2DViewer->getCurrentImagePlane();
//         double currentPlaneRowVector[3], currentPlaneColumnVector[3], currentPlaneNormalVector[3], currentPlaneOrigin[3];
//         currentPlane->getRowDirectionVector( currentPlaneRowVector );
//         currentPlane->getColumnDirectionVector( currentPlaneColumnVector );
//         currentPlane->getNormalVector( currentPlaneNormalVector );
//         currentPlane->getOrigin( currentPlaneOrigin );
//
//         m_2DViewer->getInput()->getVtkData()->ComputeStructuredCoordinates(currentPlaneOrigin, index, coordinates);
//
//         m_2DViewer->getInput()->getVtkData()->ComputeStructuredCoordinates(position, index, coordinates);
//
//         m_2DViewer->getInput()->getVtkData()->ComputeStructuredCoordinates(xyz, index, coordinates);
//
//         double * origen = m_2DViewer->getInput()->getVtkData()->GetOrigin();
//
//         dicomWorldPosition[0]=(double)index[0];
//         dicomWorldPosition[1]=(double)index[1];
//         dicomWorldPosition[2]=(double)index[2];
//
//
//         // a partir d'aquestes dades creem la matriu de projeccio, que projectara els punts
//         // del pla de referencia sobre el pla del localitzador
//         vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
//         projectionMatrix->Identity();
//         for( int column = 0; column < 3; column++ )
//         {
//             projectionMatrix->SetElement(0,column,(currentPlaneRowVector[ column ])*spacing[0]);
//             projectionMatrix->SetElement(1,column,(currentPlaneColumnVector[ column ])*spacing[1]);
//             projectionMatrix->SetElement(2,column,(currentPlaneNormalVector[ column ])*currentPlane->getThickness());
//             projectionMatrix->SetElement(column,3,currentPlaneOrigin[column]);
//         }
//
//         projectionMatrix->MultiplyPoint( dicomWorldPosition, dicomWorldPosition );
//
//         DEBUG_LOG( tr("Punt DICOM: [%1,%2,%3]").arg(dicomWorldPosition[0]).arg(dicomWorldPosition[1]).arg(dicomWorldPosition[2]) );

        m_sphere->SetCenter( position );
        m_myData->setOriginPointPosition( position );
        m_sphereActor->VisibilityOn();
        m_2DViewer->refresh();
    }

}

void Cursor3DTool::removePosition()
{
    m_state = NONE;
    m_viewer->setCursor( Qt::ArrowCursor );
//     m_sphereActor->VisibilityOff();
    m_2DViewer->refresh();
}

void Cursor3DTool::updateProjectionLines()
{
    // en cas que no sigui el viewer que estem modificant
    if( !m_2DViewer->isActive() )
    {
        // intentarem projectar el pla que hi ha a m_myData
        // primer cal que comparteixin el mateix FrameOfReference

        if( m_myFrameOfReferenceUID == m_myData->getFrameOfReferenceUID() )
        {
            // aquí ja ho deixem en mans de la projecció
            projectIntersection( m_myData->getImagePlane(), m_2DViewer->getCurrentImagePlane() );
        }
    }
}

void Cursor3DTool::projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane)
{
//     if( !(referencePlane && localizerPlane) )
//         return;

        double *position = new double[3];/* = m_myData->getOriginPointPosition();*/

        if( !(m_myInstanceUID == m_myData->getInstanceUID() ) ) // Si no són iguals, calculem la projecció
        {
            m_2DViewer->projectDICOMPointToCurrentDisplayedImage( m_myData->getOriginPointPosition(), position );

//             DEBUG_LOG( tr("La posició ha estat calculada") );
        }
        else
        {
            position = m_myData->getOriginPointPosition();

//             DEBUG_LOG( tr("La posició NO ha estat calculada") );
        }

//         DEBUG_LOG( tr("Projected point: [%1,%2,%3]").arg(position[0]).arg(position[1]).arg(position[2]) );

        if( position )
        {
            m_sphere->SetCenter( position );
            m_sphereActor->VisibilityOn();
            m_2DViewer->refresh();

            double * distance;
            int nearestSlice = m_2DViewer->getNearestSlice( position, distance);
            if ( nearestSlice != -1 ){
                m_2DViewer->setSlice( nearestSlice );
            }

//             DEBUG_LOG( tr("Nearest slice: %1").arg(nearestSlice) );
        }
}

void Cursor3DTool::updateFrameOfReference()
{
    Q_ASSERT( m_2DViewer->getInput() ); // hi ha d'haver input per força
    Series *series = m_2DViewer->getInput()->getSeries();
    if( series )
    {
        // ens guardem el nostre
        m_myFrameOfReferenceUID = series->getFrameOfReferenceUID();
        m_myInstanceUID = series->getInstanceUID();
        // i actualitzem el de les dades
        m_myData->setFrameOfReferenceUID( m_myFrameOfReferenceUID );
        m_myData->setInstanceUID( m_myInstanceUID );
    }
    else
    {
        DEBUG_LOG("EL nou volum no té series NUL!");
    }
}

void Cursor3DTool::updateImagePlane()
{
//     m_myData->setImagePlane( m_2DViewer->getCurrentImagePlane() );
}

void Cursor3DTool::refreshReferenceViewerData()
{
    // si es projectaven plans sobre el nostre drawer, les amaguem
    if( m_2DViewer->getInput() )
    {
        updateFrameOfReference();
        updateImagePlane();
    }
}

}