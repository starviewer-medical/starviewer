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
    
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    Q2DViewer *reconstructionViewer = m_viewersLayout->getViewerWidget(1)->getViewer();

    // configurem les annotacions que volem veure
    mainViewer->enableAnnotation( 
        Q2DViewer::WindowInformationAnnotation | 
        Q2DViewer::PatientOrientationAnnotation |
        Q2DViewer::RulersAnnotation | 
        Q2DViewer::SliceAnnotation |
        Q2DViewer::AcquisitionInformationAnnotation, 
        true);

    reconstructionViewer->enableAnnotation( 
        Q2DViewer::WindowInformationAnnotation | 
        Q2DViewer::RulersAnnotation | 
        Q2DViewer::SliceAnnotation,
        true);

    // Al segon viewer només li deixarem posar l'input del reslice curvilini. 
    // L'usuari no el podrà canviar amb un volum "regular"
    reconstructionViewer->disableContextMenu();

    // Inicialitzem les tools disponibles per cada visor
    initializeTools();

    // Inicialització dades
    m_numImages = 1;
    m_maxThickness = 0;

    // Cada cop que es canvia l'input del viewer principal cal actualitzar el volum de treball
    connect( m_viewersLayout->getViewerWidget(0)->getViewer(), SIGNAL( volumeChanged( Volume * ) ), SLOT( updateMainVolume( Volume * ) ) );

    // Cada cop que es canvia el viewer seleccionat s'habiliten les tools en aquest visor
    // i es deshabiliten de l'altre
    connect( m_viewersLayout, SIGNAL( viewerSelectedChanged( Q2DViewerWidget * ) ), SLOT( changeSelectedViewer( Q2DViewerWidget * ) ) );

    // Cada cop que l'usuari modifiqui el número d'imatges a generar al volum reconstruït, es torna a fer
    // la reconstrucció al visor corresponent, tenint en compte la última línia indicada per l'usuari
    // Fem que si avancem d'un en un el valor d'slab (amb teclat o amb la roda del ratolí)
    // s'actualitzi amb el signal valueChanged()
    // si el valor es canvia arrossegant l'slider, canviem el comportament i no apliquem el nou
    // valor de thickness fins que no fem el release
    // Ho fem així, ja que al arrossegar es van enviant senyals de valueChanged i això feia que
    // la resposta de l'interfície fos una mica lenta, ja que calcular el nou slab és costós
    // TODO si el procés anés amb threads no tindríem aquest problema
    turnOffDelayedUpdate();
    connect( m_numberOfImagesSlider, SIGNAL( sliderPressed () ), SLOT( turnOnDelayedUpdate() ) );
    connect( m_numberOfImagesSlider, SIGNAL( valueChanged( int ) ), SLOT( updateNumberOfImagesLabel( int ) ) );
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
    m_toolManager->enableRegisteredActionTools( mainViewer );

    m_toolManager->setupRegisteredTools( reconstructionViewer );
    reconstructionViewer->removeAction( m_toolManager->registerTool("LinePathTool") );

    // Associem el widget thickSlab amb el visor que té la reconstrucció
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
    
    // Slider per indicar el gruix de la reconstrucció només ha d'acceptar números entre 1 i 
    // el màxim número de files o columnes del volum original
    int *volumeExtent = m_mainVolume->getWholeExtent();

    int rows = 0;
    int columns = 0;
    
    // Depenent de la vista mostrada 
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    QViewer::CameraOrientationType view = mainViewer->getView();
    if ( view == QViewer::Axial )
    {
        rows = volumeExtent[1];
        columns = volumeExtent[3];
    }
    else if (view == QViewer::Sagital )
    {
        rows = volumeExtent[3];
        columns = volumeExtent[5];
    }
    else if ( view == QViewer::Coronal )
    {
        rows = volumeExtent[1];
        columns = volumeExtent[5];
    }

    double max = MathTools::maximum( rows, columns );

    m_numberOfImagesSlider->setRange( 1, max );
}

