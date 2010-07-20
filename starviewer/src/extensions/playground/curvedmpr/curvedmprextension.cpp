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
    connect( m_viewersLayout, SIGNAL( selectedViewerChanged( Q2DViewerWidget * ) ), SLOT( changeSelectedViewer( Q2DViewerWidget * ) ) );

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
            m_upPolylineThickness->setLineWidth( 3 );
            mainViewer->getDrawer()->draw( m_upPolylineThickness , mainViewer->getView(), mainViewer->getCurrentSlice() );

            // Línia thickness inferior
            m_downPolylineThickness = new DrawerPolyline;
            m_downPolylineThickness->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
            m_downPolylineThickness->setColor( Qt::blue );
            m_downPolylineThickness->setLineWidth( 3 );
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
        if ( !m_lastPolylinePoints.isEmpty() )
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
        // Guardem els punts que ha marcat l'usuari per crear la polyline
        // Per cada punt guardem la direcció en que s'haurà de desplaçar si s'hagués de fer una reconstrucció amb gruix
        storeInfoPointsPolyline( polyline );
    }

    // Si l'usuari vol reconstruir un volum amb més d'una imatge mostrem les polylines que delimitaran el gruix indicat 
    /// i guardem els punts calculats per crear-les
    if ( m_numImages > 1 )
    {
       showThicknessPolylinesAndStorePoints();
    }

    // S'inicialitzen i s'emplenen les dades VTK que han de formar el volum de la reconstrucció.
    vtkImageData *imageDataVTK = initAndFillImageDataVTK();
    
    // Es crea i assignen les dades d'inicialització al nou volum
    Volume *reslicedVolume = new Volume;
    // TODO Aquí s'haurien d'afegir les imatges que realment s'han creat, no les del volum original
    // Això simplement és un hack perquè pugui funcionar
    reslicedVolume->setImages( m_mainVolume->getImages() );
    
    // S'assignen les dades VTK al nou volum
    reslicedVolume->setData( imageDataVTK );

    return reslicedVolume;
}

void CurvedMPRExtension::storeInfoPointsPolyline( QPointer<DrawerPolyline> polyline )
{
    // Netejem informació desada de la línia dibuixada anteriorment per l'usuari
    m_lastPolylinePoints.clear();
    m_directionMovementPolylinePoints.clear();

    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );

    double *previousPoint;
    double *currentPoint;
    double *nextPoint;
    double *directorVector;

    // Guardem els punts que ha marcat l'usuari per crear la polyline
    // Per cada punt guardem la direcció en que s'haurà de desplaçar si s'hagués de fer una reconstrucció amb gruix
    QList< double* > pointsList = polyline->getPointsList();

    for ( int i = 0; i < pointsList.size(); i++)
    {
        currentPoint = pointsList.at(i);
        
        if ( i == 1 )
        {
            // Vector director entre el primer i el segon punt de la polyline
            directorVector = MathTools::directorVector( previousPoint, currentPoint );
            MathTools::normalize( directorVector );

            // El primer punt s'haurà de desplaçar en direcció perpendicular a aquest vector director
            addInfoPointLastPolyline( previousPoint, directorVector, xIndex, yIndex, zIndex );
        }
        
        if ( i > 0 )
        {
            if ( i < pointsList.size() - 1 )
            {
                // El desplaçament dels punts intermitjos de la polyline serà igual al vector director 
                // de la perpendicular de la línia formada pel punt anterior
                // i posterior a aquest.
                nextPoint = pointsList.at(i+1);
                directorVector = MathTools::directorVector( previousPoint, nextPoint );
                MathTools::normalize( directorVector );
            }
            else
            {
                // L'últim punt de la polyline es tornarà a desplaçar com el primer punt
                // en direcció perpendicular al vector director que l'uneix amb l'anterior punt
                directorVector = MathTools::directorVector( previousPoint, currentPoint );
                MathTools::normalize( directorVector );
            }

            // Guardem el punt actual i el vector de desplaçament associat
            addInfoPointLastPolyline( currentPoint, directorVector, xIndex, yIndex, zIndex );
        }

        previousPoint = currentPoint;
    }
    DEBUG_LOG(QString("-------------------"));
}

