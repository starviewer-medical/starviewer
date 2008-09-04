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
 : Tool(viewer, parent), m_projectedReferencePlane(0), m_showPlaneThickness(true), m_planesToProject(SingleImage)
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

// descomentar aquestes 4 linies si es vol mostrar el poligon del pla projectat
//      m_projectedReferencePlane = new DrawerPolygon;
//     // HACK sucedani d'smart pointer(TM)
//     m_projectedReferencePlane->increaseReferenceCount();
//     m_2DViewer->getDrawer()->draw( m_projectedReferencePlane, QViewer::Top2DPlane );
//     m_2DViewer->getDrawer()->addToGroup( m_projectedReferencePlane, "ReferenceLines" );

    refreshReferenceViewerData();

    // cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *) ), SLOT( refreshReferenceViewerData() ) );
    // cada cop que el viewer canvïi de llesca, hem d'actualitzar el pla de projecció
    connect( m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateImagePlane()) );
    // cada cop que canvii l'slab thickness haurem d'actualitzar els plans a projectar
    connect( m_2DViewer, SIGNAL(slabThicknessChanged(int)), SLOT(updateImagePlane()) );

    connect( m_2DViewer, SIGNAL(selected()),SLOT(refreshReferenceViewerData()) );
}

ReferenceLinesTool::~ReferenceLinesTool()
{
// descomentar aquestes 4 linies si es vol mostrar el poligon del pla projectat(nomes debug)
//     // ja no som propietaris de les línies creades
//     m_projectedReferencePlane->decreaseReferenceCount();
//     // ara al fer delete, s'esborraran automàticament del drawer, ja que nosaltres érem els únics propietaris
//     delete m_projectedReferencePlane;
    // Cal esborrar les línies que hàgim creat
    foreach( DrawerLine *line, m_projectedIntersectionLines )
    {
        // HACK sucedani d'smart pointer(TM)
        line->decreaseReferenceCount();
        delete line;
    }
    foreach( DrawerLine *backgroundLine, m_backgroundProjectedIntersectionLines )
    {
        // HACK sucedani d'smart pointer(TM)
        backgroundLine->decreaseReferenceCount();
        delete backgroundLine;
    }
}

void ReferenceLinesTool::setToolData(ToolData * data)
{
    m_toolData = data;
    m_myData = qobject_cast<ReferenceLinesToolData *>(data);
    // quan canvïn les dades (ImagePlane), actualitzem les línies de projecció
    connect( m_toolData, SIGNAL(changed()), SLOT(updateProjectionLines()) );

    // això serveix perquè s'apliqui tot just quan es creïi la tool
    // amb múltiples viewers
    if( m_2DViewer->isActive() )
    {
        refreshReferenceViewerData();
    }
    updateProjectionLines();
}

void ReferenceLinesTool::updateProjectionLines()
{
    // en cas que no sigui el viewer que estem modificant i que tingui input
    if( !m_2DViewer->isActive() && m_2DViewer->getInput() )
    {
        // intentarem projectar el pla que hi ha a m_myData
        // primer cal que comparteixin el mateix FrameOfReference
        if( m_myFrameOfReferenceUID == m_myData->getFrameOfReferenceUID() )
        {
            QList<ImagePlane *> planesToProject = m_myData->getPlanesToProject();
            // primer comprovar si tenim el nombre adequat de linies creades, donat el nombre de plans a projectar
            checkAvailableLines();
            if( planesToProject.count() == 0 )
            {
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
            }
            else
            {
                int drawerLineOffset = 0;
                foreach( ImagePlane *referencePlane, planesToProject )
                {
                    // aquí ja ho deixem en mans de la projecció
                    projectIntersection( referencePlane, m_2DViewer->getCurrentImagePlane(), drawerLineOffset );
                    drawerLineOffset += m_showPlaneThickness ? 2 : 1;
                }
            }
        }
        else
        {
            m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
        }
    }
    else
    {
        m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    }
}