void CurvedMPRExtension::changeThicknessReconstruction()
{
    int numImages = m_numberOfImagesSlider->value();

    if (numImages != m_numImages )
    {
        m_numImages = numImages;

        Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();

        if ( m_numImages && m_numImages > 1 )
        {
            // Obtenim quin és l'eix x i y (de desplaçament) segons la vista mostrada al visor principal
            int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
            int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );

            // Distància entre els píxels de les imatges que formen el volum
            double spacing[3];
            m_mainVolume->getSpacing( spacing );

            // Calculem la distància màxima entre la primera i la úlltima imatge que composaran la reconstrucció
            // agafant com a referència la distància entre els píxels de les imatges del volum
            double pixelsDistance = sqrt( spacing[xIndex] * spacing[xIndex] + spacing[yIndex] * spacing[yIndex] );
            m_maxThickness = m_numImages * pixelsDistance;
        }
        else
        {
            m_numImages = 1;
            m_maxThickness = 0;
        }

        // Inicialitzem o netegem les línies que mostraran al viewer principal quins són els
        // límits de les imatges que es reconstruiran pel nou volum, si es que cal mostrar-les
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

        // Per fer la reconstrucció cal que l'usuari anteriorment hagi indicat una línia
        if ( !m_lastPointsPath.isEmpty() )
        {
            updateResliceWithLastPointsPath();
        }
    }
}

void CurvedMPRExtension::updateReslice( QPointer<DrawerPolyline> polyline )
{
    if ( m_upPolylineThickness )
    {
        m_upPolylineThickness->deleteAllPoints();
        m_upPolylineThickness->update();

        m_downPolylineThickness->deleteAllPoints();
        m_downPolylineThickness->update();

        Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
        mainViewer->render();
    }

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
       showThicknessPolylines();
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
    double *nextPoint;
    double *directorVector;
    double *newLinePoint;

    // Netejem informació desada de la línia dibuixada anteriorment per l'usuari
    // per guardar la dels punts actuals
    m_lastPolylinePoints.clear();
    m_lastPointsPath.clear();
    m_pointsSegmentMovement.clear();

    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );

    // Distància entre els píxels de les imatges que formen el volum
    double spacing[3];
    m_mainVolume->getSpacing( spacing );
    double pixelsDistance = sqrt( spacing[xIndex] * spacing[xIndex] + spacing[yIndex] * spacing[yIndex] );
    
    //DEBUG_LOG(QString("PolyLine points size = %1").arg(pointsList.length()));

    int numPointsPath = -1;

    for ( int i = 0; i < pointsList.size(); i++)
    {
        currentPoint = pointsList.at(i);
        
        if ( i > 0 )
        {
            // S'anoten tots els punts que hi ha sobre la línia que formen el punt anterior 
            // i l'actual de la polyline indicada per l'usuari
            // La distància entre aquests punts per defecte serà la distància entre píxels
            directorVector = MathTools::directorVector( previousPoint, currentPoint );
            MathTools::normalize( directorVector );
            double distanceLine = MathTools::getDistance3D( previousPoint, currentPoint );
            double numPointsLine = distanceLine / pixelsDistance;

            //DEBUG_LOG(QString("numPointsLine = %1").arg(numPointsLine));

            int j;
            for (j = 0; j < numPointsLine - 1; j++)
            {
                newLinePoint = new double[3];
                newLinePoint[xIndex] = previousPoint[xIndex] + directorVector[xIndex] * pixelsDistance * (j + 1);
                newLinePoint[yIndex] = previousPoint[yIndex] + directorVector[yIndex] * pixelsDistance * (j + 1);
                newLinePoint[zIndex] = previousPoint[zIndex] + directorVector[zIndex] * pixelsDistance * (j + 1);
                
                //DEBUG_LOG(QString("New line point [%1,%2,%3]").arg(newLinePoint[0]).arg(newLinePoint[1]).arg(newLinePoint[2]));
                pointsPath.append( newLinePoint );

                // Guardem punts calculats per la última línia dibuixada per l'usuari així
                // si modifica el número d'imatges a reconstruir no cal tornar-los a calcular
                addPointLastPath( newLinePoint, xIndex, yIndex, zIndex );

                numPointsPath++;
            }
        }

        if ( i == 1 )
        {
            // Guardem el primer punt indict per l'usuari i la direcció de desplaçament
            // ja que serà necessari per dibuixar línies de referències del gruix
            addInfoPointLastPolyline( previousPoint, directorVector, xIndex, yIndex, zIndex );
        }
        
        // Guardem informació de desplaçament dels punts que toqui
        if ( i > 0 )
        {
            // Guardem per cada segment quin és l'index de l'últim punt a desplaçar
            // i en quina direcció s'hauran de desplaçar els punts anteriors.
            int idxLastPointToMove = numPointsPath;
            if ( (i + 1) == pointsList.size() )
            {
                // Si es tracta de l'últim punt de la llista llavors també li apliquem aquest desplaçament
                idxLastPointToMove++;
            }
            addInfoPointsSegmentMovement( directorVector, idxLastPointToMove, xIndex, yIndex, zIndex );
        }


        // S'inclou a la llista el punt actual
        if ( i == 0 
             || previousPoint[xIndex] != currentPoint[xIndex] 
             || previousPoint[yIndex] != currentPoint[yIndex]
             || previousPoint[zIndex] != currentPoint[zIndex] )
        {
            //DEBUG_LOG(QString("append current point [%1,%2,%3]").arg(currentPoint[0]).arg(currentPoint[1]).arg(currentPoint[2]));
            pointsPath.append( currentPoint );

            // Guardem punts calculats per la última línia dibuixada per l'usuari així
            // si modifica el número d'imatges a reconstruir no cal tornar-los a calcular
            addPointLastPath( currentPoint, xIndex, yIndex, zIndex );

            numPointsPath++;
        }
        
        if ( i > 0 )
        {
            if ( i < pointsList.size() - 1 )
            {
                // El desplaçament dels punts finals dels segments intermitjos de la línia
                // serà igual al vector director de la perpendicular de la línia formada pel punt anterior
                // i posterior a aquest.
                nextPoint = pointsList.at(i+1);
                directorVector = MathTools::directorVector( previousPoint, nextPoint );
                MathTools::normalize( directorVector );
                
                addInfoPointsSegmentMovement( directorVector, numPointsPath, xIndex, yIndex, zIndex );
            }

            // Guardem els últims punts indicats per l'usuari i la direcció de desplaçament
            // ja que serà necessari per dibuixar línies de referències del gruix
            addInfoPointLastPolyline( currentPoint, directorVector, xIndex, yIndex, zIndex );
        }

        previousPoint = currentPoint;
    }

    return pointsPath;
}

