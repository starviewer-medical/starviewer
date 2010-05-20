#include "imageplaneprojectiontool.h"
#include "imageplaneprojectiontooldata.h"
#include "q2dviewer.h"
#include "imageplane.h"
#include "drawerline.h"
#include "volume.h"
#include "logging.h"
#include "series.h"
#include "image.h"
#include "drawer.h"
#include "mathtools.h" // per càlculs d'interseccions
#include "tooldata.h"
#include "toolconfiguration.h"
// Vtk's
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>
#include <vtkImageReslice.h>
// Qt's
#include <QStringList>
#include <QColor>

namespace udg {

ImagePlaneProjectionTool::ImagePlaneProjectionTool( QViewer *viewer, QObject *parent )
:Tool(viewer,parent), m_reslice(0)
{
    m_toolName = "ImagePlaneProjectionTool";
    m_hasSharedData = true;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    m_myData = new ImagePlaneProjectionToolData;
    m_toolData = m_myData;

    m_state = NONE;

    m_volume = NULL;
}

ImagePlaneProjectionTool::~ImagePlaneProjectionTool()
{
    if ( m_reslice )
    {
        m_reslice->Delete();
    }
}

void ImagePlaneProjectionTool::setConfiguration( ToolConfiguration *configuration )
{
    checkValuesConfiguration( configuration );
    m_toolConfiguration = configuration;
    applyConfiguration();
}

void ImagePlaneProjectionTool::checkValuesConfiguration( ToolConfiguration *configuration )
{
    if ( !( configuration->containsValue( "typeConfiguration" ) ) )
    {
        DEBUG_LOG(QString("ImagePlaneProjectionTool: No s'ha indicat el tipus de configuració a aplicar al viewer.") );
        return;
    }

    //Comprovem que s'hagin assignat els paràmetres corresponents al tipus de configuració assignada
    QString typeConfiguration = configuration->getValue( "typeConfiguration" ).toString();
    if ( typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
    {
        checkConfigurationProducer( configuration );
        checkConfigurationConsumer( configuration );
    }
    else if (  typeConfiguration == QString( "PRODUCER" ) )
    {
        checkConfigurationProducer( configuration );
    }
    else if ( typeConfiguration == QString( "CONSUMER" ) )
    {
        checkConfigurationConsumer( configuration );
    }
    else
    {
        DEBUG_LOG(QString("ImagePlaneProjectionTool: Tipus de configuració indicada incorrecte. Els valors possibles són: PRODUCER o CONSUMER.") );
        return;
    }
}

void ImagePlaneProjectionTool::checkConfigurationProducer( ToolConfiguration *configuration )
{
    int numProjectedLines = configuration->getValue( "numProjectedLines" ).toInt();
    QStringList namesProjectedLines = configuration->getValue( "namesProjectedLines" ).toStringList();
    QStringList initOrientationsProjectedLines = configuration->getValue( "initOrientationsProjectedLines" ).toStringList();
    QList< QVariant > colorsProjectedLines = configuration->getValue( "colorsProjectedLines" ).toList();

    if ( numProjectedLines != namesProjectedLines.length() 
          || numProjectedLines != initOrientationsProjectedLines.length() 
          || numProjectedLines != colorsProjectedLines.length() )
    {
        DEBUG_LOG( QString("ImagePlaneProjectionTool: No s'ha indicat l'identificació, la orientació i el color de totes les línies a projectar.") );
        return;
    }

    foreach ( QString orientation, initOrientationsProjectedLines ) 
    {
        if ( orientation != QString("HORIZONTAL") && orientation != QString("VERTICAL") )
        {
            DEBUG_LOG( QString("ImagePlaneProjectionTool: No s'ha indicat un identificador d'orientació correcte.") );
            return;
        }
    }

    for ( int i = 0; i < colorsProjectedLines.length(); i++ ) 
    {
        if( !colorsProjectedLines.at( i ).canConvert<QColor>() )
            DEBUG_LOG( QString("ImagePlaneProjectionTool: No s'ha indicat un color correcte.") );
    }
}

void ImagePlaneProjectionTool::checkConfigurationConsumer( ToolConfiguration *configuration )
{
    if ( !( configuration->containsValue( "nameProjectedLine" ) ) )
    {
        DEBUG_LOG( QString("ImagePlaneProjectionTool: No s'ha indicat el nom de la línia de projecció associada a la tool amb configuració consumidor.") );
        return;
    }
}

void ImagePlaneProjectionTool::applyConfiguration()
{
    if ( !m_toolConfiguration )
    {
        DEBUG_LOG(QString("ImagePlaneProjectionTool: És obligatori associar a la tool una configuració correcta.") );
        return;
    }

    QString typeConfiguration = m_toolConfiguration->getValue( "typeConfiguration" ).toString();
    if ( typeConfiguration == QString( "PRODUCER" ) )
    {
        // Cada cop que el viewer canvïi d'input s'ha d'inicialitzar el pla projectat per cada línia de projecció de la tool 
        // i el volum compartit amb la resta de visors amb la tool activa
        connect( m_2DViewer, SIGNAL( volumeChanged( Volume * ) ), SLOT( initializeImagePlanesUpdteVolume( Volume * ) ) );

        // Cada cop que el viewer canvïi l'imatge o l'slab s'ha d'actualitzar el pla projectat per cada línia de projecció de la tool
        connect( m_2DViewer, SIGNAL( sliceChanged( int ) ), SLOT( updateProjections()) );
        connect( m_2DViewer, SIGNAL( slabThicknessChanged( int ) ), SLOT( updateProjections()) );
    }

    if ( typeConfiguration == QString( "PRODUCER" ) || typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
    {
        // Quan es modifica el pla projectar per una línia d'un viewer amb la tool configurada com a productor
        // cal comprovar si algun altre visor productor té alguna línia amb la mateixa orientació que cal actualitzar
        // ( X visors diferents poden haver de mostrar línies amb el mateix pla de projecció )
        connect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), SLOT( checkImagePlaneBindUpdated( QString ) ) );

        initProjectedLines();
    }

    if ( typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
    {
        // Quan es modifica el volum de treball a les dades compartides els visors amb la tool configurada com a 
        // productor i consumidor inicialitzen el pla projectat per cada línia de projecció de la tool comprovant
        // primer si ja existeix a les dades un pla per la orientació corresponent.
        connect( m_myData, SIGNAL( dataInitialized() ), SLOT( initializeImagePlanesCheckingData() ) );
    }
    
    if ( typeConfiguration == QString( "CONSUMER" ) || typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
    {
        // Quan es modifica el pla projectat per una línia d'un viewer amb la tool configurada com a productor
        // cal comprovar si es tracta de la línia associada a l'actual viewer i si és així mostrar el nou pla
        // (aquesta línia de projecció està mostrada a un viewer amb la tool configurada com a productor)
        connect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), SLOT( checkProjectedLineBindUpdated( QString ) ) );

        m_nameProjectedLineBind = m_toolConfiguration->getValue( "nameProjectedLine" ).toString();
        
        // Inicialitzem el reslice del viewer
        m_reslice = vtkImageReslice::New();
        m_reslice->AutoCropOutputOn();
        m_reslice->SetInterpolationModeToCubic();
    }
}

void ImagePlaneProjectionTool::initProjectedLines()
{
    QStringList names = m_toolConfiguration->getValue( "namesProjectedLines" ).toStringList();
    QStringList orientations = m_toolConfiguration->getValue( "initOrientationsProjectedLines" ).toStringList();
    QList< QVariant> colorsProjectedLines = m_toolConfiguration->getValue( "colorsProjectedLines" ).toList();

    //TODO
    // S'han de netejar les lines mostrades i tornar-les a inicialitzar. Què passarà amb les de references lines??????
    m_2DViewer->clearViewer();
    
    // Inicialitzem les diferents línies de projecció visibles al viewer
    for ( int i = 0; i < names.length(); i++)
    {
        QString name = names.at( i );
        QString orientation = orientations.at( i );
        DrawerLine *projectedLine = new DrawerLine();
        projectedLine->setColor( colorsProjectedLines.at(i).value<QColor>() );
        m_projectedLines.insert( projectedLine, ( QStringList () << name << orientation ) );
        m_imagePlanes.insert( name, new ImagePlane() );
    }
}

void ImagePlaneProjectionTool::initializeImagePlanesUpdteVolume( Volume *volume )
{
    m_volume = volume;

    // Guardem a les dades compartides el nou volum de treball
    m_myData->setVolume( volume );

    //TODO
    // S'han de netejar les lines mostrades i tornar-les a inicialitzar. Que passarà amb les de references lines??????
    m_2DViewer->clearViewer();
    
    // Inicialitzem l'imagePlane representat per cada linia de projecció, la dibuixem 
    // i actualitzem les dades compartides indicant el pla projectat
    QMapIterator< DrawerLine *, QStringList > iterator( m_projectedLines );
    while (iterator.hasNext()) 
    {
        iterator.next();
        initializeImagePlane( iterator.key(), volume );
    }

    // Indiquem que s'han inicialitzat les dades perquè si existeix algun visor productor&consumidor pugui
    // actualitzar les dades mostrades
    m_myData->emitDataInitialized();
}

void ImagePlaneProjectionTool::initializeImagePlanesCheckingData()
{
    //TODO
    // S'han de netejar les lines mostrades i tornar-les a inicialitzar. Que passarà amb les de references lines??????
    m_2DViewer->clearViewer();

    QMapIterator< DrawerLine *, QStringList > iterator( m_projectedLines );
    while ( iterator.hasNext() ) 
    {
        iterator.next();
        DrawerLine *projectedLine = iterator.key();
        QString name = iterator.value().at(0);
        ImagePlane *imagePlane;
        imagePlane = m_myData->getProjectedLineImagePlane( name );
        if ( imagePlane != NULL )
        {
            updateProjection( projectedLine, imagePlane, false, false );
        }
        else
        {
            initializeImagePlane( projectedLine, m_myData->getVolume() );
        }
    }
}

void ImagePlaneProjectionTool::initializeImagePlane( DrawerLine *projectedLine, Volume *volume )
{
    // Inicialitzem el pla que ha de projectar la tool sobre el Viewer configurat com a productor
    
    if ( volume != NULL )
    {
        QStringList infoProjectedLine = m_projectedLines[ projectedLine ];
        
        // Pla que ha de projectar la tool sobre el viewer 
        ImagePlane *imagePlane = new ImagePlane();

        // Imatges que composen el volum de dades 
        QList<Image*> images;
        images = volume->getImages();

        // Límits i orientació de la primera imatge del volum 
        Image *firtImage;
        firtImage = images.at(0);

        ImagePlane *firstImagePlane = new ImagePlane();
        firstImagePlane->fillFromImage(firtImage);

        QList< QVector<double> > boundsFirstImagePlane;
        boundsFirstImagePlane = firstImagePlane->getCentralBounds();
        QVector<double> tlhcFirstImagePlane = boundsFirstImagePlane.at(0);
        QVector<double> trhcFirstImagePlane = boundsFirstImagePlane.at(1);
        QVector<double> brhcFirstImagePlane = boundsFirstImagePlane.at(2);
        QVector<double> blhcFirstImagePlane = boundsFirstImagePlane.at(3);

        double rowDirectionVector[3];
        firstImagePlane->getRowDirectionVector(rowDirectionVector);
        int rows = firstImagePlane->getRows();
        double rowLength = firstImagePlane->getRowLength();

        double columnDirectionVector[3];
        firstImagePlane->getColumnDirectionVector(columnDirectionVector);
        int columns = firstImagePlane->getColumns();
        double columnLength = firstImagePlane->getColumnLength();

        // L'espaiat cal obtenir-lo del volum perquè no disposem de la distància entre imatges d'una altra manera
        double spacing[3];
        volume->getSpacing(spacing);

        // Informació sobre la mida i direcció de la pila d'imatges
        int zDepth;
        zDepth = images.size();
        double zDirectionVector[3];
        volume->getStackDirection(zDirectionVector);
        
        // Límits de l'última imatge del volum 
        Image *lastImage;
        lastImage = images.at(zDepth-1);

        ImagePlane *lastImagePlane = new ImagePlane();
        lastImagePlane->fillFromImage(lastImage);

        QList< QVector<double> > boundsLastImagePlane;
        boundsLastImagePlane = lastImagePlane->getCentralBounds();
        QVector<double> tlhcLastImagePlane = boundsLastImagePlane.at(0);
        QVector<double> trhcLasttImagePlane = boundsLastImagePlane.at(1);
        QVector<double> brhcLastImagePlane = boundsLastImagePlane.at(2);
        QVector<double> blhcLastImagePlane = boundsLastImagePlane.at(3);

        double maxXBound, maxYBound, maxZBound;
        double imageOrigin[3];

        // Ajust de la mida del pla a les dimensions de la corresponent orientació
        QString orientation = infoProjectedLine.at( 1 );
        if ( orientation == QString("VERTICAL") )
        {
            //YZ, x-normal
            // No cal sumar spacings perquè ja ho fa a dins l'imagePlane
            maxYBound = sqrt( ( ( double ) zDepth*zDepth + rows*rows ) );
            maxZBound = sqrt( ( ( double ) zDepth*zDepth + rows*rows ) );
            
            imageOrigin[0] = tlhcLastImagePlane[0] + rowDirectionVector[0] * rowLength / 2;
            imageOrigin[1] = tlhcLastImagePlane[1] + rowDirectionVector[1] * rowLength / 2;
            imageOrigin[2] = tlhcLastImagePlane[2] + rowDirectionVector[2] * rowLength / 2;
            
            imagePlane->setOrigin( imageOrigin );
            imagePlane->setRows( zDepth );
            imagePlane->setColumns( rows );
            imagePlane->setSpacing( spacing[1], spacing[2] );
            imagePlane->setThickness( spacing[0] );
            imagePlane->setRowDirectionVector( columnDirectionVector );
            imagePlane->setColumnDirectionVector( -zDirectionVector[0], -zDirectionVector[1], -zDirectionVector[2] );
        }
        else if ( orientation == QString("HORIZONTAL") )
        {
            //XZ, y-normal
            // No cal sumar spacings perquè ja ho fa a dins l'imagePlane
            maxZBound = sqrt( ( ( double ) columns*columns + zDepth*zDepth ) );
            maxXBound = sqrt( ( ( double ) columns*columns + zDepth*zDepth ) );

            imageOrigin[0] = tlhcLastImagePlane[0] + columnDirectionVector[0] * columnLength / 2;
            imageOrigin[1] = tlhcLastImagePlane[1] + columnDirectionVector[1] * columnLength / 2;
            imageOrigin[2] = tlhcLastImagePlane[2] + columnDirectionVector[2] * columnLength / 2;
            
            imagePlane->setOrigin( imageOrigin );
            imagePlane->setRows( zDepth );
            imagePlane->setColumns( columns );
            imagePlane->setSpacing( spacing[0], spacing[2] );
            imagePlane->setThickness( spacing[1] );
            imagePlane->setRowDirectionVector( rowDirectionVector );
            imagePlane->setColumnDirectionVector( -zDirectionVector[0], -zDirectionVector[1], -zDirectionVector[2] );
        }
        
        // Quan es modifica l'ImagePlane de la tool s'actualitza la projecció de la línia corresponent
        updateProjection( projectedLine, imagePlane, false, true );
    }
}

void ImagePlaneProjectionTool::updateProjections()
{
    // Cada cop que al viewer canvia d'input, l'imatge o l'slab s'ha d'obtenir el pla representat per 
    // cada línia projectada i actualitzar la projeccció. També s'actualitza el canvi de pla a les dades compartides
    QMapIterator< DrawerLine *, QStringList > iterator( m_projectedLines );
    while (iterator.hasNext()) 
    {
        iterator.next();
        DrawerLine *projectedLine = iterator.key();
        QStringList infoProjectedLine = iterator.value();
        ImagePlane *imagePlane = m_myData->getProjectedLineImagePlane( infoProjectedLine.at( 0 ) );
        updateProjection( projectedLine, imagePlane );
    }
}

void ImagePlaneProjectionTool::updateProjection( DrawerLine *projectedLine, ImagePlane *imagePlane )
{
    updateProjection( projectedLine, imagePlane, true, true );
}

void ImagePlaneProjectionTool::updateProjection( DrawerLine *projectedLine, ImagePlane *imagePlane, bool projectedLineDrawed, bool updateToolData )
{
    ImagePlane *localizerPlane;

    QString typeConfiguration = m_toolConfiguration->getValue( "typeConfiguration" ).toString();
    if ( typeConfiguration == QString("PRODUCER") )
    {
        localizerPlane = m_2DViewer->getCurrentImagePlane();
    }
    else if ( typeConfiguration == QString("PRODUCER&CONSUMER") )
    {
        localizerPlane = m_myData->getProjectedLineImagePlane( m_nameProjectedLineBind );
    }

    if( !(imagePlane && localizerPlane) )
        return;

    // primer mirem que siguin plans diferents
    if( *imagePlane != *localizerPlane )
    {
        double firstIntersectionPoint[3], secondIntersectionPoint[3];

        // Calculem totes les possibles interseccions
        int numberOfIntersections = imagePlane->getIntersections( localizerPlane, firstIntersectionPoint, secondIntersectionPoint );

        if( numberOfIntersections > 0 )
        {
            // Passem els punts a coordenades de món
            m_2DViewer->projectDICOMPointToCurrentDisplayedImage( firstIntersectionPoint, firstIntersectionPoint );
            m_2DViewer->projectDICOMPointToCurrentDisplayedImage( secondIntersectionPoint, secondIntersectionPoint );

            // Actualitzem la projecció del pla (pintem la línia)
            // La línia projectada indica la intersecció entre el pla projectat per la línia i el pla actual del Viewer
            // ( Només cal tenir en compte eixos x i y de la línia projectada que són els que es veuen per pantalla
            //  així després podrem detectar si l'usuari vol moure la línia, altrament no podríem perquè la distància sortiria molt gran )
            if ( typeConfiguration == QString("PRODUCER") )
            {
                firstIntersectionPoint[2] = 0;
                secondIntersectionPoint[2] = 0;
            }
            else
            {
                double firstPoint[3];
                firstPoint[0] = firstIntersectionPoint[0];
                firstPoint[1] = firstIntersectionPoint[1];
                firstPoint[2] = firstIntersectionPoint[2];

                double secondPoint[3];
                secondPoint[0] = secondIntersectionPoint[0];
                secondPoint[1] = secondIntersectionPoint[1];
                secondPoint[2] = secondIntersectionPoint[2];

                double origin[3];
                localizerPlane->getOrigin( origin );

                // Cal agafar les coordenades x i y en funció de la orientació de la línia projectada
                // i restal-li l'origen perquè en el reslice l'origen és el [0,0,0]
                if ( m_nameProjectedLineBind == "VERTICAL_LINE" )
                {
                    //YZ, x-normal
                    firstIntersectionPoint[0] = abs( firstPoint[1] - origin[1] );
                    firstIntersectionPoint[1] = abs( firstPoint[2] - origin[2] );
                    firstIntersectionPoint[2] = 0;

                    secondIntersectionPoint[0] = abs( secondPoint[1] - origin[1] );
                    secondIntersectionPoint[1] = abs( secondPoint[2] - origin[2] ); 
                    secondIntersectionPoint[2] = 0;
                }
                else if ( m_nameProjectedLineBind == "HORIZONTAL_LINE" )
                {
                    //XZ, y-normal
                    firstIntersectionPoint[0] = abs( firstPoint[0] - origin[0] );
                    firstIntersectionPoint[1] = abs( firstPoint[2] - origin[2] );
                    firstIntersectionPoint[2] = 0;

                    secondIntersectionPoint[0] = abs( secondPoint[0] - origin[0] );
                    secondIntersectionPoint[1] = abs( secondPoint[2] - origin[2] );
                    secondIntersectionPoint[2] = 0;
                }
            }
            
            projectedLine->setFirstPoint( firstIntersectionPoint );
            projectedLine->setSecondPoint( secondIntersectionPoint );

            if ( !projectedLineDrawed )
            {
                m_2DViewer->getDrawer()->draw( projectedLine, Q2DViewer::Top2DPlane );
            }

            if ( !projectedLine->isVisible() )
            {
                projectedLine->visibilityOn();
            }

            projectedLine->update( DrawerPrimitive::VTKRepresentation );

            
            QStringList infoProjectedLine = m_projectedLines[ projectedLine ];
            QString name = infoProjectedLine.at( 0 );

            ImagePlane *copiaImagePlane = new ImagePlane( imagePlane );

            // Actualitzem l'imagePlane projectat per la línia a l'objecte actual
            m_imagePlanes.insert( name, copiaImagePlane );
                        
            if ( updateToolData )
            {
                // Actualitzem l'imagePlane projectat per la línia a les dades compartides de la tool
                m_myData->setProjectedLineImagePlane( name, imagePlane );
            }
        }
        else
        {
            if( projectedLine->isVisible() )
            {
                projectedLine->visibilityOff();
                projectedLine->update( DrawerPrimitive::VTKRepresentation );
            }
        }
    }
    else
    {
        if( projectedLine->isVisible() )
        {
            projectedLine->visibilityOff();
            projectedLine->update( DrawerPrimitive::VTKRepresentation );
        }
    }

    m_2DViewer->render();
}

void ImagePlaneProjectionTool:: checkImagePlaneBindUpdated( QString nameProjectedLine )
{
    if ( m_imagePlanes.contains( nameProjectedLine ) )
    {
        ImagePlane *myImagePlane = m_imagePlanes.value( nameProjectedLine );
        ImagePlane *myDataImagePlane = m_myData->getProjectedLineImagePlane( nameProjectedLine );

        if ( myImagePlane != myDataImagePlane )
        {
            QMapIterator< DrawerLine *, QStringList > iterator( m_projectedLines );
            bool searched = false;
            DrawerLine *projectedLine;
            while ( iterator.hasNext() && !searched ) 
            {
                iterator.next();
                projectedLine = iterator.key();
                QString name = iterator.value().at(0);
                searched = ( nameProjectedLine == name );
            }

            if ( searched ) updateProjection( projectedLine, myDataImagePlane, true, false );
        }
    }
}

void ImagePlaneProjectionTool::checkProjectedLineBindUpdated( QString nameProjectedLine )
{
    if (m_nameProjectedLineBind == nameProjectedLine )
    {
        showImagePlaneProjectedLineBind();
    }
}

void ImagePlaneProjectionTool::showImagePlaneProjectedLineBind()
{
    // Han modificat el pla projectat per la línia associada a l'actual viewer amb la tool configurada com a consumidor
    Volume *volume = m_myData->getVolume();

    if ( volume != NULL )
    {
        if ( m_volume != volume)
        {
            // S'ha modificat el volum amb el que s'ha de treballar als visors que tenen activada la tool,
            // per tant cal inicialitzar el reslice
            m_volume = volume;
            initReslice( volume );
        }
        else
        {
            updateReslice( volume );
        }
    }
}


void ImagePlaneProjectionTool::initReslice( Volume *volume )
{
    m_reslice->SetInput( volume->getVtkData() );

    updateReslice( volume );

    // Assignem la informació de la sèrie, estudis, pacient...
    Volume *reslicedVolume = new Volume;
    reslicedVolume->setImages( volume->getImages() );
    reslicedVolume->setData( m_reslice->GetOutput() );
    m_2DViewer->setInput( reslicedVolume );
}

void ImagePlaneProjectionTool::updateReslice( Volume *volume )
{
    if ( volume == NULL || !m_reslice ||  !( vtkImageData::SafeDownCast( m_reslice->GetInput() ) ) )
    {
        return;
    }

    // Baixem la resolució
    m_reslice->SetInterpolationModeToNearestNeighbor();

    ImagePlane *imagePlane = m_myData->getProjectedLineImagePlane( m_nameProjectedLineBind );

    // Obtenim els vectors directors dels tres eixos de l'image plane que volem mostrar
    double xDirectionVector[3], yDirectionVector[3], zDirectionVector[3];
    imagePlane->getRowDirectionVector( xDirectionVector );
    imagePlane->getColumnDirectionVector( yDirectionVector );
    imagePlane->getNormalVector( zDirectionVector );
    
    // Assignem a la matriu ResliceAxes els eixos de la llesca de sortida
    m_reslice->SetResliceAxesDirectionCosines( xDirectionVector, yDirectionVector, zDirectionVector );

    double origin[3];
    imagePlane->getOrigin(origin);

    // Especifiquem quin ha de ser el punt d'origen de la matriu ResliceAxes
    m_reslice->SetResliceAxesOrigin( origin );

    double spacing[2];
    imagePlane->getSpacing(spacing);

    // Indiquem la distància entre les llesques de sortida que es podran anar obtenint.
    m_reslice->SetOutputSpacing( spacing[0] , spacing[1] , 1 ); // obtenim una única llesca thickness = 1

    // Indiquem quin ha de ser considerat l'origen de les llesques de sortida
    //m_reslice->SetOutputOrigin( origin[0] , origin[1] , origin[2] );
    m_reslice->SetOutputOrigin( 0 , 0 , 0 );

    double rows = imagePlane->getRows();
    double columns = imagePlane->getColumns();

    // Límits de les llesques de sortida
    m_reslice->SetOutputExtent( 0 , columns - 1 , 0 , rows - 1 , 0 , 0 );

    // Fem efectius els canvis fets anteriorment sobre el vtkImageReslace
    m_reslice->Update();

    // Tornem ha augmentar la resolució
    m_reslice->SetInterpolationModeToCubic();

    // Visualitzem els canvis al viewer
    m_2DViewer->render();
}

void ImagePlaneProjectionTool::setToolData(ToolData * data)
{
    if ( !m_toolConfiguration )
    {
        DEBUG_LOG(QString("ImagePlaneProjectionTool: És obligatori associar a la tool una configuració correcta.") );
        return;
    }

    QString typeConfiguration = m_toolConfiguration->getValue( "typeConfiguration" ).toString();

    // Desfem els vincles anteriors
    if ( typeConfiguration == QString( "PRODUCER" ) )
    {
        disconnect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), this, SLOT( checkImagePlaneBindUpdated( QString ) ) );
    }
    if ( typeConfiguration == QString( "CONSUMER" ) )
    {
        disconnect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), this, SLOT( checkProjectedLineBindUpdated( QString ) ) );
    }
    if ( typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
    {
        disconnect( m_myData, SIGNAL( dataInitialized() ), this, SLOT( initializeImagePlanesCheckingData() ) );
        disconnect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), this, SLOT( checkImagePlaneBindUpdated( QString ) ) );
        disconnect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), this, SLOT( checkProjectedLineBindUpdated( QString ) ) );
    }

    delete m_myData;

    // Creem de nou les dades
    m_toolData = data;
    m_myData = qobject_cast<ImagePlaneProjectionToolData *>(data);
    
    if ( typeConfiguration == QString( "PRODUCER" ) )
    {
        connect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), SLOT( checkImagePlaneBindUpdated( QString ) ) );

        if ( m_volume != NULL )
        {
            // Quan es modifiquen les dades compartides de la tool cal tornar-les a actualitzar assignant 
            // el volum i el pla projectat de cada línia del viewer
            initializeImagePlanesUpdteVolume( m_volume );
        }
    }
    else
    {
        Volume *volume = m_myData->getVolume();

        if ( typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
        {
            connect( m_myData, SIGNAL( dataInitialized() ), SLOT( initializeImagePlanesCheckingData() ) );
            connect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), SLOT( checkImagePlaneBindUpdated( QString ) ) );
            
            if (volume != NULL)
            {
                initializeImagePlanesCheckingData();
            }
        }
        
        if ( typeConfiguration == QString( "CONSUMER" ) || typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
        {
            connect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), SLOT( checkProjectedLineBindUpdated( QString ) ) );
            
            if (volume != NULL)
            {
                // Quan es modifiquen les dades compartides de la tool i ja ha estat assignat el volum de treball
                // cal mostrar el pla projectat per la seva línia associada
                showImagePlaneProjectedLineBind();
            }
        }
    }
}