void ReferenceLinesTool::projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane, int drawerLineOffset)
{
    if( !(referencePlane && localizerPlane) )
        return;

    // primer mirem que siguin plans diferents
    if( *localizerPlane != *referencePlane )
    {
        //
        // projecció de la intersecció dels plans
        //
        /// llegir http://fixunix.com/dicom/51195-scanogram-lines-mr.html

        // recollim les dades del pla del localitzador sobre el qual volem projectar el de referència
        double localizerNormalVector[3], localizerOrigin[3];
        localizerPlane->getNormalVector( localizerNormalVector );
        localizerPlane->getOrigin( localizerOrigin );

        //
        // TODO mirar exactament quan cal amagar les línies i quan no, depenent de les interseccions trobades
        //
        // un cop tenim les interseccions nomes cal projectar-les i pintar la linia

        if( m_showPlaneThickness )
        {
            // calculem totes les possibles interseccions
            QList< QVector<double> > upperPlaneBounds = referencePlane->getUpperBounds();
            double firstIntersectionPoint[3], secondIntersectionPoint[3];

            int numberOfIntersections = this->getIntersections( upperPlaneBounds.at(0), upperPlaneBounds.at(1), upperPlaneBounds.at(2), upperPlaneBounds.at(3), localizerPlane, firstIntersectionPoint, secondIntersectionPoint );
            if( numberOfIntersections > 0 )
            {
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage( firstIntersectionPoint, firstIntersectionPoint );
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage( secondIntersectionPoint, secondIntersectionPoint );

                // linia discontinua
                m_projectedIntersectionLines[ drawerLineOffset ]->setFirstPoint( firstIntersectionPoint );
                m_projectedIntersectionLines[ drawerLineOffset ]->setSecondPoint( secondIntersectionPoint );
                // linia de background
                m_backgroundProjectedIntersectionLines[ drawerLineOffset ]->setFirstPoint( firstIntersectionPoint );
                m_backgroundProjectedIntersectionLines[ drawerLineOffset ]->setSecondPoint( secondIntersectionPoint );

                m_2DViewer->getDrawer()->showGroup("ReferenceLines");
            }
            else
            {
                // TODO en comptes de fer un hide, posar valors 0,0 a cada coordenada perquè no afecti a altres línies que sí interecten?
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
            }

            QList< QVector<double> > lowerPlaneBounds = referencePlane->getLowerBounds();
            numberOfIntersections = this->getIntersections( lowerPlaneBounds.at(0), lowerPlaneBounds.at(1), lowerPlaneBounds.at(2), lowerPlaneBounds.at(3), localizerPlane, firstIntersectionPoint, secondIntersectionPoint );

            // un cop tenim les interseccions nomes cal projectar-les i pintar la linia
            if( numberOfIntersections > 0 )
            {
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage( firstIntersectionPoint, firstIntersectionPoint );
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage( secondIntersectionPoint, secondIntersectionPoint );

                // linia discontinua
                m_projectedIntersectionLines[ drawerLineOffset+1 ]->setFirstPoint( firstIntersectionPoint );
                m_projectedIntersectionLines[ drawerLineOffset+1 ]->setSecondPoint( secondIntersectionPoint );
                // linia de background
                m_backgroundProjectedIntersectionLines[ drawerLineOffset+1 ]->setFirstPoint( firstIntersectionPoint );
                m_backgroundProjectedIntersectionLines[ drawerLineOffset+1 ]->setSecondPoint( secondIntersectionPoint );

                m_2DViewer->getDrawer()->showGroup("ReferenceLines");
            }
            else
            {
                // TODO en comptes de fer un hide, posar valors 0,0 a cada coordenada perquè no afecti a altres línies que sí interecten?
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
                // si no hi ha cap intersecció apliquem el pla directament, "a veure què"
                //TODO això és per debug ONLY!!
    //             projectPlane( referencePlane );
            }
        }
        else // nomes agafem el pla "central"
        {
            QList< QVector<double> > planeBounds = referencePlane->getCentralBounds();
            double firstIntersectionPoint[3], secondIntersectionPoint[3];

            int numberOfIntersections = this->getIntersections( planeBounds.at(0), planeBounds.at(1), planeBounds.at(2), planeBounds.at(3), localizerPlane, firstIntersectionPoint, secondIntersectionPoint );
            if( numberOfIntersections > 0 )
            {
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage( firstIntersectionPoint, firstIntersectionPoint );
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage( secondIntersectionPoint, secondIntersectionPoint );

                // linia discontinua
                m_projectedIntersectionLines[ drawerLineOffset ]->setFirstPoint( firstIntersectionPoint );
                m_projectedIntersectionLines[ drawerLineOffset ]->setSecondPoint( secondIntersectionPoint );
                // linia de background
                m_backgroundProjectedIntersectionLines[ drawerLineOffset ]->setFirstPoint( firstIntersectionPoint );
                m_backgroundProjectedIntersectionLines[ drawerLineOffset ]->setSecondPoint( secondIntersectionPoint );

                m_2DViewer->getDrawer()->showGroup("ReferenceLines");
            }
            else
            {
                // TODO en comptes de fer un hide, posar valors 0,0 a cada coordenada perquè no afecti a altres línies que sí interecten?
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
            }
        }
    }
    else
    {
        m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    }
}

void ReferenceLinesTool::projectPlane(ImagePlane *planeToProject)
{
    QList< QVector<double> > planeBounds = planeToProject->getCentralBounds();
    double projectedVertix1[3],projectedVertix2[3],projectedVertix3[3],projectedVertix4[3];

    m_2DViewer->projectDICOMPointToCurrentDisplayedImage( (double *)planeBounds.at(0).data(), projectedVertix1 );
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage( (double *)planeBounds.at(1).data(), projectedVertix2 );
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage( (double *)planeBounds.at(2).data(), projectedVertix3 );
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage( (double *)planeBounds.at(3).data(), projectedVertix4 );

    // donem els punts al poligon a dibuixar
    m_projectedReferencePlane->setVertix( 0, projectedVertix1 );
    m_projectedReferencePlane->setVertix( 1, projectedVertix2 );
    m_projectedReferencePlane->setVertix( 2, projectedVertix3 );
    m_projectedReferencePlane->setVertix( 3, projectedVertix4 );
    m_2DViewer->getDrawer()->showGroup("ReferenceLines");
}

