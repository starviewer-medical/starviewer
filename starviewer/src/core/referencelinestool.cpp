/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "referencelinestool.h"
#include "referencelinestooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "series.h"
#include "imageplane.h"
#include "drawer.h"
#include "drawerpolygon.h"
// vtk
#include <vtkMatrix4x4.h> // per les transformacions amb matrius del pla

namespace udg {

ReferenceLinesTool::ReferenceLinesTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "ReferenceLinesTool";
    m_hasSharedData = true;

    m_myData = new ReferenceLinesToolData;
    m_toolData = m_myData;
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectionLines()) );

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    m_projectedReferencePlane = new DrawerPolygon;
    m_2DViewer->getDrawer()->draw( m_projectedReferencePlane, QViewer::Top2DPlane );
    m_2DViewer->getDrawer()->addToGroup( m_projectedReferencePlane, "ReferenceLines" );
    refreshInput();

    // cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *) ), SLOT( refreshInput() ) );
    // cada cop que el viewer canvïi de llesca, hem d'actualitzar el pla de projecció
    connect( m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateImagePlane()) );

    connect( m_2DViewer, SIGNAL(selected()),SLOT(refreshInput()) );
}

ReferenceLinesTool::~ReferenceLinesTool()
{
    delete m_projectedReferencePlane;
}

void ReferenceLinesTool::setToolData(ToolData * data)
{
    m_toolData = data;
    m_myData = qobject_cast<ReferenceLinesToolData *>(data);
    // quan canvïn les dades (ImagePlane), actualitzem les línies de projecció
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectionLines()) );
}

void ReferenceLinesTool::updateProjectionLines()
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

void ReferenceLinesTool::projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane)
{
    if( !(referencePlane && localizerPlane) )
        return;

    // primer mirem que siguin plans diferents
    if( *localizerPlane != *referencePlane )
    {
        // implementem primer el sistema simple d'en clunie. A partir dels plans
        // fem una projecció del pla de referència sobre el pla del localitzador
        double referenceOrigin[3], rowVector[3], columnVector[3];
        referencePlane->getOrigin( referenceOrigin );
        referencePlane->getRowDirectionVector( rowVector );
        referencePlane->getColumnDirectionVector( columnVector );

        // necessitem coordenades homogènies per els càlculs de matriu que fem amb la vtkMatrix4x4
        double tlhc[4], trhc[4], brhc[4], blhc[4];
        tlhc[3] = 1.0;
        trhc[3] = 1.0;
        brhc[3] = 1.0;
        blhc[3] = 1.0;

        // definim els 4 punts del pla a projectar
        for( int i = 0; i < 3; i++ )
        {
            tlhc[i] = referenceOrigin[i];
            trhc[i] = referenceOrigin[i] + rowVector[i]*referencePlane->getRowLength();
            brhc[i] = referenceOrigin[i] + rowVector[i]*referencePlane->getRowLength() + columnVector[i]*referencePlane->getColumnLength();
            blhc[i] = referenceOrigin[i] + columnVector[i]*referencePlane->getColumnLength();
        }

        // recollim les dades del pla del localitzador sobre el qual volem projectar el de referència
        double localizerRowVector[3], localizerColumnVector[3], localizerNormalVector[3], localizerOrigin[3];
        localizerPlane->getRowDirectionVector( localizerRowVector );
        localizerPlane->getColumnDirectionVector( localizerColumnVector );
        localizerPlane->getNormalVector( localizerNormalVector );
        localizerPlane->getOrigin( localizerOrigin );

        vtkMatrix4x4 *transformMatrix = vtkMatrix4x4::New();
        transformMatrix->Identity();
        // primer traslladem tots els punts
        for( int i = 0; i < 3; i++ )
        {
            tlhc[i] -= localizerOrigin[i];
            trhc[i] -= localizerOrigin[i];
            brhc[i] -= localizerOrigin[i];
            blhc[i] -= localizerOrigin[i];
        }
        // ara construim la matriu de projecció
        for( int column = 0; column < 3; column++ )
        {
            transformMatrix->SetElement(0,column,localizerRowVector[ column ]);
            transformMatrix->SetElement(1,column,localizerColumnVector[ column ]);
            transformMatrix->SetElement(2,column,localizerNormalVector[ column ]);
            // aquí apliquem el desplaçament després de la rotació per reubicar
            transformMatrix->SetElement(column,3,localizerOrigin[column]);
        }

        // apliquem la projecció a cada punt del pla que volem projectar
        transformMatrix->MultiplyPoint( tlhc, tlhc );
        transformMatrix->MultiplyPoint( trhc, trhc );
        transformMatrix->MultiplyPoint( brhc, brhc );
        transformMatrix->MultiplyPoint( blhc, blhc );

        // els punts amb els que definim el nostre polígon no són coordenades homogènies
        double p1[3],p2[3],p3[3],p4[3];
        for(int j = 0; j<3; j++)
        {
            p1[j] = tlhc[j];
            p2[j] = trhc[j];
            p3[j] = brhc[j];
            p4[j] = blhc[j];
        }

        m_projectedReferencePlane->setVertix( 0,p1 );
        m_projectedReferencePlane->setVertix( 1,p2 );
        m_projectedReferencePlane->setVertix( 2,p3 );
        m_projectedReferencePlane->setVertix( 3,p4 );
        m_2DViewer->getDrawer()->showGroup("ReferenceLines");
    }
}

void ReferenceLinesTool::updateFrameOfReference(Volume *volume)
{
    Series *series = volume->getSeries();
    if( series )
    {
        // ens guardem el nostre
        m_myFrameOfReferenceUID = series->getFrameOfReferenceUID();
        // i actualitzem el de les dades
        m_myData->setFrameOfReferenceUID( m_myFrameOfReferenceUID );
    }
    else
    {
        DEBUG_LOG("EL nou volum no té series NUL!");
    }
}

void ReferenceLinesTool::updateImagePlane()
{
    m_myData->setImagePlane( m_2DViewer->getCurrentImagePlane() );
}

void ReferenceLinesTool::refreshInput()
{
    m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    if( m_2DViewer->getInput() )
    {
        updateFrameOfReference(m_2DViewer->getInput());
        updateImagePlane();
    }
}

}