void ImagePlaneProjectionTool::handleEvent( long unsigned eventID )
{
    if( !m_toolConfiguration )
    {
        DEBUG_LOG("No s'ha assignat cap configuració. La tool no funcionarà com s'espera.");
        return;
    }

    QString typeConfiguration = m_toolConfiguration->getValue( "typeConfiguration" ).toString();
    if ( typeConfiguration == QString("PRODUCER") || typeConfiguration == QString("PRODUCER&CONSUMER") )
    {
        switch( eventID )
        {
            case vtkCommand::LeftButtonPressEvent:
                detectManipulationProjectedLine();
                break;

            case vtkCommand::LeftButtonReleaseEvent:
                releaseProjectedLine();
                break;

            case vtkCommand::MouseMoveEvent:
                if( m_state == ROTATING )
                {
                    rotateProjectedLine();
                }
                else if( m_state == PUSHING )
                {
                    pushProjectedLine();
                }
                break;

            default:
                break;
        }
    }
}

void ImagePlaneProjectionTool::detectManipulationProjectedLine()
{
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate( clickedWorldPoint );

    // detectem si el clic ha estat aprop d'alguna línia projectada
    double point[3] = { clickedWorldPoint[0] , clickedWorldPoint[1] , 0.0 };
    double distanceToProjectedLine;
    DrawerLine *projectedLine;
    bool picked = false;

    QMapIterator< DrawerLine *, QStringList > iterator( m_projectedLines );
    while (iterator.hasNext() && !picked ) 
    {
        iterator.next();
        projectedLine = iterator.key();
    
        distanceToProjectedLine = projectedLine->getDistanceToPoint( point );

        // donem una "tolerància" mínima
        if( distanceToProjectedLine < 50.0 )
        {
            m_pickedProjectedLine = projectedLine;

            QStringList infoProjectedLine = m_projectedLines[ m_pickedProjectedLine ];
            m_pickedProjectedLineName = infoProjectedLine.at( 0 );

            if ( m_2DViewer->getInteractor()->GetControlKey() )
            {
                m_state = PUSHING;
            }
            else
            {
                m_state = ROTATING;
            }

            m_initialPickX = clickedWorldPoint[0];
            m_initialPickY = clickedWorldPoint[1];

            m_2DViewer->setCursor( QCursor( Qt::OpenHandCursor ) );

            picked = true;
        }
    }
}

