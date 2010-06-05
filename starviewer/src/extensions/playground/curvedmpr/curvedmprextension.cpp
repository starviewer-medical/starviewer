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
#include "drawer.h"
#include "drawerpolyline.h"
#include "image.h"
// Vtk's
#include <vtkImageData.h>

namespace udg {

CurvedMPRExtension::CurvedMPRExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    CurvedMPRSettings().init();
    
    // TODO corretgir aquesta inicialització inicial, això no hauria de ser necessari
    m_viewersLayout->addViewer( "0.0\\1.0\\1.0\\0.0" );
    m_viewersLayout->setGrid(2,1);
    // Al segon viewer només li deixarem posar l'input del reslice curvilini. 
    // L'usuari no el podrà canviar amb un volum "regular"
    m_viewersLayout->getViewerWidget(1)->getViewer()->disableContextMenu();

    initializeTools();

    // Cada cop que es canvia l'input del viewer principal cal actualitzar el volum de treball
    connect( m_viewersLayout->getViewerWidget(0)->getViewer(), SIGNAL( volumeChanged( Volume * ) ), SLOT( updateMainVolume( Volume * ) ) );

    // Cada cop que es canvia el viewer seleccionat s'habiliten les tools en aquest visor
    // i es deshabiliten de l'altre
    connect( m_viewersLayout, SIGNAL( viewerSelectedChanged( Q2DViewerWidget * ) ), SLOT( changeSelectedViewer( Q2DViewerWidget * ) ) );

    // A l'input on l'usuari pot indicar el gruix de la reconstrucció només ha d'acceptar números entre 1 i X 
    // TODO El màxim correspondrà al número de files o columnes del volum original
    QValidator *validator = new QIntValidator(1, 1000, this);
    m_thickReconstruction->setValidator(validator);
    m_thickReconstruction->setText( "1" );

    m_numImages = 1;
    m_maxThickness = 0;

    // Cada cop que l'usuari modifiqui el gruix indicat per fer el MIP, es torna a fer
    // la reconstrucció al visor corresponent, tenint en compte la última línia indicada per l'usuari
    connect( m_thickReconstruction, SIGNAL( returnPressed () ), SLOT( changeThicknessReconstruction() ) );
}

CurvedMPRExtension::~CurvedMPRExtension()
{
    if ( !m_upPolylineThickness.isNull() )
    {
        m_upPolylineThickness->decreaseReferenceCount();
        delete m_upPolylineThickness;
    }

    if ( !m_downPolylineThickness.isNull() )
    {
        m_downPolylineThickness->decreaseReferenceCount();
        delete m_downPolylineThickness;
    }
}