void CurvedMPRExtension::addInfoPointLastPolyline( double *p, double *dv, int xIndex, int yIndex, int zIndex )
{
    // Guardem els últims punts indicats per l'usuari i la direcció de desplaçament
    // que serà necessària si s'ha de fer una reconstrucció amb més d'una imatge
    
    // Coordenades del punt
    double *point = new double[3];
    point[xIndex] = p[xIndex];
    point[yIndex] = p[yIndex];
    point[zIndex] = p[zIndex];
    m_lastPolylinePoints.append( point );
    
    // Vector de desplaçament del punt perpendicular al vector director indicat
    // V(a,b) --> V2(-b,a)
    double *directorVector = new double[3];
    directorVector[xIndex] = - dv[yIndex];
    directorVector[yIndex] = dv[xIndex];
    directorVector[zIndex] = dv[zIndex];
    m_directionMovementPolylinePoints.append( directorVector );
    
    DEBUG_LOG(QString("InfoPointLastPolyline director vector [%1,%2,%3]").arg(directorVector[xIndex]).arg(directorVector[yIndex]).arg(directorVector[zIndex]));
    DEBUG_LOG(QString("InfoPointLastPolyline point [%1,%2,%3]").arg(point[xIndex]).arg(point[yIndex]).arg(point[zIndex]));
}


void CurvedMPRExtension::showThicknessPolylinesAndStorePoints()
{
    // Netejem informació desada anteriorment per les línies que indiquen gruix
    m_upPolylinePoints.clear();
    m_downPolylinePoints.clear();

    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );

    // Els punts que formaran les dues polylines d'indicació de gruix seran el resultat 
    // de desplaçar en la direcció adequada els punts de la polyline indicada per l'usuari
    double distanceMovement = m_maxThickness / 2;
    
    for ( int i = 0; i < m_lastPolylinePoints.size(); i++)
    {
        // Informació de desplaçament guardada per cada punt de la polyline
        double *point = m_lastPolylinePoints.at(i);
        double *directorVector = m_directionMovementPolylinePoints.at(i);

        double *upPolylinePoint = new double[3];
        upPolylinePoint[xIndex] = point[xIndex] + ( distanceMovement * directorVector[xIndex] );
        upPolylinePoint[yIndex] = point[yIndex] + ( distanceMovement * directorVector[yIndex] );
        upPolylinePoint[zIndex] = point[zIndex] + ( distanceMovement * directorVector[zIndex] );
        m_upPolylineThickness->addPoint( upPolylinePoint );
        m_upPolylinePoints.append( upPolylinePoint );

        double *downPolylinePoint = new double[3];
        downPolylinePoint[xIndex] = point[xIndex] - ( distanceMovement * directorVector[xIndex] );
        downPolylinePoint[yIndex] = point[yIndex] - ( distanceMovement * directorVector[yIndex] );
        downPolylinePoint[zIndex] = point[zIndex] - ( distanceMovement * directorVector[zIndex] );
        m_downPolylineThickness->addPoint( downPolylinePoint );
        m_downPolylinePoints.append( downPolylinePoint );
    }

    m_upPolylineThickness->update();
    m_downPolylineThickness->update();
    mainViewer->render();

    DEBUG_LOG(QString("-------------------"));
    DEBUG_LOG(QString("m_upPolylinePoints"));
    for (int i = 0; i < m_upPolylinePoints.size(); i++ )
    {
        double *point = m_upPolylinePoints.at(i);
        DEBUG_LOG(QString("Punt %1 [%2,%3,%4]").arg(i).arg(point[xIndex]).arg(point[yIndex]).arg(point[zIndex]));
    }
    DEBUG_LOG(QString("-------------------"));

    DEBUG_LOG(QString("-------------------"));
    DEBUG_LOG(QString("m_downPolylinePoints"));
    for (int i = 0; i < m_downPolylinePoints.size(); i++ )
    {
        double *point = m_downPolylinePoints.at(i);
        DEBUG_LOG(QString("Punt %1 [%2,%3,%4]").arg(i).arg(point[xIndex]).arg(point[yIndex]).arg(point[zIndex]));
    }
    DEBUG_LOG(QString("-------------------"));
}