void CurvedMPRExtension::addPointLastPath( double *point, int xIndex, int yIndex, int zIndex )
{
    // Guardem punts calculats per la última línia dibuixada per l'usuari així
    // si modifica el número d'imatges a reconstruir no cal tornar-los a calcular
    double *pointLastPath = new double[3];

    pointLastPath[xIndex] = point[xIndex];
    pointLastPath[yIndex] = point[yIndex];
    pointLastPath[zIndex] = point[zIndex];
    
    m_lastPointsPath.append( pointLastPath );
}

void CurvedMPRExtension::addInfoPointLastPolyline( double *point, double *directorVector, int xIndex, int yIndex, int zIndex )
{
    // Guardem els últims punts indicats per l'usuari i la direcció de desplaçament
    // ja que serà necessari per dibuixar línies de referències del gruix
    InfoPointLastPolyline infoPointLastPolyline;
    
    // Coordenades del punt
    infoPointLastPolyline.point = new double[3];
    infoPointLastPolyline.point[xIndex] = point[xIndex];
    infoPointLastPolyline.point[yIndex] = point[yIndex];
    infoPointLastPolyline.point[zIndex] = point[zIndex];
    
    // Vector de desplaçament del punt
    infoPointLastPolyline.directorVector = new double[3];
    infoPointLastPolyline.directorVector[xIndex] = - directorVector[yIndex];
    infoPointLastPolyline.directorVector[yIndex] = directorVector[xIndex];
    infoPointLastPolyline.directorVector[zIndex] = directorVector[zIndex];
    
    m_lastPolylinePoints.append( infoPointLastPolyline );

    //DEBUG_LOG(QString("InfoPointLastPolyline director vector [%1,%2,%3]").arg(infoPointLastPolyline.directorVector[xIndex]).arg(infoPointLastPolyline.directorVector[yIndex]).arg(infoPointLastPolyline.directorVector[zIndex]));
    //DEBUG_LOG(QString("InfoPointLastPolyline point [%1,%2,%3]").arg(infoPointLastPolyline.point[xIndex]).arg(infoPointLastPolyline.point[yIndex]).arg(infoPointLastPolyline.point[zIndex]));
}