void CurvedMPRExtension::initializeTools()
{
    // Creem el tool manager
    m_toolManager = new ToolManager(this);
    // Obtenim les accions de cada tool que volem
    m_slicingToolButton->setDefaultAction( m_toolManager->registerTool("SlicingTool") );
    m_zoomToolButton->setDefaultAction( m_toolManager->registerTool("ZoomTool") );
    m_distanceToolButton->setDefaultAction( m_toolManager->registerTool("DistanceTool") );
    m_polylineROIToolButton->setDefaultAction( m_toolManager->registerTool("PolylineROITool") );
    m_eraserToolButton->setDefaultAction( m_toolManager->registerTool("EraserTool") );
    m_voxelInformationToolButton->setDefaultAction( m_toolManager->registerTool("VoxelInformationTool") );
    m_linePathToolButton->setDefaultAction( m_toolManager->registerTool("LinePathTool") );
    m_angleToolButton->setDefaultAction( m_toolManager->registerTool( "AngleTool" ) );
    m_openAngleToolButton->setDefaultAction( m_toolManager->registerTool( "NonClosedAngleTool" ) );
    m_axialViewToolButton->setDefaultAction( m_toolManager->registerActionTool( "AxialViewActionTool" ) );
    m_sagitalViewToolButton->setDefaultAction( m_toolManager->registerActionTool( "SagitalViewActionTool" ) );
    m_coronalViewToolButton->setDefaultAction( m_toolManager->registerActionTool( "CoronalViewActionTool" ) );
    m_rotateClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool( "RotateClockWiseActionTool" ) );
    m_rotateCounterClockWiseToolButton->setDefaultAction( m_toolManager->registerActionTool( "RotateCounterClockWiseActionTool" ) );
    m_flipHorizontalToolButton->setDefaultAction( m_toolManager->registerActionTool( "HorizontalFlipActionTool" ) );
    m_flipVerticalToolButton->setDefaultAction( m_toolManager->registerActionTool( "VerticalFlipActionTool" ) );
    m_restoreToolButton->setDefaultAction( m_toolManager->registerActionTool( "RestoreActionTool" ) );
    m_invertToolButton->setDefaultAction( m_toolManager->registerActionTool( "InvertWindowLevelActionTool" ) );

    // Tools sense botó
    m_toolManager->registerTool("WindowLevelTool");
    m_toolManager->registerTool("TranslateTool");
    m_toolManager->registerTool("SlicingKeyboardTool");
    m_toolManager->registerTool("WindowLevelPresetsTool");
    // and so on...

    // Definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "DistanceTool" << "PolylineROITool" << "EraserTool" << "LinePathTool" << "AngleTool" << "NonClosedAngleTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingTool" << "WindowLevelTool" << "TranslateTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools(defaultTools);

    // Registrem al manager les tools que van als diferents viewers
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    Q2DViewer *reconstructionViewer = m_viewersLayout->getViewerWidget(1)->getViewer();
    m_toolManager->setupRegisteredTools( mainViewer );
    m_toolManager->setupRegisteredTools( reconstructionViewer );
    m_toolManager->enableRegisteredActionTools( mainViewer );

    m_thickSlabWidget->link( reconstructionViewer );

    // Cal assabentar-se cada cop que es creï aquesta tool
    // HACK Això és un hack!!! És una manera de fer que cada cop que cliquem el botó de l'eina se'ns connecti
    // el signal que ens diu quina és la figura que s'ha dibuixat i així poder fer l'MPR Curvilini
    // Fer servir un signal triggered o toggled de l'acció de la tool no funciona ja que primer s'executaria
    // el nostre slot i després es crearia la tool i per tant el nostre slot sempre obtindria una Tool nul·la
    // TODO Caldria trobar una manera de que el toolmanager ens digués quan es crea una tool d'un o varis viewers
    // per situacions com aquesta
    // Tenir en compte que amb aquest mètode, si s'activa la tool per shortcut, el més segur és que no funcionarà!!!
    connect( m_linePathToolButton, SIGNAL( clicked(bool) ), SLOT( updateLinePathToolConnection(bool) ) );
    // Activem per defecte la tool LinePathTool
    m_linePathToolButton->click();
}

void CurvedMPRExtension::setInput( Volume *input )
{
    m_viewersLayout->getViewerWidget(0)->getViewer()->setInput(input);
}

void CurvedMPRExtension::updateMainVolume( Volume *volume )
{
    m_mainVolume = volume;
}

void CurvedMPRExtension::changeThicknessReconstruction()
{
    m_numImages = m_thickReconstruction->text().toInt();

    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();

    if ( m_numImages && m_numImages > 1 )
    {
        // Obtenim quin és l'eix y (de desplaçament) segons la vista mostrada al visor principal
        int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );

        // Distància entre els píxels de les imatges que formen el volum
        double spacing[3];
        m_mainVolume->getSpacing( spacing );

        // Calculem la distància entre la primera i la úlltima imatge que composaran la reconstrucció
        m_maxThickness = m_numImages * spacing[yIndex];
    }
    else
    {
        m_numImages = 1;
        m_maxThickness = 0;
    }

    // Per fer la reconstrucció cal que l'usuari anteriorment hagi indicat una línia
    if ( !m_lastPointsPath.isEmpty() )
    {
        // Inicialitzem o netegem les línies que mostraran al viewer principal quins són els
        // límits de les imatges que es reconstruiran pel nou volum
        if (!m_upPolylineThickness )
        {
            // Línia thickness superior
            m_upPolylineThickness = new DrawerPolyline;
            m_upPolylineThickness->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
            m_upPolylineThickness->setColor( Qt::blue );
            m_upPolylineThickness->setLineWidth( 1 );
            mainViewer->getDrawer()->draw( m_upPolylineThickness , mainViewer->getView(), mainViewer->getCurrentSlice() );

            // Línia thickness inferior
            m_downPolylineThickness = new DrawerPolyline;
            m_downPolylineThickness->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
            m_downPolylineThickness->setColor( Qt::blue );
            m_downPolylineThickness->setLineWidth( 1 );
            mainViewer->getDrawer()->draw( m_downPolylineThickness , mainViewer->getView(), mainViewer->getCurrentSlice() );

            // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
            m_upPolylineThickness->increaseReferenceCount();
            m_downPolylineThickness->increaseReferenceCount();
        }
        else
        {
            m_upPolylineThickness->deleteAllPoints();
            m_upPolylineThickness->update();

            m_downPolylineThickness->deleteAllPoints();
            m_downPolylineThickness->update();

            mainViewer->render();
        }
        
        // Portem a terme la reconstrucció
        updateResliceWithLastPointsPath();
    }
}

