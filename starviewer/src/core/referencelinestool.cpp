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
#include "drawerline.h"
#include "mathtools.h"
// vtk
#include <vtkPlane.h>

namespace udg {

ReferenceLinesTool::ReferenceLinesTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent), m_projectedReferencePlane(0)
{
    m_toolName = "ReferenceLinesTool";
    m_hasSharedData = true;

    m_myData = new ReferenceLinesToolData;
    m_toolData = m_myData;
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectionLines()) );

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className() );

    resurrectPolygon();
    refreshReferenceViewerData();

    // cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *) ), SLOT( refreshReferenceViewerData() ) );
    // cada cop que el viewer canvïi de llesca, hem d'actualitzar el pla de projecció
    connect( m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateImagePlane()) );

    connect( m_2DViewer, SIGNAL(selected()),SLOT(refreshReferenceViewerData()) );
}

ReferenceLinesTool::~ReferenceLinesTool()
{
    disconnect( m_projectedReferencePlane, SIGNAL( dying(DrawerPrimitive*) ), this, SLOT( resurrectPolygon() ) );
    disconnect( m_projectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), this, SLOT( resurrectPolygon() ) );
    disconnect( m_backgroundProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), this, SLOT( resurrectPolygon() ) );

    delete m_projectedReferencePlane;
    delete m_projectedIntersection;
    delete m_backgroundProjectedIntersection;
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

void ReferenceLinesTool::resurrectPolygon()
{
    m_projectedReferencePlane = new DrawerPolygon;
// descomentar aquestes 3 linies si es vol mostrar el poligon del pla projectat
//     m_2DViewer->getDrawer()->draw( m_projectedReferencePlane, QViewer::Top2DPlane );
//     m_2DViewer->getDrawer()->addToGroup( m_projectedReferencePlane, "ReferenceLines" );
//     connect( m_projectedReferencePlane, SIGNAL( dying(DrawerPrimitive*) ), SLOT( resurrectPolygon() ) );

    // linia de background, igual a l'anterior, per fer-la ressaltar a la imatge TODO es podria estalviar aquesta linia de mes si el propi drawer
    // admetes "background" en una linia amb "stiple" discontinu
    m_backgroundProjectedIntersection = new DrawerLine;
    m_backgroundProjectedIntersection->setColor( QColor(0,0,0) );

    m_2DViewer->getDrawer()->draw( m_backgroundProjectedIntersection, QViewer::Top2DPlane );
    m_2DViewer->getDrawer()->addToGroup( m_backgroundProjectedIntersection, "ReferenceLines" );
    connect( m_backgroundProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), SLOT( resurrectPolygon() ) );

    // linia projectada de tall entre els plans localitzador i referencia
    m_projectedIntersection = new DrawerLine;
    m_projectedIntersection->setColor( QColor(255,160,0) );
    m_projectedIntersection->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );

    m_2DViewer->getDrawer()->draw( m_projectedIntersection, QViewer::Top2DPlane );
    m_2DViewer->getDrawer()->addToGroup( m_projectedIntersection, "ReferenceLines" );
    connect( m_projectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), SLOT( resurrectPolygon() ) );

    // TODO mirar si ens podem estalviar de cridar aquest metode aqui
    m_2DViewer->getDrawer()->showGroup("ReferenceLines");
}