void ImagePlaneProjectionTool::rotateProjectedLine()
{
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate( clickedWorldPoint );

    double vec1[3], vec2[3];
    double axis[3];
    double direction[3];
    double center[3];

    ImagePlane *imagePlane = m_myData->getProjectedLineImagePlane( m_pickedProjectedLineName );

    imagePlane->getCenter( center );

    vec1[0] = m_initialPickX - center[0];
    vec1[1] = m_initialPickY - center[1];
    vec1[2] = 0.0;

    vec2[0] = clickedWorldPoint[0] - center[0];
    vec2[1] = clickedWorldPoint[1] - center[1];
    vec2[2] = 0.0;

    double degrees = MathTools::angleInDegrees( vec1 , vec2 );

    m_initialPickX = clickedWorldPoint[0];
    m_initialPickY = clickedWorldPoint[1];

    MathTools::crossProduct( vec1 , vec2 , direction );
    this->getRotationAxisImagePlane( imagePlane, axis );
    double dot = MathTools::dotProduct( direction , axis );

    axis[0] *= dot;
    axis[1] *= dot;
    axis[2] *= dot;

    MathTools::normalize( axis );
    this->rotateMiddleImagePlane( imagePlane, degrees , axis );

    this->updateProjection( m_pickedProjectedLine, imagePlane );
}