void CurvedMPRExtension::updateReslice( QPointer<DrawerPolyline> polyline )
{
    updateReslice( polyline, true );
}

void CurvedMPRExtension::updateResliceWithLastPointsPath()
{
    this->setCursor( QCursor( Qt::WaitCursor ) );
    updateReslice( 0, false );
    this->setCursor( QCursor( Qt::ArrowCursor ) );
}


void CurvedMPRExtension::updateReslice( QPointer<DrawerPolyline> polyline, bool calculatePointsPath )
{
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    Q2DViewer *reconstructionViewer = m_viewersLayout->getViewerWidget(1)->getViewer();

    // Assignem al cursor l'estat wait
    mainViewer->setCursor( QCursor( Qt::WaitCursor ) );
    reconstructionViewer->setCursor( QCursor( Qt::WaitCursor ) );

    // Es porta a terme l'MPR Curvilini per obtenir el nou volum amb la reconstrucció
    Volume *reslicedVolume = doCurvedReslice( polyline, calculatePointsPath );

    // Visualitzem al segon viewer la reconstrucció del nou volum de dades obtingut
    reconstructionViewer->setInput( reslicedVolume );
    reconstructionViewer->render();

    // Tornem cursor a l'estat normal
    mainViewer->setCursor( QCursor( Qt::ArrowCursor ) );
    reconstructionViewer->setCursor( QCursor( Qt::ArrowCursor ) );
}

Volume* CurvedMPRExtension::doCurvedReslice( QPointer<DrawerPolyline> polyline, bool calculatePointsPath )
{
    Q_ASSERT(m_mainVolume);

    QList<double *> pointsPath;
    if ( calculatePointsPath )
    {
        // Es construeix una llista amb tots els punts que hi ha sobre la polyline indicada per
        // l'usuari i que cal tenir en compte al fer la reconstrucció
        pointsPath = getPointsPath( polyline );
    }

    // Cal tenir en compte que si ens han indicat que es vol reconstruir un volum amb més d'una imatge
    // els punts de la línia indicada per l'usuari seran desplaçats
    // per equivaldre als punts del pla de projecció que representarà la primera imatge.  
    // Després s'aplicarà el desplaçament corresponent a aquests punts per generar les
    // successives imatges.
    if ( m_numImages > 1 )
    {
       pointsPath = getLastPointsPathForFirstImage();
       showThichkessPolylines();
    }

    // S'inicialitzen i s'emplenen les dades VTK que han de formar el volum de la reconstrucció.
    vtkImageData *imageDataVTK = vtkImageData::New();
    initAndFillImageDataVTK( pointsPath, imageDataVTK );
    
    // Es crea i assignen les dades d'inicialització al nou volum
    Volume *reslicedVolume = new Volume;
    // TODO Aquí s'haurien d'afegir les imatges que realment s'han creat, no les del volum original
    // Això simplement és un hack perquè pugui funcionar
    reslicedVolume->setImages( m_mainVolume->getImages() );
    
    // S'assignen les dades VTK al nou volum
    reslicedVolume->setData( imageDataVTK );

    return reslicedVolume;
}