void ReferenceLinesTool::projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane)
{
    if( !(referencePlane && localizerPlane) )
        return;

    // primer mirem que siguin plans diferents
    if( *localizerPlane != *referencePlane )
    {
        // Farem dues projeccions:
        // Una sera la projeccio directa del pla de referencia sobre el localitzador
        // que es el metode proposat per en David Clunie ( http://www.dclunie.com/medical-image-faq/html/part2.html, ap. 2.2.1 )
        // La segona sera la projeccio de la linia de tall entre els dos plans, derivat de la idea d'en Clunie
        // La primera aproximacio, nomes es fara servir per motius de debug i la que es representara per pantalla sera
        // la segona, que de cares al diagnostic es la mes correcta.

        // implementem primer el sistema simple d'en clunie. A partir dels plans
        // fem una projecció del pla de referència sobre el pla del localitzador

        // Primer de tot obtenim les dades del pla de referencia
        double referenceOrigin[3], referenceRowVector[3], referenceColumnVector[3];
        referencePlane->getOrigin( referenceOrigin );
        referencePlane->getRowDirectionVector( referenceRowVector );
        referencePlane->getColumnDirectionVector( referenceColumnVector );

        // cal calcular els els 4 punts del pla de referencia
        // aquests mateixos punts serviran tant per la projeccio total del pla com per
        // calcular els punts de tall dels plans per la solucio 2 ( la que s'aplica en realitat )
        double referencePlaneVertix1[3],referencePlaneVertix2[3],referencePlaneVertix3[3],referencePlaneVertix4[3];
        for( int i = 0; i < 3; i++ )
        {
            referencePlaneVertix1[i] = referenceOrigin[i];
            referencePlaneVertix2[i] = referenceOrigin[i] + referenceRowVector[i]*referencePlane->getRowLength();
            referencePlaneVertix3[i] = referenceOrigin[i] + referenceRowVector[i]*referencePlane->getRowLength() + referenceColumnVector[i]*referencePlane->getColumnLength();
            referencePlaneVertix4[i] = referenceOrigin[i] + referenceColumnVector[i]*referencePlane->getColumnLength();
        }

        //
        // Solucio 1: projeccio de plans
        //

        // !!! ATENCIO !!!!
        // si volem estalviar calculs quan no es mostra el poligon projectat podem comentar aquest bloc fins arribar a l'inici
        // de la "Solucio 2"

        // ara calculem les projeccions de cada punt del pla de referencia
//         double projectedVertix1[3],projectedVertix2[3],projectedVertix3[3],projectedVertix4[3];
//
//         m_2DViewer->projectPointToCurrentDisplayedImage( referencePlaneVertix1, projectedVertix1 );
//         m_2DViewer->projectPointToCurrentDisplayedImage( referencePlaneVertix2, projectedVertix2 );
//         m_2DViewer->projectPointToCurrentDisplayedImage( referencePlaneVertix3, projectedVertix3 );
//         m_2DViewer->projectPointToCurrentDisplayedImage( referencePlaneVertix4, projectedVertix4 );
//
//         // donem els punts al poligon a dibuixar
//         m_projectedReferencePlane->setVertix( 0, projectedVertix1 );
//         m_projectedReferencePlane->setVertix( 1, projectedVertix2 );
//         m_projectedReferencePlane->setVertix( 2, projectedVertix3 );
//         m_projectedReferencePlane->setVertix( 3, projectedVertix4 );
//         m_2DViewer->getDrawer()->showGroup("ReferenceLines");

        //
        // Solucio 2: projeccio de la interseccio dels plans
        //
        /// llegir http://fixunix.com/dicom/51195-scanogram-lines-mr.html

        // recollim les dades del pla del localitzador sobre el qual volem projectar el de referència
        double localizerNormalVector[3], localizerOrigin[3];
        localizerPlane->getNormalVector( localizerNormalVector );
        localizerPlane->getOrigin( localizerOrigin );

        // calculem totes les possibles interseccions
        int numberOfIntersections = 0;
        double t, firstIntersectionPoint[3], secondIntersectionPoint[3];
        if( vtkPlane::IntersectWithLine( referencePlaneVertix1, referencePlaneVertix2, localizerNormalVector, localizerOrigin, t, firstIntersectionPoint ) )
        {
            DEBUG_LOG( QString("Segment P1-P2 intersecciona en el punt: %1,%2,%3").arg( firstIntersectionPoint[0] ).arg( firstIntersectionPoint[1] ).arg( firstIntersectionPoint[2] ) );
            numberOfIntersections = 1;

            if( vtkPlane::IntersectWithLine( referencePlaneVertix3, referencePlaneVertix4, localizerNormalVector, localizerOrigin, t, secondIntersectionPoint ) )
            {
                DEBUG_LOG( QString("Segment P3-P4 intersecciona en el punt: %1,%2,%3").arg( secondIntersectionPoint[0] ).arg( secondIntersectionPoint[1] ).arg( secondIntersectionPoint[2] ) );
                numberOfIntersections = 2;
            }
        }
        else if( vtkPlane::IntersectWithLine( referencePlaneVertix2, referencePlaneVertix3, localizerNormalVector, localizerOrigin, t, firstIntersectionPoint ) )
        {
            DEBUG_LOG( QString("Segment P2-P3 intersecciona en el punt: %1,%2,%3").arg( firstIntersectionPoint[0] ).arg( firstIntersectionPoint[1] ).arg( firstIntersectionPoint[2] ) );
            numberOfIntersections = 1;

            if( vtkPlane::IntersectWithLine( referencePlaneVertix4, referencePlaneVertix1, localizerNormalVector, localizerOrigin, t, secondIntersectionPoint ) )
            {
                DEBUG_LOG( QString("Segment P4-P1 intersecciona en el punt: %1,%2,%3").arg( secondIntersectionPoint[0] ).arg( secondIntersectionPoint[1] ).arg( secondIntersectionPoint[2] ) );
                numberOfIntersections = 2;
            }
        }
        // un cop tenim les interseccions nomes cal projectar-les i pintar la linia
        DEBUG_LOG(" ======== Nombre d'interseccions entre plans: " +  QString::number( numberOfIntersections ) );
        if( numberOfIntersections == 2 )
        {
            m_2DViewer->projectPointToCurrentDisplayedImage( firstIntersectionPoint, firstIntersectionPoint );
            m_2DViewer->projectPointToCurrentDisplayedImage( secondIntersectionPoint, secondIntersectionPoint );

            // linia discontinua
            m_projectedIntersection->setFirstPoint( firstIntersectionPoint );
            m_projectedIntersection->setSecondPoint( secondIntersectionPoint );
            // linia de background
            m_backgroundProjectedIntersection->setFirstPoint( firstIntersectionPoint );
            m_backgroundProjectedIntersection->setSecondPoint( secondIntersectionPoint );

            m_2DViewer->getDrawer()->showGroup("ReferenceLines");
        }
        else
        {
            m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
        }

    }
}

void ReferenceLinesTool::updateFrameOfReference()
{
    Q_ASSERT( m_2DViewer->getInput() ); // hi ha d'haver input per força
    Series *series = m_2DViewer->getInput()->getSeries();
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

void ReferenceLinesTool::refreshReferenceViewerData()
{
    // si es projectaven plans sobre el nostre drawer, les amaguem
    m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    if( m_2DViewer->getInput() )
    {
        updateFrameOfReference();
        updateImagePlane();
    }
}

}