void CurvedMPRExtension::addInfoPointsSegmentMovement( double *directorVector, int idxLastPointToMove, int xIndex, int yIndex, int zIndex )
{
    // Guardem per cada segment quin és l'index de l'últim punt a desplaçar
    // i en quina direcció s'hauran de desplaçar els punts anteriors
    // En aquest cas aquesta direcció serà el vector director de la recta perpendicular al segment
    // creat entre el punt anterior i l'actual
    // vectorDirector(A,B) <-> vectorDirectorPerpendicular(-B,A)
    InfoPointsSegmentMovement infoPointsSegmentMovement;

    // Vector director recta perpendicular
    infoPointsSegmentMovement.directorVector = new double[3];
    infoPointsSegmentMovement.directorVector[xIndex] = - directorVector[yIndex];
    infoPointsSegmentMovement.directorVector[yIndex] = directorVector[xIndex];
    infoPointsSegmentMovement.directorVector[zIndex] = directorVector[zIndex];
    
    // índex últim punt afegit que haurà de desplaçar-se en aquesta direcció si cal reconstruir més d'una imatge
    infoPointsSegmentMovement.idxLastPointToMove = idxLastPointToMove; 
    
    m_pointsSegmentMovement.append( infoPointsSegmentMovement );

    //DEBUG_LOG(QString("infoPointsSegmentMovement [%1,%2,%3, %4]").arg(infoPointsSegmentMovement.directorVector[0]).arg(infoPointsSegmentMovement.directorVector[1]).arg(infoPointsSegmentMovement.directorVector[2]).arg(infoPointsSegmentMovement.idxLastPointToMove));
}

QList<double *> CurvedMPRExtension::getLastPointsPathForFirstImage()
{
    QList<double *> pointsPathForFirstImage;
    
    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getYIndexForView( mainViewer->getView() );

    // Distància que caldrà desplaçar cada punt
    double distanceMovement = m_maxThickness / 2;

    // Informació de desplaçament guardada per cada tram
    int numSegmentsProcessed = 0;
    InfoPointsSegmentMovement infoPointsSegmentMovement = m_pointsSegmentMovement.at(numSegmentsProcessed);
    int idxLastPointSegment = infoPointsSegmentMovement.idxLastPointToMove;
    double *vectorDirector = infoPointsSegmentMovement.directorVector;
    numSegmentsProcessed++;

    for ( int i = 0; i < m_lastPointsPath.size(); i++)
    {
        if ( i == idxLastPointSegment + 1 )
        {
            infoPointsSegmentMovement = m_pointsSegmentMovement.at(numSegmentsProcessed);
            idxLastPointSegment = infoPointsSegmentMovement.idxLastPointToMove;
            vectorDirector = infoPointsSegmentMovement.directorVector;
            numSegmentsProcessed++;
        }

        double *lastPoint = m_lastPointsPath.at(i);

        // Desplacem els punts segons la direcció del vector director que li toqui al punt actual
        double *point = new double[3];
        point[xIndex] = lastPoint[xIndex] - ( distanceMovement * vectorDirector[xIndex] );
        point[yIndex] = lastPoint[yIndex] - ( distanceMovement * vectorDirector[yIndex] );
        point[zIndex] = lastPoint[zIndex] - ( distanceMovement * vectorDirector[zIndex] );

        pointsPathForFirstImage.append( point );
    }

    return pointsPathForFirstImage;
}

void CurvedMPRExtension::showThicknessPolylines()
{
    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );

    // Els punts que formaran les dues polyLines seran el resultat de desplaçar en la direcció
    // adequada els punts de la polyline indicada per l'usuari
    double distanceMovement = m_maxThickness / 2;
    
    for ( int i = 0; i < m_lastPolylinePoints.size(); i++)
    {
        // Informació de desplaçament guardada per cada punt de la polyline
        InfoPointLastPolyline infoPointLastPolyline = m_lastPolylinePoints.at(i);
        double *point = infoPointLastPolyline.point;
        double *directorVector = infoPointLastPolyline.directorVector;

        double *upPolylinePoint = new double[3];
        upPolylinePoint[xIndex] = point[xIndex] + ( distanceMovement * directorVector[xIndex] );
        upPolylinePoint[yIndex] = point[yIndex] + ( distanceMovement * directorVector[yIndex] );
        upPolylinePoint[zIndex] = point[zIndex] + ( distanceMovement * directorVector[zIndex] );
        m_upPolylineThickness->addPoint( upPolylinePoint );

        double *downPolylinePoint = new double[3];
        downPolylinePoint[xIndex] = point[xIndex] - ( distanceMovement * directorVector[xIndex] );
        downPolylinePoint[yIndex] = point[yIndex] - ( distanceMovement * directorVector[yIndex] );
        downPolylinePoint[zIndex] = point[zIndex] - ( distanceMovement * directorVector[zIndex] );
        m_downPolylineThickness->addPoint( downPolylinePoint );
    }

    m_upPolylineThickness->update();
    m_downPolylineThickness->update();
    mainViewer->render();
}