QList<double *> CurvedMPRExtension::getPointsPath( QPointer<DrawerPolyline> polyline )
{
    QList< double* > pointsList = polyline->getPointsList();
    QList<double *> pointsPath;
    double *previousPoint;
    double *currentPoint;
    double *newLinePoint;
    double *pointLastPath;
    double *pointLastPolyline;

    m_lastPointsPath.clear();

    // Obtenim quins són els eixos x, y segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );

    // Distància entre els píxels de les imatges que formen el volum
    double spacing[3];
    m_mainVolume->getSpacing( spacing );
    double pixelsDistance = sqrt( spacing[xIndex] * spacing[xIndex] + spacing[yIndex] * spacing[yIndex] );
    
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

            //DEBUG_LOG(QString("numPointsLine = %1").arg(numPointsLine));

            for (int j = 0; j < numPointsLine - 1; j++)
            {
                // Només ens interessa el valor del punt en les coordenades x i y ja que la z serà la que variï segons 
                // la profunditat en que ens trobem al generar la reconstrucció.
                newLinePoint = new double[3];
                newLinePoint[xIndex] = previousPoint[xIndex] + directorVector[xIndex] * pixelsDistance * (j + 1);
                newLinePoint[yIndex] = previousPoint[yIndex] + directorVector[yIndex] * pixelsDistance * (j + 1);
                
                //DEBUG_LOG(QString("New line point [%1,%2,%3]").arg(newLinePoint[0]).arg(newLinePoint[1]).arg(newLinePoint[2]));
                pointsPath.append( newLinePoint );

                pointLastPath = new double[3];
                pointLastPath[xIndex] = newLinePoint[xIndex];
                pointLastPath[yIndex] = newLinePoint[yIndex];
                m_lastPointsPath.append( pointLastPath );
            }
        }

        // S'inclou a la llista el punt actual
        if (i == 0 || previousPoint[xIndex] != currentPoint[xIndex] || previousPoint[yIndex] != currentPoint[yIndex] )
        {
            //DEBUG_LOG(QString("append current point [%1,%2,%3]").arg(currentPoint[0]).arg(currentPoint[1]).arg(currentPoint[2]));
            pointsPath.append( currentPoint );

            pointLastPath = new double[3];
            pointLastPath[xIndex] = currentPoint[xIndex];
            pointLastPath[yIndex] = currentPoint[yIndex];
            m_lastPointsPath.append( pointLastPath );

            pointLastPolyline = new double[3];
            pointLastPolyline[xIndex] = currentPoint[xIndex];
            pointLastPolyline[yIndex] = currentPoint[yIndex];
            m_lastPolylinePoints.append( pointLastPolyline );
        }

        previousPoint = currentPoint;
    }

    return pointsPath;
}

QList<double *> CurvedMPRExtension::getLastPointsPathForFirstImage()
{
    QList<double *> pointsPathForFirstImage;
    double *point;
    double *newLinePoint;
    
    // Obtenim quins són els eixos x, y segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );

    double yDistanceMovement = m_maxThickness / 2;

    for ( int i = 0; i < m_lastPointsPath.size(); i++)
    {
        point = m_lastPointsPath.at(i);

        // Només cal desplaçar el punt per l'eix y segons la vista mostrada al visor
        newLinePoint = new double[3];
        newLinePoint[xIndex] = point[xIndex];
        newLinePoint[yIndex] = point[yIndex] - yDistanceMovement;

        pointsPathForFirstImage.append( newLinePoint );
    }

    return pointsPathForFirstImage;
}

void CurvedMPRExtension::showThichkessPolylines()
{
    // Obtenim quins són els eixos x, y segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );

    // Els punts que formaran les dues polyLines seran el resultat de desplaçar a l'eix de les y
    // els punts de la polyline indicada per l'usuari
    double yDistanceMovement = m_maxThickness / 2;
    double *point;
    double *upPolylinePoint;
    double *downPolylinePoint;
    for ( int i = 0; i < m_lastPolylinePoints.size(); i++)
    {
        point = m_lastPolylinePoints.at(i);

        upPolylinePoint = new double[3];
        upPolylinePoint[xIndex] = point[xIndex];
        upPolylinePoint[yIndex] = point[yIndex] + yDistanceMovement;
        m_upPolylineThickness->addPoint( upPolylinePoint );

        downPolylinePoint = new double[3];
        downPolylinePoint[xIndex] = point[xIndex];
        downPolylinePoint[yIndex] = point[yIndex] - yDistanceMovement;
        m_downPolylineThickness->addPoint( downPolylinePoint );
    }

    m_upPolylineThickness->update();
    m_downPolylineThickness->update();
    mainViewer->render();
}