int ReferenceLinesTool::getIntersections( QVector<double> tlhc, QVector<double> trhc, QVector<double> brhc, QVector<double> blhc, ImagePlane *localizerPlane, double firstIntersectionPoint[3], double secondIntersectionPoint[3] )
{
    double t;
    int numberOfIntersections = 0;
    double localizerNormalVector[3], localizerOrigin[3];
    localizerPlane->getNormalVector( localizerNormalVector );
    localizerPlane->getOrigin( localizerOrigin );

    // Primera "paral·lela"
    if( vtkPlane::IntersectWithLine( (double *)tlhc.data(), (double *)trhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint ) )
        numberOfIntersections++;
    if( vtkPlane::IntersectWithLine( (double *)brhc.data(), (double *)blhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint ) )
        numberOfIntersections++;

    if( numberOfIntersections == 0 ) // provar amb la segona "paral·lela"
    {
        if( vtkPlane::IntersectWithLine( (double *)trhc.data(), (double *)brhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint ) )
            numberOfIntersections++;

        if( vtkPlane::IntersectWithLine( (double *)blhc.data(), (double *)tlhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint ) )
            numberOfIntersections++;
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
    if( !m_2DViewer->isActive() )
        return;

    switch( m_planesToProject )
    {
    case SingleImage:
        if( m_2DViewer->isThickSlabActive() )
        {
            QList<ImagePlane *> planes;
            planes << m_2DViewer->getCurrentImagePlane();
            planes << m_2DViewer->getImagePlane( m_2DViewer->getCurrentSlice() + m_2DViewer->getSlabThickness() - 1, m_2DViewer->getCurrentPhase() );

            m_myData->setPlanesToProject( planes );
        }
        else
        {
            m_myData->setPlanesToProject( m_2DViewer->getCurrentImagePlane() );
        }
    break;

    case AllImages:
        // TODO implementar
    break;
    }

}

void ReferenceLinesTool::refreshReferenceViewerData()
{
    // si es projectaven plans sobre el nostre drawer, els amaguem
    m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    if( m_2DViewer->getInput() )
    {
        updateFrameOfReference();
        updateImagePlane();
    }
}

DrawerLine *ReferenceLinesTool::createNewLine( bool isBackgroundLine )
{
    DrawerLine *line = new DrawerLine;
    // HACK sucedani d'smart pointer(TM)
    line->increaseReferenceCount();

    if( isBackgroundLine )
        line->setColor( QColor(0,0,0) );
    else
    {
        line->setColor( QColor(255,160,0) );
        line->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
    }

    return line;
}

void ReferenceLinesTool::checkAvailableLines()
{
    int planesToProject = m_myData->getPlanesToProject().count();
    int neededLines = planesToProject;
    if( m_showPlaneThickness )
        neededLines *= 2;

    // llistat de linies de background
    int numberOfLines = m_backgroundProjectedIntersectionLines.count();

    // ara comprovem pels dos llistats que estiguin be
    if( neededLines > numberOfLines )
    {
        int linesToCreate = neededLines - numberOfLines;
        // cal crear linies noves
        for( int i=0; i<linesToCreate; i++ )
        {
            DrawerLine *line = createNewLine( true );
            m_2DViewer->getDrawer()->draw( line, QViewer::Top2DPlane );
            m_2DViewer->getDrawer()->addToGroup( line, "ReferenceLines" );
            m_backgroundProjectedIntersectionLines << line;
        }
    }
    else if( neededLines < numberOfLines )
    {
        int linesToRemove = numberOfLines - neededLines;
        // tenim mes linies de les necessaries, cal esborrar
        for( int i=0; i<linesToRemove; i++ )
        {
            DrawerLine *line = m_backgroundProjectedIntersectionLines.takeLast();
            // ja no som propietaris de les línies creades, HACK sucedani d'smart pointer(TM)
            line->decreaseReferenceCount();
            delete line;
        }
    }

    // llistat de linies "principals"
    numberOfLines = m_projectedIntersectionLines.count();

    // ara comprovem pels dos llistats que estiguin be
    if( neededLines > numberOfLines )
    {
        int linesToCreate = neededLines - numberOfLines;
        // cal crear linies noves
        for( int i=0; i<linesToCreate; i++ )
        {
            DrawerLine *line = createNewLine();
            m_2DViewer->getDrawer()->draw( line, QViewer::Top2DPlane );
            m_2DViewer->getDrawer()->addToGroup( line, "ReferenceLines" );
            m_projectedIntersectionLines << line;
        }
    }
    else if( neededLines < numberOfLines )
    {
        int linesToRemove = numberOfLines - neededLines;
        // tenim mes linies de les necessaries, cal esborrar
        for( int i=0; i<linesToRemove; i++ )
        {
            DrawerLine *line = m_projectedIntersectionLines.takeLast();
            // ja no som propietaris de les línies creades, HACK sucedani d'smart pointer(TM)
            line->decreaseReferenceCount();
            delete line;
        }
    }


}

}
