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
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className() );
    }

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

    disconnect( m_upperProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), this, SLOT( resurrectPolygon() ) );
    disconnect( m_backgroundUpperProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), this, SLOT( resurrectPolygon() ) );
    disconnect( m_lowerProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), this, SLOT( resurrectPolygon() ) );
    disconnect( m_backgroundLowerProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), this, SLOT( resurrectPolygon() ) );

    delete m_projectedReferencePlane;
    delete m_upperProjectedIntersection;
    delete m_backgroundUpperProjectedIntersection;
    delete m_lowerProjectedIntersection;
    delete m_backgroundLowerProjectedIntersection;

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

    //
    // linia superior projectada de tall entre els plans localitzador i referencia
    //

    // linia de background, igual a la següent, per fer-la ressaltar a la imatge TODO es podria estalviar aquesta linia de mes si el propi drawer
    // admetes "background" en una linia amb "stiple" discontinu
    m_backgroundUpperProjectedIntersection = new DrawerLine;
    m_backgroundUpperProjectedIntersection->setColor( QColor(0,0,0) );

    m_2DViewer->getDrawer()->draw( m_backgroundUpperProjectedIntersection, QViewer::Top2DPlane );
    m_2DViewer->getDrawer()->addToGroup( m_backgroundUpperProjectedIntersection, "ReferenceLines" );
    connect( m_backgroundUpperProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), SLOT( resurrectPolygon() ) );

    // linia "de punts"
    m_upperProjectedIntersection = new DrawerLine;
    m_upperProjectedIntersection->setColor( QColor(255,160,0) );
    m_upperProjectedIntersection->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );

    m_2DViewer->getDrawer()->draw( m_upperProjectedIntersection, QViewer::Top2DPlane );
    m_2DViewer->getDrawer()->addToGroup( m_upperProjectedIntersection, "ReferenceLines" );
    connect( m_upperProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), SLOT( resurrectPolygon() ) );

    //
    // linia inferior projectada de tall entre els plans localitzador i referencia
    //
    m_backgroundLowerProjectedIntersection = new DrawerLine;
    m_backgroundLowerProjectedIntersection->setColor( QColor(0,0,0) );

    m_2DViewer->getDrawer()->draw( m_backgroundLowerProjectedIntersection, QViewer::Top2DPlane );
    m_2DViewer->getDrawer()->addToGroup( m_backgroundLowerProjectedIntersection, "ReferenceLines" );
    connect( m_backgroundLowerProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), SLOT( resurrectPolygon() ) );

    m_lowerProjectedIntersection = new DrawerLine;
    m_lowerProjectedIntersection->setColor( QColor(255,160,0) );
    m_lowerProjectedIntersection->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );

    m_2DViewer->getDrawer()->draw( m_lowerProjectedIntersection, QViewer::Top2DPlane );
    m_2DViewer->getDrawer()->addToGroup( m_lowerProjectedIntersection, "ReferenceLines" );
    connect( m_lowerProjectedIntersection, SIGNAL( dying(DrawerPrimitive*) ), SLOT( resurrectPolygon() ) );

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

        //
        // Solucio 1: projeccio de plans
        //

        // ara calculem les projeccions de cada punt del pla de referencia
        // obtenim els els 4 punts del pla de referencia (central)