void CurvedMPRExtension::initAndFillImageDataVTK(const QList<double *> &pointsPath, vtkImageData *imageDataVTK)
{
    Q_ASSERT(m_mainVolume);

    // Obtenim quin és l'eix de profunditat i l'eix y segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );

    // Inicialització les dades VTK que formaran el volum de la reconstrucció.
    double maxX = (double) pointsPath.length();
    double maxY = m_mainVolume->getDimensions()[zIndex];
    
    imageDataVTK->SetOrigin( .0, .0, .0 );
    imageDataVTK->SetSpacing( 1., 1., 1. );
    imageDataVTK->SetDimensions( maxX, maxY, m_numImages );
    imageDataVTK->SetWholeExtent( 0, maxX, 0, maxY, 0, m_numImages );
    imageDataVTK->SetScalarTypeToShort();
    imageDataVTK->SetNumberOfScalarComponents(1);
    imageDataVTK->AllocateScalars();

    // S'emplena el dataset del volum amb el valor dels píxels resultat de projectar en profunditat
    // la línia indicada per l'usuari

    // Cal tenir en compte que si ens han indicat que es vol reconstruir un volum amb més d'una imatge
    // els punts de la línia indicada per l'usuari han estat desplaçats per equivaldre als punts
    // del pla de projecció que representarà la primera imatge.  
    // Així doncs si és el cas, caldrà aplicar el desplaçament següent als punts calculats per generar les
    // successives imatges.
    double yDistanceMovement = 0;
    if ( m_numImages > 1 )
    {
        yDistanceMovement = m_mainVolume->getSpacing()[yIndex];
    }

    double depth = m_mainVolume->getOrigin()[zIndex];
    double spacing = m_mainVolume->getSpacing()[zIndex];
    
    int y=0;
    while ( depth <= maxY )
    {
        // Obtenim el valor del pixel a la llesca actual per tots els punts indicats per 
        // l'usuari i que formen les columnes de la imatge o les imatges resultat
        for ( int x = 0; x < pointsPath.size(); x++ )
        {
            double *point = pointsPath.at(x);
        
            Volume::VoxelType voxelValue;
            
            // Es calcula el valor del voxel allà on es troba el punt actual a la profunditat 
            // actual del volum (recordem que varia segons la vista mostrada al visor principal)
            point[zIndex] = depth;

            // Es calcula el valor del voxel per cada imatge que hagim de reconstruïr i 
            // s'apunta el valor a la posició corresponent del volum de dades
            int idxImage = 0;
            double initialPointY = point[yIndex];
            while ( idxImage < m_numImages )
            {
                // Es calcula el valor del voxel segons la imatge que estiguem reconstruïnt
                // Per tant, si és necessari s'aplicarà el desplaçament calculat
                point[yIndex] = initialPointY + ( yDistanceMovement * idxImage );

                if ( m_mainVolume->getVoxelValue(point, voxelValue) )
                {
                    // Accedim a la posició del volum on es vol modificar el valor del píxel
                    signed short * scalarPointer = (signed short *) imageDataVTK->GetScalarPointer( x, y, idxImage );
                    *scalarPointer = voxelValue;
                }
                idxImage++;
            }
            // Perquè no afecti a les següents línies de les imatges
            point[yIndex] = initialPointY;
        }
        depth += spacing;
        y++;
    }
}

void CurvedMPRExtension::changeSelectedViewer( Q2DViewerWidget *selectedWidget )
{
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    Q2DViewer *reconstructionViewer = m_viewersLayout->getViewerWidget(1)->getViewer();
    
    if ( selectedWidget == m_viewersLayout->getViewerWidget(0) )
    {
        m_toolManager->enableRegisteredActionTools( mainViewer );
        m_toolManager->disableRegisteredActionTools( reconstructionViewer );
    }
    else
    {
        m_toolManager->enableRegisteredActionTools( reconstructionViewer );
        m_toolManager->disableRegisteredActionTools( mainViewer );
    }
}

void CurvedMPRExtension::updateLinePathToolConnection(bool enabled)
{
    if ( enabled )
    {
        Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
        // Quan l'usuari indiqui la línia sobre la que caldrà projectar, llavors s'iniciarà el procés
        // de creació del reslicedVolume que caldrà visualitzar al segon viewer.
        ToolProxy *toolProxy = mainViewer->getToolProxy();
        LinePathTool *linePathTool = qobject_cast<LinePathTool *>( toolProxy->getTool( "LinePathTool" ) );
        connect( linePathTool, SIGNAL( finished( QPointer<DrawerPolyline> ) ), SLOT( updateReslice( QPointer<DrawerPolyline> )) );
    }
}

}; // end namespace udg