void ImagePlaneProjectionTool::getRotationAxisImagePlane( ImagePlane *imagePlane, double axis[3] )
{
    if( !imagePlane )
        return;

    double columnDirectionVector[3];
    imagePlane->getColumnDirectionVector( columnDirectionVector );

    axis[0] = -columnDirectionVector[0];
    axis[1] = -columnDirectionVector[1];
    axis[2] = -columnDirectionVector[2];
}

void ImagePlaneProjectionTool::rotateMiddleImagePlane( ImagePlane *imagePlane, double degrees , double rotationAxis[3] )
{
    vtkTransform *m_transform = vtkTransform::New();
    m_transform->Identity();
    
    double centre[3];
    imagePlane->getCenter( centre );
    
    m_transform->Translate( centre[0], centre[1], centre[2] );
    m_transform->RotateWXYZ( degrees , rotationAxis );
    m_transform->Translate( -centre[0], -centre[1], -centre[2] );

    // ara que tenim la transformació, l'apliquem als punts del pla ( origen, punt1 , punt2)
    
    double rowDirectionVector[3], columnDirectionVector[3], origin[3];
    imagePlane->getRowDirectionVector( rowDirectionVector );
    imagePlane->getColumnDirectionVector( columnDirectionVector );
    imagePlane->getOrigin( origin );

    double point1[3], point2[3];
    for ( int i=0; i < 3; i++ )
    {
        point1[i] = rowDirectionVector[i] + origin[i];
        point2[i] = columnDirectionVector[i] + origin[i];
    }

    double newOrigin[3];
    m_transform->TransformPoint( origin, newOrigin );
    imagePlane->setOrigin( newOrigin );

    double newPoint1[3], newPoint2[3];
    m_transform->TransformPoint( point1 , newPoint1 );
    m_transform->TransformPoint( point2 , newPoint2 );

    for ( int i=0; i < 3; i++ )
    {
        rowDirectionVector[i] = newPoint1[i] - newOrigin[i];
        columnDirectionVector[i] = newPoint2[i] - newOrigin[i];
    }
    imagePlane->setRowDirectionVector( rowDirectionVector );  
    imagePlane->setColumnDirectionVector( columnDirectionVector );
}

void ImagePlaneProjectionTool::pushProjectedLine()
{
    ImagePlane *imagePlane = m_myData->getProjectedLineImagePlane( m_pickedProjectedLineName );

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate( clickedWorldPoint );

    // Get the motion vector
    double v[3];
    v[0] = clickedWorldPoint[0] - m_initialPickX;
    v[1] = clickedWorldPoint[1] - m_initialPickY;
    v[2] = 0.0;

    double normal[3];
    imagePlane->getNormalVector( normal );
    imagePlane->push( MathTools::dotProduct( v, normal ) );

    this->updateProjection( m_pickedProjectedLine, imagePlane );

    m_initialPickX = clickedWorldPoint[0];
    m_initialPickY = clickedWorldPoint[1];
}

void ImagePlaneProjectionTool::releaseProjectedLine()
{
    m_state = NONE;
    m_2DViewer->setCursor( QCursor( Qt::ArrowCursor ) );
}

}