//         QList< QVector<double> > referencePlaneBounds = referencePlane->getCentralBounds();
//         double projectedVertix1[3],projectedVertix2[3],projectedVertix3[3],projectedVertix4[3];
//
//         m_2DViewer->projectPointToCurrentDisplayedImage( (double *)referencePlaneBounds.at(0).data(), projectedVertix1 );
//         m_2DViewer->projectPointToCurrentDisplayedImage( (double *)referencePlaneBounds.at(1).data(), projectedVertix2 );
//         m_2DViewer->projectPointToCurrentDisplayedImage( (double *)referencePlaneBounds.at(2).data(), projectedVertix3 );
//         m_2DViewer->projectPointToCurrentDisplayedImage( (double *)referencePlaneBounds.at(3).data(), projectedVertix4 );
//
//         // donem els punts al poligon a dibuixar
//         m_projectedReferencePlane->setVertix( 0, projectedVertix1 );
//         m_projectedReferencePlane->setVertix( 1, projectedVertix2 );
//         m_projectedReferencePlane->setVertix( 2, projectedVertix3 );
//         m_projectedReferencePlane->setVertix( 3, projectedVertix4 );
//         m_2DViewer->getDrawer()->showGroup("ReferenceLines");

        //
        // Solucio 2: projeccio de la intersecció dels plans
        //
        /// llegir http://fixunix.com/dicom/51195-scanogram-lines-mr.html

        // recollim les dades del pla del localitzador sobre el qual volem projectar el de referència
        double localizerNormalVector[3], localizerOrigin[3];
        localizerPlane->getNormalVector( localizerNormalVector );
        localizerPlane->getOrigin( localizerOrigin );

        // calculem totes les possibles interseccions
        QList< QVector<double> > upperPlaneBounds = referencePlane->getUpperBounds();
        double firstIntersectionPoint[3], secondIntersectionPoint[3];
        int numberOfIntersections = this->getIntersections( upperPlaneBounds.at(0), upperPlaneBounds.at(1), upperPlaneBounds.at(2), upperPlaneBounds.at(3), localizerPlane, firstIntersectionPoint, secondIntersectionPoint );

        // un cop tenim les interseccions nomes cal projectar-les i pintar la linia
        DEBUG_LOG(" ======== Nombre d'interseccions entre plans: " +  QString::number( numberOfIntersections ) );
        if( numberOfIntersections == 2 )
        {
            m_2DViewer->projectPointToCurrentDisplayedImage( firstIntersectionPoint, firstIntersectionPoint );
            m_2DViewer->projectPointToCurrentDisplayedImage( secondIntersectionPoint, secondIntersectionPoint );

            // linia discontinua
            m_upperProjectedIntersection->setFirstPoint( firstIntersectionPoint );
            m_upperProjectedIntersection->setSecondPoint( secondIntersectionPoint );
            // linia de background
            m_backgroundUpperProjectedIntersection->setFirstPoint( firstIntersectionPoint );
            m_backgroundUpperProjectedIntersection->setSecondPoint( secondIntersectionPoint );

            m_2DViewer->getDrawer()->showGroup("ReferenceLines");
        }
        else
        {
            m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
        }

        QList< QVector<double> > lowerPlaneBounds = referencePlane->getLowerBounds();
        numberOfIntersections = this->getIntersections( lowerPlaneBounds.at(0), lowerPlaneBounds.at(1), lowerPlaneBounds.at(2), lowerPlaneBounds.at(3), localizerPlane, firstIntersectionPoint, secondIntersectionPoint );

        // un cop tenim les interseccions nomes cal projectar-les i pintar la linia
        DEBUG_LOG(" ======== Nombre d'interseccions entre plans: " +  QString::number( numberOfIntersections ) );
        if( numberOfIntersections == 2 )
        {
            m_2DViewer->projectPointToCurrentDisplayedImage( firstIntersectionPoint, firstIntersectionPoint );
            m_2DViewer->projectPointToCurrentDisplayedImage( secondIntersectionPoint, secondIntersectionPoint );

            // linia discontinua
            m_lowerProjectedIntersection->setFirstPoint( firstIntersectionPoint );
            m_lowerProjectedIntersection->setSecondPoint( secondIntersectionPoint );
            // linia de background
            m_backgroundLowerProjectedIntersection->setFirstPoint( firstIntersectionPoint );
            m_backgroundLowerProjectedIntersection->setSecondPoint( secondIntersectionPoint );

            m_2DViewer->getDrawer()->showGroup("ReferenceLines");
        }
    }
}

int ReferenceLinesTool::getIntersections( QVector<double> tlhc, QVector<double> trhc, QVector<double> brhc, QVector<double> blhc, ImagePlane *localizerPlane, double firstIntersectionPoint[3], double secondIntersectionPoint[3] )
{
    double t;
    int numberOfIntersections = 0;
    double localizerNormalVector[3], localizerOrigin[3];
    localizerPlane->getNormalVector( localizerNormalVector );
    localizerPlane->getOrigin( localizerOrigin );
    if( vtkPlane::IntersectWithLine( (double *)tlhc.data(), (double *)trhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint ) )
    {
        DEBUG_LOG( QString("Segment P1-P2 intersecciona en el punt: %1,%2,%3").arg( firstIntersectionPoint[0] ).arg( firstIntersectionPoint[1] ).arg( firstIntersectionPoint[2] ) );
        numberOfIntersections = 1;

        if( vtkPlane::IntersectWithLine( (double *)brhc.data(), (double *)blhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint ) )
        {
            DEBUG_LOG( QString("Segment P3-P4 intersecciona en el punt: %1,%2,%3").arg( secondIntersectionPoint[0] ).arg( secondIntersectionPoint[1] ).arg( secondIntersectionPoint[2] ) );
            numberOfIntersections = 2;
        }
    }
    else if( vtkPlane::IntersectWithLine( (double *)trhc.data(), (double *)brhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint ) )
    {
        DEBUG_LOG( QString("Segment P2-P3 intersecciona en el punt: %1,%2,%3").arg( firstIntersectionPoint[0] ).arg( firstIntersectionPoint[1] ).arg( firstIntersectionPoint[2] ) );
        numberOfIntersections = 1;

        if( vtkPlane::IntersectWithLine( (double *)blhc.data(), (double *)tlhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint ) )
        {
            DEBUG_LOG( QString("Segment P4-P1 intersecciona en el punt: %1,%2,%3").arg( secondIntersectionPoint[0] ).arg( secondIntersectionPoint[1] ).arg( secondIntersectionPoint[2] ) );
            numberOfIntersections = 2;
        }
    }
    return numberOfIntersections;
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
