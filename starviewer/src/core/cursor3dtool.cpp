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
#include "image.h"
#include "imageplane.h"
#include "drawercrosshair.h"
#include "drawer.h"
// vtk
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>

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
    connect( m_2DViewer, SIGNAL(sliceChanged( int ) ), SLOT( hideCrossHair() ) );

    refreshReferenceViewerData();

    m_crossHair = NULL;
}

Cursor3DTool::~Cursor3DTool()
{
    if( m_crossHair )
    {
        //HACK succedani d'Smart Pointer per tal que el drawer no elimini el crossHair quan s'activi el thickslab
        m_crossHair->decreaseReferenceCount();
        delete m_crossHair;
    }
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
}

void Cursor3DTool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            initializePosition();
            break;

        case vtkCommand::MouseMoveEvent:
            updatePosition();
            break;
        
        case vtkCommand::LeftButtonReleaseEvent:
            removePosition();
            break;
    }
}

void Cursor3DTool::initializePosition()
{
    if( !m_2DViewer->getInput() )
        return;
    
    m_viewer->setCursor( QCursor( Qt::BlankCursor ) );
    m_state = Computing;

    if ( !m_crossHair )
    {
        double xyz[3];
        m_2DViewer->getCurrentCursorImageCoordinate( xyz );
        m_crossHair = new DrawerCrossHair;

        //HACK succedani d'Smart Pointer per tal que el drawer no elimini el crossHair quan s'activi el thickslab
        m_crossHair->increaseReferenceCount();

        m_crossHair->setCentrePoint( xyz[0], xyz[1],xyz[2] );
        m_2DViewer->getDrawer()->draw( m_crossHair , QViewer::Top2DPlane );
    }

    m_myData->setVisible( true );
    updatePosition();

}

void Cursor3DTool::updatePosition()
{
    // En cas que no sigui el viewer que estem modificant
    // i que l'estat sigui l'indicat
    if( m_2DViewer->isActive() && m_state == Computing )
    {
        int index[3];
        double * dicomWorldPosition = new double[4];
        double coordinates[3];
        double xyz[3];
        ImagePlane * currentPlane = NULL;
        Image *image = NULL;

        //Cal fer els càlculs per passar del món VTK al mon que té el DICOM per guardar el punt en dicom a les dades compartides de la tool.
        // 1.- Trobar el punt correcte en el món VTK
        if( m_2DViewer->getCurrentCursorImageCoordinate( xyz ) )
        {
            // 2.- Trobar l'índex del vòxel en el DICOM
            m_2DViewer->getInput()->getVtkData()->ComputeStructuredCoordinates(xyz, index, coordinates);

            // 3.- Necessitem la imatge la qual pertany el punt per tal de trobar la imatge del dicom que conté la informació del pla.
            int slice = m_2DViewer->getCurrentSlice();
            double *spacing = m_2DViewer->getInput()->getSpacing();
            double *origin = m_2DViewer->getInput()->getOrigin();
            int zIndex = Q2DViewer::getZIndexForView( m_2DViewer->getView() );

            xyz[zIndex] = origin[zIndex] + (slice * spacing[zIndex]);
            switch( m_2DViewer->getView() )
            {
                case Q2DViewer::Axial:
                    currentPlane = m_2DViewer->getCurrentImagePlane();
                    break;

                case Q2DViewer::Sagital:
                case Q2DViewer::Coronal:
                    if( index[2] < m_2DViewer->getInput()->getImages().count() )
                    {
                        image = m_2DViewer->getInput()->getImage(index[2]); //La llesca sempre és l'index[2] del DICOM
                        currentPlane = new ImagePlane();
                        currentPlane->fillFromImage( image);
                    }
                    break;
            }

            if( currentPlane )
            {
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
                m_crossHair->setCentrePoint( xyz[0], xyz[1], xyz[2] );
                m_crossHair->setVisibility( true );
                m_crossHair->update();
                m_2DViewer->render();
                m_myData->setOriginPointPosition( dicomWorldPosition ); // Punt al món real (DICOM)
            }
            else
            {
                DEBUG_LOG("The requested plane is not available, maybe due to poor multiframe support.");
                INFO_LOG("No es pot actualitzar la posició del cursor 3D perquè no podem obtenir el pla corresponent.");
            }
        }
    }
}

