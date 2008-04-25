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
// #include "mathtools.h"
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
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectedPoint()) );

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className() );

    // cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *) ), SLOT( refreshReferenceViewerData() ) );
    connect( m_2DViewer, SIGNAL(selected()),SLOT(refreshReferenceViewerData()) );

    refreshReferenceViewerData();
    
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
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectedPoint()) );

    if( m_2DViewer->isActive() )
    {
        refreshReferenceViewerData();
    }
    updateProjectedPoint();
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

    updatePosition();
}

void Cursor3DTool::updatePosition()
{

    // en cas que no sigui el viewer que estem modificant
    if( m_2DViewer->isActive() )
    {
        int index[3];
        double * dicomWorldPosition = new double[4];
        double coordinates[3];
        double xyz[3];
        ImagePlane * currentPlane;
        Image *image;
         
        //Cal fer els càlculs per passar del món VTK al mon que té el DICOM per guardar el punt en dicom a les dades compartides de la tool.
        // 1.- Trobar el punt correcte en el món VTK
        m_2DViewer->getCurrentCursorPosition( xyz );

        // 2.- Trobar l'índex del vòxel en el DICOM
        m_2DViewer->getInput()->getVtkData()->ComputeStructuredCoordinates(xyz, index, coordinates);

        // 3.- Necessitem la imatge la qual pertany el punt per tal de trobar la imatge del dicom que conté la informació del pla.

        if( xyz[0] != -1.0 && xyz[1] != -1.0 && xyz[2] != -1)
        {
            int slice = m_2DViewer->getCurrentSlice();
            double *spacing = m_2DViewer->getInput()->getSpacing();
            double *origin = m_2DViewer->getInput()->getOrigin();
                    
            switch( m_2DViewer->getView() )
            {
                case Q2DViewer::Axial:

                    xyz[2] = origin[2] + (slice * spacing[2]);
                    currentPlane = m_2DViewer->getCurrentImagePlane();
                    break;

                case Q2DViewer::Sagital:
                    xyz[0] = origin[0] + (slice * spacing[0]);
                    image = m_2DViewer->getInput()->getSeries()->getImages().at( index[2] ); //La llesca sempre és l'index[2] del DICOM
                    currentPlane = new ImagePlane();
                    currentPlane->fillFromImage( image);
                    break;

                case Q2DViewer::Coronal:
                    xyz[1] = origin[1] + (slice * spacing[1]);
                    image = m_2DViewer->getInput()->getSeries()->getImages().at( index[2] ); //La llesca sempre és l'index[2] del DICOM
                    currentPlane = new ImagePlane();
                    currentPlane->fillFromImage( image);
                    break;
            }

            // 3.- Construim la matiu per mapejar l'index del píxel del DICOM a un punt del món real
            double currentPlaneRowVector[3], currentPlaneColumnVector[3], currentPlaneOrigin[3];
            currentPlane->getRowDirectionVector( currentPlaneRowVector );
            currentPlane->getColumnDirectionVector( currentPlaneColumnVector );
            currentPlane->getOrigin( currentPlaneOrigin );

            vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
            projectionMatrix->Identity();
            int row;
            for( row = 0; row < 3; row++ )
            {
                projectionMatrix->SetElement(row,0, (currentPlaneRowVector[ row ])*spacing[0]);
                projectionMatrix->SetElement(row,1, (currentPlaneColumnVector[ row ])*spacing[1]);
                projectionMatrix->SetElement(row,2, 0.0);
                projectionMatrix->SetElement(row,3, currentPlaneOrigin[row]);
            }
            
            // 3.- Mappeig de l'índex del píxel al món real
            dicomWorldPosition[0] = (double)index[0];
            dicomWorldPosition[1] = (double)index[1];
            dicomWorldPosition[2] = (double)index[2];
            dicomWorldPosition[3] = 1.0;
            projectionMatrix->MultiplyPoint( dicomWorldPosition, dicomWorldPosition );// Matriu * punt

            // 4.- Modificar les dades compartides del punt per tal que els altres s'actualitzin i situar el punt origen
            m_sphere->SetCenter( xyz ); //Posició al món VTK
            m_myData->setOriginPointPosition( dicomWorldPosition ); // Punt al món real (DICOM)
            m_sphereActor->VisibilityOn();
            m_2DViewer->refresh();
        }
    }
}

void Cursor3DTool::removePosition()
{
    m_state = NONE;
    m_viewer->setCursor( Qt::ArrowCursor );
//     m_sphereActor->VisibilityOff();
    m_2DViewer->refresh();
}

void Cursor3DTool::updateProjectedPoint()
{
    // en cas que no sigui el viewer que estem modificant
    if( !m_2DViewer->isActive() )
    {
        //Només podem projectar si tenen el mateix frame of reference UID
        if( m_myFrameOfReferenceUID == m_myData->getFrameOfReferenceUID() )
        {
            projectPoint();
        }
    }
}

void Cursor3DTool::projectPoint()
{
        double *position = new double[3];
            
        m_2DViewer->projectDICOMPointToCurrentDisplayedImage( m_myData->getOriginPointPosition(), position );

        if( position )
        {
            m_sphere->SetCenter( position );
            m_sphereActor->VisibilityOn();
            m_2DViewer->refresh();

            double distance;
            int nearestSlice = m_2DViewer->getNearestSlice( m_myData->getOriginPointPosition(), distance );
            if ( nearestSlice != -1 ){
                m_2DViewer->setSlice( nearestSlice );
            }
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

void Cursor3DTool::refreshReferenceViewerData()
{
    // si es projectaven plans sobre el nostre drawer, les amaguem
    if( m_2DViewer->getInput() )
    {
        updateFrameOfReference();
    }
}

}