void CurvedMPRExtension::initAndFillImageDataVTK(const QList<double *> &pointsPath, vtkImageData *imageDataVTK)
{
    Q_ASSERT(m_mainVolume);

    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );

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
    // successives imatges i en la direcció corresponent en cada cas
    double distanceMovement = 0;
    if ( m_numImages > 1 )
    {
        // Distància entre els píxels de les imatges que formen el volum
        double spacing[3];
        m_mainVolume->getSpacing( spacing );

        distanceMovement = sqrt( spacing[xIndex] * spacing[xIndex] + spacing[yIndex] * spacing[yIndex] );
    }

    // Per saber espai que ens hem de moure en profunditat en cada iteració
    double spacing = m_mainVolume->getSpacing()[zIndex];

    // Informació de desplaçament guardada per cada tram
    int numSegmentsProcessed = 0;
    InfoPointsSegmentMovement infoPointsSegmentMovement = m_pointsSegmentMovement.at(numSegmentsProcessed);
    int idxLastPointSegment = infoPointsSegmentMovement.idxLastPointToMove;
    double *vectorDirector = infoPointsSegmentMovement.directorVector;
    numSegmentsProcessed++;

    // Obtenim el valor dels pixels per tots els punts indicats en cada profunditat 
    // i ho fem a la vegada per les imatges que calgui reconstruir
    for ( int x = 0; x < pointsPath.size(); x++ )
    {
        double *point = pointsPath.at(x);

        // Direcció de desplaçament que li correspon al punt actual
        if ( x == idxLastPointSegment + 1 )
        {
            infoPointsSegmentMovement = m_pointsSegmentMovement.at(numSegmentsProcessed);
            idxLastPointSegment = infoPointsSegmentMovement.idxLastPointToMove;
            vectorDirector = infoPointsSegmentMovement.directorVector;
            numSegmentsProcessed++;
        }

        double xDesplacement = distanceMovement * vectorDirector[xIndex];
        double yDesplacement = distanceMovement * vectorDirector[yIndex];
        
        // Es calcula el valor del voxel per cada imatge que hagim de reconstruïr i 
        // s'apunta el valor a la posició corresponent del volum de dades
        int idxImage = 0;
        double initialPointX = point[xIndex];
        double initialPointY = point[yIndex];
        while ( idxImage < m_numImages )
        {
            // Es calcula el valor del voxel segons la imatge que estiguem reconstruïnt
            // Per tant, si és necessari s'aplicarà el desplaçament calculat
            point[xIndex] = initialPointX + ( xDesplacement * idxImage );
            point[yIndex] = initialPointY + ( yDesplacement * idxImage );
            
            int y=0;
            double depth = m_mainVolume->getOrigin()[zIndex];
            
            while ( depth <= maxY )
            {
                // Es calcula el valor del voxel allà on es troba el punt actual a la profunditat 
                // actual del volum (recordem que varia segons la vista mostrada al visor principal)
                // i per cada imatge
                point[zIndex] = depth;

                Volume::VoxelType voxelValue;
                if ( m_mainVolume->getVoxelValue(point, voxelValue) )
                {
                    // Accedim a la posició del volum on es vol modificar el valor del píxel
                    signed short * scalarPointer = (signed short *) imageDataVTK->GetScalarPointer( x, y, idxImage );
                    *scalarPointer = voxelValue;
                }

                depth += spacing;
                y++;
            }
            idxImage++;
        }
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

void CurvedMPRExtension::turnOnDelayedUpdate()
{
    disconnect( m_numberOfImagesSlider, SIGNAL( valueChanged(int) ), this, SLOT( changeThicknessReconstruction() ) );
    connect( m_numberOfImagesSlider, SIGNAL( sliderReleased () ), SLOT( onSliderReleased() ) );
}

void CurvedMPRExtension::turnOffDelayedUpdate()
{
    disconnect( m_numberOfImagesSlider, SIGNAL( sliderReleased () ), this, SLOT( onSliderReleased() ) );
    connect( m_numberOfImagesSlider, SIGNAL( valueChanged(int) ), SLOT( changeThicknessReconstruction() ) );
}

void CurvedMPRExtension::onSliderReleased()
{
    changeThicknessReconstruction();
    turnOffDelayedUpdate();
}

void CurvedMPRExtension::updateNumberOfImagesLabel(int value)
{
    m_numberOfImagesLabel->setText( QString::number( value ) );
}

}; // end namespace udg