void Cursor3DTool::removePosition()
{
    if( m_state == Computing )
    {
        m_state = None;
        m_viewer->setCursor( Qt::ArrowCursor );
    }
    /// S'ha demanat que el cursor no desparegui al deixar de clicar.
//     m_crossHair->setVisibility( false );
//     m_crossHair->update();
//     m_2DViewer->render();
//     m_myData->setVisible( false );
}

void Cursor3DTool::updateProjectedPoint()
{
    // en cas que no sigui el viewer que estem modificant
    if( !m_2DViewer->isActive() && m_2DViewer->getInput() )
    {
        if ( !m_crossHair && m_2DViewer->getInput() )
        {
            m_crossHair = new DrawerCrossHair;

            //HACK succedani d'Smart Pointer per tal que el drawer no elimini el crossHair quan s'activi el thickslab
            m_crossHair->increaseReferenceCount();

            m_2DViewer->getDrawer()->draw( m_crossHair , QViewer::Top2DPlane );
        }

        if( !m_myData->isVisible() )
        {
            m_crossHair->setVisibility( false );
        }
        else
        {
            //Només podem projectar si tenen el mateix frame of reference UID
            if( m_myFrameOfReferenceUID == m_myData->getFrameOfReferenceUID() )
            {
                m_crossHair->setVisibility( true );
                projectPoint();
            }
            else
            {
                m_crossHair->setVisibility( false );
            }
        }
        m_crossHair->update();
        m_2DViewer->render();
    }
}

void Cursor3DTool::projectPoint()
{
        double *position = new double[3];
        m_2DViewer->projectDICOMPointToCurrentDisplayedImage( m_myData->getOriginPointPosition(), position );

        if( position )
        {
            double distance;
            int nearestSlice = getNearestSlice( m_myData->getOriginPointPosition(), distance );

            if ( nearestSlice != -1 && distance < ( m_2DViewer->getThickness()*1.5 ) ){
                m_2DViewer->setSlice( nearestSlice );
                m_crossHair->setCentrePoint( position[0], position[1], position[2] );
                m_crossHair->setVisibility( true );
            }
            else //L'amaguem perquè sinó estariem mostrant un punt incorrecte
            {
                m_crossHair->setVisibility( false );
            }

            m_crossHair->update();
            m_2DViewer->render();
        }
}

void Cursor3DTool::updateFrameOfReference()
{
    Q_ASSERT( m_2DViewer->getInput() ); // hi ha d'haver input per força

    // TODO De moment agafem la primera imatge perquè assumim que totes pertanyen a la mateixa sèrie.
    // També ho fem així de moment per evitar problemes amb imatges multiframe, que encara no tractem correctament
    Image *image = m_2DViewer->getInput()->getImage(0);
    if( image )
    {
        Series *series = image->getParentSeries();
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

void Cursor3DTool::hideCrossHair()
{
    if( m_2DViewer->isActive() && m_state == None )
    {
        m_crossHair->setVisibility( false );
        m_crossHair->update();
        m_2DViewer->render();
        m_myData->setVisible( false );
    }
}

int Cursor3DTool::getNearestSlice(double projectedPosition[3], double &distance)
{
    double currentDistance;
    double minimumDistance = -1.0;
    int minimumSlice = -1;
    double currentPlaneOrigin[3], currentNormalVector[3];
    ImagePlane *currentPlane = 0;
    int maximumSlice = m_2DViewer->getMaximumSlice();
    int currentPhase = m_2DViewer->getCurrentPhase();

    for (int i = 0; i < maximumSlice; i++)
    {
        currentPlane = m_2DViewer->getImagePlane(i, currentPhase);

        if ( currentPlane )
        {
            currentPlane->getOrigin(currentPlaneOrigin);
            currentPlane->getNormalVector(currentNormalVector);

            currentDistance = vtkPlane::DistanceToPlane(projectedPosition, currentNormalVector, currentPlaneOrigin);

            if ( (currentDistance < minimumDistance) || (minimumDistance == -1.0) )
            {
                minimumDistance = currentDistance;
                minimumSlice = i;
            }
        }
    }
    distance = minimumDistance;

    return minimumSlice;
}

}