vtkImageData* CurvedMPRExtension::initAndFillImageDataVTK()
{
    Q_ASSERT( m_mainVolume );

    // S'emplena el dataset del volum amb el valor dels píxels resultat de projectar en profunditat
    // la línia indicada per l'usuari o les N línies que caben en el gruix indicat 
    vtkImageData *imageDataVTK = vtkImageData::New();

    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );

    // Obtenim els punts que formen la línia que s'ha de projectar i que
    // indica el número màxim de columnes de les imatges resultants
    QList< double *> pointsPath;
    double distanceMovement = 0;
    QList< double * > firstLinePoints;
    if ( m_numImages == 1 )
    {
        // Obtenim tots els punts que formen la última línia indicada per l'usuari
        pointsPath = getPointsPath( m_lastPolylinePoints );
    }
    else
    {
        DEBUG_LOG(QString("-------------------"));
        DEBUG_LOG(QString("Num imatges a reconstruir = %1").arg(m_numImages));

        // Obtenim tots els punts que formen la línia que marca límit superior del gruix
        QList<double *> upPolylinePointsPath = getPointsPath( m_upPolylinePoints );

        DEBUG_LOG(QString("UpPolylinePointsPath = %1").arg(upPolylinePointsPath.size()));

        // Obtenim tots els punts que formen la línia que marca límit inferior del gruix
        QList<double *> downPolylinePointsPath = getPointsPath( m_downPolylinePoints );

        DEBUG_LOG(QString("DownPolylinePointsPath = %1").arg(downPolylinePointsPath.size()));

        // La línia que ha d'indicar el número màxim de columnes de les imatges resultat
        // és la que té el perímetre més gran
        pointsPath = upPolylinePointsPath;
        if ( downPolylinePointsPath.size() > upPolylinePointsPath.size() ) 
        {
            pointsPath = downPolylinePointsPath;
        }

        // Cal reconstruir un volum amb més d'una imatge per tant per obtenir els punts
        // que representarien les polylines de cada tall caldrà aplicar el següent desplaçament
        double spacing[3];
        m_mainVolume->getSpacing( spacing );
        distanceMovement = sqrt( spacing[xIndex] * spacing[xIndex] + spacing[yIndex] * spacing[yIndex] );

        // Els punts de referència seran els de la línia inferior, i aquests caldrà desplaçar-los
        // en la direcció corresponent aquesta distància
        firstLinePoints = m_downPolylinePoints;

        DEBUG_LOG(QString("-------------------"));
        DEBUG_LOG(QString("firstLinePoints"));
        for (int i = 0; i < firstLinePoints.size(); i++ )
        {
            double *point = firstLinePoints.at(i);
            DEBUG_LOG(QString("Punt %1 [%2,%3,%4]").arg(i).arg(point[xIndex]).arg(point[yIndex]).arg(point[zIndex]));
        }
        DEBUG_LOG(QString("-------------------"));
    }

    // Inicialització les dades VTK que formaran el volum de la reconstrucció.
    double maxX = (double) pointsPath.length();
    double maxY = m_mainVolume->getDimensions()[zIndex];

    DEBUG_LOG(QString("maxX = %1").arg(maxX));
    DEBUG_LOG(QString("maxY = %1").arg(maxY));
    
    imageDataVTK->SetOrigin( .0, .0, .0 );
    imageDataVTK->SetSpacing( 1., 1., 1. );
    imageDataVTK->SetDimensions( maxX, maxY, m_numImages );
    imageDataVTK->SetWholeExtent( 0, maxX, 0, maxY, 0, m_numImages );
    imageDataVTK->SetScalarTypeToShort();
    imageDataVTK->SetNumberOfScalarComponents(1);
    imageDataVTK->AllocateScalars();
    
    // Per saber espai que ens hem de moure en profunditat en cada iteració
    double spacing = m_mainVolume->getSpacing()[zIndex];

    // Procediment:
    // Per cada imatge a obtenir s'obté els punts que formen la línia de projecció
    // i obtenim el valor de cada punt a cada profunditat, creant així les N imatges
    // que han de composar el volum reconstruït
    int idxImage = 0;
    while ( idxImage < m_numImages )
    {
        if (idxImage > 0 )
        {
            // Obtenim els punts que composen la línia a projectar per la imatge actual
            QList< double * > linePoints;
            for ( int p = 0; p < firstLinePoints.size(); p++ )
            {
                double *referencePoint = firstLinePoints.at(p);
                double *directionVector = m_directionMovementPolylinePoints.at(p);
                
                double *linePoint = new double[3];
                linePoint[xIndex] = referencePoint[xIndex] + ( distanceMovement * directionVector[xIndex] * idxImage );
                linePoint[yIndex] = referencePoint[yIndex] + ( distanceMovement * directionVector[yIndex] * idxImage );
                linePoint[zIndex] = referencePoint[zIndex] + ( distanceMovement * directionVector[zIndex] * idxImage );

                linePoints.append( linePoint );
            }

            DEBUG_LOG(QString("-------------------"));
            DEBUG_LOG(QString("linePoints"));
            for (int i = 0; i < linePoints.size(); i++ )
            {
                double *point = linePoints.at(i);
                DEBUG_LOG(QString("Punt %1 [%2,%3,%4]").arg(i).arg(point[xIndex]).arg(point[yIndex]).arg(point[zIndex]));
            }
            DEBUG_LOG(QString("-------------------"));
            
            // Obtenim tots els punts que formen la línia delimitada per aquests punts
            pointsPath = getPointsPath( linePoints );
        }

        for ( int x = 0; x < pointsPath.size(); x++ )
        {
            double *point = pointsPath.at(x);

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
        }
        idxImage++;
    }

    return imageDataVTK;
}

