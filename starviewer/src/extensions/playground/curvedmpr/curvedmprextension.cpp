/***************************************************************************
 *   Copyright (C) 2010 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "curvedmprextension.h"

#include "curvedmprsettings.h"
#include "toolmanager.h"
#include "volume.h"
#include "toolproxy.h"
#include "tool.h"
#include "linepathtool.h"
#include "mathtools.h"
#include "drawerpolyline.h"
#include "image.h"
// Vtk's
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkCell.h>

namespace udg {

CurvedMPRExtension::CurvedMPRExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    CurvedMPRSettings().init();
    
    // TODO corretgir aquesta inicialització inicial, això no hauria de ser necessari
    m_viewersLayout->addViewer( "0.0\\1.0\\1.0\\0.0" );
    m_viewersLayout->setGrid(2,1);
    
    initializeTools();

    // Quan l'usuari indiqui la línia sobre la que caldrà projectar, llavors s'iniciarà el procés
    // de creació del reslicedVolume que caldrà visualitzar al segon viewer.
    ToolProxy *toolProxy = m_viewersLayout->getViewerWidget(0)->getViewer()->getToolProxy();
    LinePathTool *linePathTool = qobject_cast<LinePathTool *>( toolProxy->getTool( "LinePathTool" ) );
    connect( linePathTool, SIGNAL( finished( QPointer<DrawerPolyline> ) ), SLOT( updateReslice( QPointer<DrawerPolyline> )) );
}

CurvedMPRExtension::~CurvedMPRExtension()
{
}

void CurvedMPRExtension::initializeTools()
{
    // Creem el tool manager
    m_toolManager = new ToolManager(this);
    // Obtenim les accions de cada tool que volem
    m_scrollToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_distanceToolButton->setDefaultAction( m_toolManager->registerTool("DistanceTool") );
    m_polylineROIToolButton->setDefaultAction( m_toolManager->registerTool("PolylineROITool") );
    m_eraserToolButton->setDefaultAction( m_toolManager->registerTool("EraserTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_linePathToolButton->setDefaultAction( m_toolManager->registerTool("LinePathTool") );
    // Tools sense botó
    m_toolManager->registerTool("WindowLevelTool");
    m_toolManager->registerTool("TranslateTool");
    m_toolManager->registerTool("SlicingKeyboardTool");
    m_toolManager->registerTool("WindowLevelPresetsTool");
    // and so on...

    // Definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "DistanceTool" << "PolylineROITool" << "EraserTool" << "LinePathTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingTool" << "WindowLevelTool" << "TranslateTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools(defaultTools);

    // Registrem al manager les tools que van als diferents viewers
    m_toolManager->setupRegisteredTools( m_viewersLayout->getViewerWidget(0)->getViewer() );
    m_toolManager->setupRegisteredTools( m_viewersLayout->getViewerWidget(1)->getViewer() );

    // Activem per defecte la tool LinePathTool
    m_toolManager->getRegisteredToolAction("LinePathTool")->trigger();
}

void CurvedMPRExtension::setInput( Volume *input )
{
    m_viewersLayout->getViewerWidget(0)->getViewer()->setInput(input);
    m_viewersLayout->getViewerWidget(1)->getViewer()->setInput(input);
}

void CurvedMPRExtension::updateReslice( QPointer<DrawerPolyline> polyline )
{
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    Volume *volume = mainViewer->getInput();

    // Obtenim volum de dades del segon viewer
    Q2DViewer *reconstructionViewer = m_viewersLayout->getViewerWidget(1)->getViewer();
    Volume *reslicedVolume = reconstructionViewer->getInput();

    // Es porta a terme l'MPR Curvilini per obtenir el nou volum amb la reconstrucció
    doCurvedReslice( volume, polyline, reslicedVolume );

    // Visualitzem al segon viewer la reconstrucció assignada al seu volum de dades
    reconstructionViewer->render();
}

void CurvedMPRExtension::doCurvedReslice( Volume *volume, QPointer<DrawerPolyline> polyline, Volume *reslicedVolume )
{
    // Distància entre els píxels de les imatges que formen el volum
    double spacing[3];
    volume->getSpacing( spacing );
    double pixelsDistance = sqrt( spacing[0] * spacing[0] + spacing[1] * spacing[1] );

    // Es construeix una llista amb tots els punts que hi ha sobre la polyline indicada per
    // l'usuari i que cal tenir en compte al fer la reconstrucció
    QList< double* > *pointsPath = new QList< double* >;
    getPointsPath( polyline, pixelsDistance, pointsPath );

    // S'inicialitzen i s'emplenen les dades VTK que han de formar el volum de la reconstrucció.
    vtkImageData *imageDataVTK = vtkImageData::New();
    initAndFillImageDataVTK( volume, pointsPath, imageDataVTK );
    
    // S'assignen les dades VTK al nou volum
    reslicedVolume->setData( imageDataVTK );
}

void CurvedMPRExtension::getPointsPath( QPointer<DrawerPolyline> polyline, double pixelsDistance, QList< double * > *pointsPath )
{
    QList< double* > pointsList = polyline->getPointsList();
    double *previousPoint;
    double *currentPoint;
    double *newLinePoint;
    
    //DEBUG_LOG(QString("PolyLine points size = %1").arg(pointsList.length()));

    for ( int i = 0; i < pointsList.size(); i++)
    {
        currentPoint = pointsList.at(i);
        
        if ( i > 0 )
        {
            // S'anoten tots els punts que hi ha sobre la línia que formen el punt anterior 
            // i l'actual de la polyline indicada per l'usuari
            // La distància entre aquests punts per defecte serà la distància entre píxels

            double *directorVector = MathTools::directorVector( previousPoint, currentPoint );
            double distanceLine = MathTools::getDistance3D( previousPoint, currentPoint );
            double numPointsLine = distanceLine / pixelsDistance;

            MathTools::normalize( directorVector );

            //DEBUG_LOG(QString("DirectorVector [%1,%2,%3]").arg(directorVector[0]).arg(directorVector[1]).arg(directorVector[2]));

            //DEBUG_LOG(QString("numPointsLine = %1").arg(numPointsLine));

            for (int j = 0; j < numPointsLine - 1; j++)
            {
                newLinePoint = new double[3];
                newLinePoint[0] = previousPoint[0] + directorVector[0] * pixelsDistance * (j + 1);
                newLinePoint[1] = previousPoint[1] + directorVector[1] * pixelsDistance * (j + 1);
                newLinePoint[2] = previousPoint[2] + directorVector[2] * pixelsDistance * (j + 1);

                //DEBUG_LOG(QString("New line point [%1,%2,%3]").arg(newLinePoint[0]).arg(newLinePoint[1]).arg(newLinePoint[2]));

                pointsPath->append( newLinePoint );
            }
        }

        // S'inclou a la llista el punt actual
        if (i == 0 || previousPoint[0] != currentPoint[0] || previousPoint[1] != currentPoint[1] || previousPoint[2] != currentPoint[2] )
        {
            //DEBUG_LOG(QString("append current point [%1,%2,%3]").arg(currentPoint[0]).arg(currentPoint[1]).arg(currentPoint[2]));
            pointsPath->append( currentPoint );
        }

        previousPoint = currentPoint;
    }

    /*for ( int i = 0; i < pointsPath->size(); i++ )
    {
        double *point = pointsPath->at( i );
        DEBUG_LOG(QString("Point path [%1,%2,%3]").arg(point[0]).arg(point[1]).arg(point[2]));
    }*/
}