QList<double *> CurvedMPRExtension::getPointsPath( QList<double *> linePoints )
{
    QList<double *> pointsPath;

    // Obtenim quines són les coordenades (x,y,z) segons la vista mostrada al visor principal
    Q2DViewer *mainViewer = m_viewersLayout->getViewerWidget(0)->getViewer();
    int xIndex = mainViewer->getXIndexForView( mainViewer->getView() );
    int yIndex = mainViewer->getYIndexForView( mainViewer->getView() );
    int zIndex = mainViewer->getZIndexForView( mainViewer->getView() );

    // Distància entre els píxels de les imatges que formen el volum
    double spacing[3];
    m_mainVolume->getSpacing( spacing );
    double pixelsDistance = sqrt( spacing[xIndex] * spacing[xIndex] + spacing[yIndex] * spacing[yIndex] );
    
    double *previousPoint;
    double *currentPoint;
    double *directorVector;
    double *newLinePoint;

    // Obtenim tots els punts que formen la línia delimitada pels punts passats per paràmetre
    for ( int i = 0; i < linePoints.size(); i++)
    {
        currentPoint = linePoints.at(i);
        
        if ( i > 0 )
        {
            // S'anoten tots els punts que hi ha sobre la línia que formen el punt anterior 
            // i l'actual de la línia indicada per paràmetre
            // La distància entre aquests punts per defecte serà la distància entre píxels
            directorVector = MathTools::directorVector( previousPoint, currentPoint );
            MathTools::normalize( directorVector );
            double distanceLine = MathTools::getDistance3D( previousPoint, currentPoint );
            double numPointsLine = distanceLine / pixelsDistance;

            //DEBUG_LOG(QString("numPointsLine = %1").arg(numPointsLine));
            for (int j = 0; j < numPointsLine - 1; j++)
            {
                newLinePoint = new double[3];
                newLinePoint[xIndex] = previousPoint[xIndex] + directorVector[xIndex] * pixelsDistance * (j + 1);
                newLinePoint[yIndex] = previousPoint[yIndex] + directorVector[yIndex] * pixelsDistance * (j + 1);
                newLinePoint[zIndex] = previousPoint[zIndex] + directorVector[zIndex] * pixelsDistance * (j + 1);
                
                //DEBUG_LOG(QString("New line point [%1,%2,%3]").arg(newLinePoint[0]).arg(newLinePoint[1]).arg(newLinePoint[2]));
                pointsPath.append( newLinePoint );
            }
        }

        // S'inclou a la llista el punt actual
        if ( i == 0 
             || previousPoint[xIndex] != currentPoint[xIndex] 
             || previousPoint[yIndex] != currentPoint[yIndex]
             || previousPoint[zIndex] != currentPoint[zIndex] )
        {
            //DEBUG_LOG(QString("append current point [%1,%2,%3]").arg(currentPoint[0]).arg(currentPoint[1]).arg(currentPoint[2]));
            pointsPath.append( currentPoint );
        }
        
        previousPoint = currentPoint;
    }

    return pointsPath;
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