void CurvedMPRExtension::initAndFillImageDataVTK( Volume * volume, QList< double* > *pointsPath, vtkImageData *imageDataVTK )
{
    // Inicialització les dades VTK que formaran el volum de la reconstrucció.
    /*
    imageDataVTK->SetOrigin( .0, .0, .0 );
    imageDataVTK->SetSpacing( 1., 1., 1. );
    imageDataVTK->SetDimensions( 10, 10, 1 );
    imageDataVTK->SetWholeExtent( 0, 9, 0, 9, 0, 0 );
    imageDataVTK->SetScalarTypeToShort();
    imageDataVTK->SetNumberOfScalarComponents(1);
    imageDataVTK->AllocateScalars();
    */

    double maxX = (double) pointsPath->length();
    QList<Image*> slices = volume->getImages();
    double maxY = (double) slices.length();

    imageDataVTK->SetOrigin( .0, .0, .0 );
    imageDataVTK->SetSpacing( 1., 1., 1. ); //???
    imageDataVTK->SetDimensions( maxX, maxY, 1 );
    imageDataVTK->SetWholeExtent( 0, maxX, 0, maxY, 0, 1 );
    imageDataVTK->SetScalarTypeToShort();
    imageDataVTK->SetNumberOfScalarComponents(1);
    imageDataVTK->AllocateScalars();

    // S'emplena el dataset del volum amb el valor dels píxels resultat de projectar en profunditat
    // la línia indicada per l'usuari

    // Píxels que formen la imatge i dels quals modificarem el valor
    signed short * scalarPointer = (signed short *) imageDataVTK->GetScalarPointer();

    //DEBUG_LOG(QString("maxY %1").arg(maxY));
    //DEBUG_LOG(QString("maxX %1").arg(maxX));

    // Per cada llesca del volum (tantes files com número d'imatges)
    QListIterator<Image*> it(slices);
    while (it.hasNext())
    {
        Image *slice = it.next();
        double depth = slice->getImagePositionPatient()[2];

        // Obtenim el valor del pixel a la llesca actual per tots els punts indicats per 
        // l'usuari i que formen les columnes de la imatge resultat
        for ( int x = 0; x < pointsPath->size(); x++ )
        {
            double *point = pointsPath->at(x);
        
            Volume::VoxelType voxelValue;
            
            // Es calcula el valor del voxel allà on es troba el punt actual a la llesca actual
            point[2] = depth;

            //DEBUG_LOG(QString("Point imatge [%1,%2,%3]").arg(point[0]).arg(point[1]).arg(point[2]));
            if ( volume->getVoxelValue(point, voxelValue) )
            {
                //DEBUG_LOG(QString("Valor pixel %1").arg(voxelValue));
                // S'afegeix el valor del píxel a les dades VTK
                *scalarPointer = voxelValue;
                *scalarPointer++;
            }
        }
    }
}

}; // end namespace udg


