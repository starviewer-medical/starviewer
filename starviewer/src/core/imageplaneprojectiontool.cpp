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
#include <vtkMath.h>
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
        if ( orientation != QString("AXIAL") && orientation != QString("SAGITAL") && orientation != QString("CORONAL") )
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
        // Cada cop que el viewer canvïi d'input s'ha d'actualitzar el volum compartit amb la resta de visors amb la tool activa
        connect( m_2DViewer, SIGNAL( volumeChanged( Volume * ) ), SLOT( updateVolume( Volume * ) ) );

        // Cada cop que el viewer canvïi d'input s'ha d'inicialitzar el pla projectat per cada línia de projecció de la tool
        connect( m_2DViewer, SIGNAL( volumeChanged( Volume * ) ), SLOT( initializeImagePlanes() ) );

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
        connect( m_myData, SIGNAL( volumeChanged( Volume * ) ), SLOT( initializeImagePlanesCheckingData() ) );
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
    m_2DViewer->getDrawer()->clearViewer();
    
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

void ImagePlaneProjectionTool::updateVolume( Volume *volume )
{
    m_volume = volume;
    m_myData->setVolume( volume );
}

void ImagePlaneProjectionTool::initializeImagePlanes( )
{
    //TODO
    // S'han de netejar les lines mostrades i tornar-les a inicialitzar. Que passarà amb les de references lines??????
    m_2DViewer->getDrawer()->clearViewer();
    
    // Inicialitzem l'imagePlane representat per cada linia de projecció, la dibuixem 
    // i actualitzem les dades compartides indicant el pla projectat
    QMapIterator< DrawerLine *, QStringList > iterator( m_projectedLines );
    while (iterator.hasNext()) 
    {
        iterator.next();
        initializeImagePlane( iterator.key() );
    }
}

void ImagePlaneProjectionTool::initializeImagePlanesCheckingData()
{
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
            updateProjection( projectedLine, imagePlane, true );
        }
        else
        {
            initializeImagePlane( projectedLine );
        }
    }
}

void ImagePlaneProjectionTool::initializeImagePlane( DrawerLine *projectedLine )
{
    // Inicialitzem el pla que ha de projectar la tool sobre el Viewer configurat com a productor
    Volume *volume = m_myData->getVolume();    

    if ( volume != NULL )
    {
        QStringList infoProjectedLine = m_projectedLines[ projectedLine ];
        
        // Pla que ha de projectar la tool sobre el viewer 
        ImagePlane *imagePlane = new ImagePlane();

        int extent[6];
        volume->getWholeExtent(extent);
        double origin[3];
        volume->getOrigin(origin);
        double spacing[3];
        volume->getSpacing(spacing);

        // Prevent obscuring voxels by offsetting the plane geometry
        double xbounds[] = {origin[0] + spacing[0] * (extent[0] - 0.5) ,
                            origin[0] + spacing[0] * (extent[1] + 0.5)};
        double ybounds[] = {origin[1] + spacing[1] * (extent[2] - 0.5),
                            origin[1] + spacing[1] * (extent[3] + 0.5)};
        double zbounds[] = {origin[2] + spacing[2] * (extent[4] - 0.5),
                            origin[2] + spacing[2] * (extent[5] + 0.5)};

        if ( spacing[0] < 0.0 )
        {
            double t = xbounds[0];
            xbounds[0] = xbounds[1];
            xbounds[1] = t;
        }
        if ( spacing[1] < 0.0 )
        {
            double t = ybounds[0];
            ybounds[0] = ybounds[1];
            ybounds[1] = t;
        }
        if ( spacing[2] < 0.0 )
        {
            double t = zbounds[0];
            zbounds[0] = zbounds[1];
            zbounds[1] = t;
        }
        
        double maxXBound, maxYBound, maxZBound;
        double imageOrigin[3], point1[3], point2[3];
        double *rowDirectionVector, *columnDirectionVector;

        QString orientation = infoProjectedLine.at( 1 );
        if ( orientation == QString("AXIAL") )
        {
            DEBUG_LOG(QString(" orientation axial"));
            //XY, z-normal : vista axial , en principi d'aquesta vista nomès canviarem la llesca
            maxXBound = sqrt( xbounds[1]*xbounds[1] + ybounds[1]*ybounds[1] );
            maxYBound = sqrt( xbounds[1]*xbounds[1] + ybounds[1]*ybounds[1] );

            imageOrigin[0] = xbounds[0];
            imageOrigin[1] = ybounds[1];
            imageOrigin[2] = zbounds[1];

            point1[0] = xbounds[1];
            point1[1] = ybounds[1];
            point1[2] = zbounds[1];

            point2[0] = xbounds[0];
            point2[1] = ybounds[0];
            point2[2] = zbounds[1];

            imagePlane->setOrigin( imageOrigin );
            imagePlane->setRows(maxXBound);
            imagePlane->setColumns(maxYBound);
            imagePlane->setSpacing(spacing[1],spacing[2]);

            rowDirectionVector = MathTools::directorVector( imageOrigin, point1 );
            imagePlane->setRowDirectionVector( rowDirectionVector  );

            columnDirectionVector = MathTools::directorVector( imageOrigin, point2 );
            imagePlane->setColumnDirectionVector( columnDirectionVector );
        }
        else if ( orientation == QString("SAGITAL") )
        {
            DEBUG_LOG(QString(" orientation sagital"));
            //YZ, x-normal : vista sagital
            // estem ajustant la mida del pla a les dimensions d'aquesta orientació
            // TODO podríem donar unes mides a cada punt que fossin suficientment grans com per poder mostrejar qualssevol orientació en el volum, potser fent una bounding box o simplement d'una forma més "bruta" doblant la longitud d'aquest pla :P
            maxYBound = sqrt( ybounds[1]*ybounds[1] + zbounds[1]*zbounds[1] );
            maxZBound = sqrt( ybounds[1]*ybounds[1] + zbounds[1]*zbounds[1] );

            imageOrigin[0] = xbounds[0];
            imageOrigin[1] = ybounds[0];
            imageOrigin[2] = zbounds[1];

            point1[0] = xbounds[0];
            point1[1] = ybounds[1];
            point1[2] = zbounds[1];

            point2[0] = xbounds[0];
            point2[1] = ybounds[0];
            point2[2] = zbounds[0];

            imagePlane->setOrigin( imageOrigin );
            imagePlane->setRows(maxYBound);
            imagePlane->setColumns(maxZBound);
            imagePlane->setSpacing(spacing[1],spacing[2]);

            rowDirectionVector = MathTools::directorVector( imageOrigin, point1 );
            imagePlane->setRowDirectionVector( rowDirectionVector  );

            columnDirectionVector = MathTools::directorVector( imageOrigin, point2 );
            imagePlane->setColumnDirectionVector( columnDirectionVector );

            imagePlane->push( -0.5 * ( xbounds[1] - xbounds[0] ) );
        }
        else if ( orientation == QString("CORONAL") )
        {
            DEBUG_LOG(QString(" orientation coronal"));
            //ZX, y-normal : vista coronal
            // estem ajustant la mida del pla a les dimensions d'aquesta orientació
            // TODO podríem donar unes mides a cada punt que fossin suficientment grans com per poder mostrejar qualssevol orientació en el volum, potser fent una bounding box o simplement d'una forma més "bruta" doblant la longitud d'aquest pla :P
            // TODO comprovar si és correcte aquest ajustament de mides
            maxZBound = sqrt( ybounds[1]*ybounds[1] + xbounds[1]*xbounds[1] );
            maxXBound = sqrt( ybounds[1]*ybounds[1] + xbounds[1]*xbounds[1] );
            double diffXBound = maxXBound - xbounds[1];
            double diffZBound = maxZBound - zbounds[1];

            imageOrigin[0] = xbounds[0] - diffXBound*0.5;
            imageOrigin[1] = ybounds[0];
            imageOrigin[2] = zbounds[1] + diffZBound*0.5;

            point1[0] = xbounds[1] + diffXBound*0.5;
            point1[1] = ybounds[0];
            point1[2] = zbounds[1] + diffZBound*0.5;

            point2[0] = xbounds[0] - diffXBound*0.5;
            point2[1] = ybounds[0];
            point2[2] = zbounds[0] - diffZBound*0.5;

            imagePlane->setOrigin( imageOrigin );
            imagePlane->setRows(maxXBound);
            imagePlane->setColumns(maxZBound);
            imagePlane->setSpacing(spacing[0],spacing[2]);

            rowDirectionVector = MathTools::directorVector( imageOrigin, point1 );
            imagePlane->setRowDirectionVector( rowDirectionVector  );

            columnDirectionVector = MathTools::directorVector( imageOrigin, point2 );
            imagePlane->setColumnDirectionVector( columnDirectionVector );

            // posem a la llesca central
            imagePlane->push(  0.5 * ( ybounds[1] - ybounds[0] ) );
        }
        
        // Quan es modifica l'ImagePlane de la tool s'actualitza la projecció de la línia corresponent
        updateProjection( projectedLine, imagePlane, false );
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
    updateProjection( projectedLine, imagePlane, true );
}

void ImagePlaneProjectionTool::updateProjection( DrawerLine *projectedLine, ImagePlane *imagePlane, bool projectedLineDrawed )
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

    DEBUG_LOG(QString("Image plane viewer"));
    DEBUG_LOG(localizerPlane->toString(true));

    DEBUG_LOG(QString("Image plane tool"));
    DEBUG_LOG(imagePlane->toString(true));

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
            //  (La línia projectada indica la intersecció entre el pla projectat per la línia i el pla actual del Viewer)
            //TODO
            //Si no assigno un 0 a la tercera posició dels punts d'intersecció la distància amb la línia surt molt gran [382506]
            firstIntersectionPoint[2] = 0;
            secondIntersectionPoint[2] = 0;
            projectedLine->setFirstPoint( firstIntersectionPoint );
            projectedLine->setSecondPoint( secondIntersectionPoint );

            DEBUG_LOG(QString("Valor del primer punt d'intersecció = [%1,%2,%3]").arg(firstIntersectionPoint[0]).arg(firstIntersectionPoint[1]).arg(firstIntersectionPoint[2]));
            DEBUG_LOG(QString("Valor del segon punt d'intersecció = [%1,%2,%3]").arg(secondIntersectionPoint[0]).arg(secondIntersectionPoint[1]).arg(secondIntersectionPoint[2]));

            if ( !projectedLineDrawed )
            {
                m_2DViewer->getDrawer()->draw( projectedLine, Q2DViewer::Top2DPlane );
                projectedLineDrawed = true;
            }

            if ( !projectedLine->isVisible() )
            {
                projectedLine->visibilityOn();
            }

            projectedLine->update( DrawerPrimitive::VTKRepresentation );

            // Actualitzem l'imagePlane projectat per la línia a les dades compartides de la tool
            QStringList infoProjectedLine = m_projectedLines[ projectedLine ];
            QString name = infoProjectedLine.at( 0 );
            m_imagePlanes.insert( name, imagePlane );
            m_myData->setProjectedLineImagePlane( name, imagePlane );
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

    m_2DViewer->getDrawer()->refresh();
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

            if ( searched ) updateProjection( projectedLine, myDataImagePlane, true );
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
    Volume *volumeResliced = new Volume;
    volumeResliced->setImages( volume->getImages() );
    volumeResliced->setData( m_reslice->GetOutput() );
    m_2DViewer->setInput( volumeResliced ); 
}

void ImagePlaneProjectionTool::updateReslice( Volume *volume )
{
    if ( volume == NULL || !m_reslice ||  !( vtkImageData::SafeDownCast( m_reslice->GetInput() ) ) )
    {
        return;
    }

    m_reslice->SetInterpolationModeToNearestNeighbor();

    ImagePlane *imagePlane = m_myData->getProjectedLineImagePlane( m_nameProjectedLineBind );

    DEBUG_LOG(QString("Image plane updateReslice"));
    DEBUG_LOG(imagePlane->toString(true));

    // Calculate appropriate pixel spacing for the reslicing
    double spacing[3];
    volume->getSpacing( spacing );

    int i;

    // Origen del volum de dades
    double origin[3];
    volume->getOrigin( origin );

    // Obtenim l'extensió(àrea) del volum
    int extent[6];
    volume->getWholeExtent(extent);

    double bounds[] = { origin[0] + spacing[0]*extent[0], //xmin
                        origin[0] + spacing[0]*extent[1], //xmax
                        origin[1] + spacing[1]*extent[2], //ymin
                        origin[1] + spacing[1]*extent[3], //ymax
                        origin[2] + spacing[2]*extent[4], //zmin
                        origin[2] + spacing[2]*extent[5] };//zmax

    for ( i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
    {
        if ( bounds[i] > bounds[i+1] )
        {
            double t = bounds[i+1];
            bounds[i+1] = bounds[i];
            bounds[i] = t;
        }
    }

    double abs_normal[3];
    imagePlane->getNormalVector(abs_normal);

    double nmax = 0.0;
    int k = 0;
    for ( i = 0; i < 3; i++ )
    {
        abs_normal[i] = fabs(abs_normal[i]);
        if ( abs_normal[i]>nmax )
        {
            nmax = abs_normal[i];
            k = i;
        }
    }

    double planeCenter[3];
    imagePlane->getCenter(planeCenter);
    
    // Force the plane to lie within the true image bounds along its normal
    //
    if ( planeCenter[k] > bounds[2*k+1] )
    {
        planeCenter[k] = bounds[2*k+1];
    }
    else if ( planeCenter[k] < bounds[2*k] )
    {
        planeCenter[k] = bounds[2*k];
    }
    imagePlane->setCenter(planeCenter);

    // obtenim els vectors
    double planeAxis1[3], planeAxis2[3], rowDirectionVector[3], columnDirectionVector[3];
    imagePlane->getRowDirectionVector( rowDirectionVector );
    imagePlane->getColumnDirectionVector( columnDirectionVector );

    planeAxis1[0] = rowDirectionVector[0];
    planeAxis1[1] = rowDirectionVector[1];
    planeAxis1[2] = rowDirectionVector[2];

    planeAxis2[0] = columnDirectionVector[0];
    planeAxis2[1] = columnDirectionVector[1];
    planeAxis2[2] = columnDirectionVector[2];

    // The x,y dimensions of the plane
    //
    double planeSizeX = vtkMath::Normalize( planeAxis1 );
    double planeSizeY = vtkMath::Normalize( planeAxis2 );

    double normal[3];
    imagePlane->getNormalVector( normal );

    // Generate the slicing matrix
    //
    // Podria ser membre de classe, com era originariament o passar per paràmetre
    // Aquesta matriu serveix per indicar els eixos dels voxels de sortida
    // - Primera columna: vector indicant l'eix de les x (x1, x2, x3, 1)
    // - Segona columna: vector indicant l'eix de les y (y1, y2, y3, 1)
    // - Tercera columna: vector indicant l'eix de les z (z1, z2, z3, 1)
    // - Quarta columna: vector indicant l'origen dels eixos
    vtkMatrix4x4 *resliceAxes = vtkMatrix4x4::New();
    resliceAxes->Identity();
    for ( i = 0; i < 3; i++ )
    {
        resliceAxes->SetElement(0,i,planeAxis1[i]);
        resliceAxes->SetElement(1,i,planeAxis2[i]);
        resliceAxes->SetElement(2,i,normal[i]);
    }

    double planeOrigin[4];
    imagePlane->getOrigin(planeOrigin);
    planeOrigin[3] = 1.0;
    double originXYZW[4];
    resliceAxes->MultiplyPoint(planeOrigin,originXYZW);

    resliceAxes->Transpose();
    double neworiginXYZW[4];
    double point[] =  {originXYZW[0],originXYZW[1],originXYZW[2],originXYZW[3]};
    resliceAxes->MultiplyPoint(point,neworiginXYZW);

    resliceAxes->SetElement(0,3,neworiginXYZW[0]);
    resliceAxes->SetElement(1,3,neworiginXYZW[1]);
    resliceAxes->SetElement(2,3,neworiginXYZW[2]);

    // Assignem la matriu d'eixos al nostre vtkImageReslice
    m_reslice->SetResliceAxes( resliceAxes );

    double spacingX = fabs(planeAxis1[0]*spacing[0])+\
                      fabs(planeAxis1[1]*spacing[1])+\
                      fabs(planeAxis1[2]*spacing[2]);

    double spacingY = fabs(planeAxis2[0]*spacing[0])+\
                    fabs(planeAxis2[1]*spacing[1])+\
                    fabs(planeAxis2[2]*spacing[2]);


    // Pad extent up to a power of two for efficient texture mapping

    // make sure we're working with valid values
    double realExtentX = ( spacingX == 0 ) ? 0 : planeSizeX / spacingX;

    int extentX;
    // Sanity check the input data:
    // * if realExtentX is too large, extentX will wrap
    // * if spacingX is 0, things will blow up.
    // * if realExtentX is naturally 0 or < 0, the padding will yield an
    //   extentX of 1, which is also not desirable if the input data is invalid.
    if (realExtentX > (VTK_INT_MAX >> 1) || realExtentX < 1)
    {
        WARN_LOG( "Invalid X extent. [" + QString::number( realExtentX ) + "] Perhaps the input data is empty?" );
        extentX = 0;
    }
    else
    {
        extentX = 1;
        while (extentX < realExtentX)
        {
            extentX = extentX << 1;
        }
    }

    // make sure extentY doesn't wrap during padding
    double realExtentY = ( spacingY == 0 ) ? 0 : planeSizeY / spacingY;

    int extentY;
    if (realExtentY > (VTK_INT_MAX >> 1) || realExtentY < 1)
    {
        WARN_LOG( "Invalid Y extent. [" + QString::number( realExtentY ) + "] Perhaps the input data is empty?" );
        extentY = 0;
    }
    else
    {
        extentY = 1;
        while (extentY < realExtentY)
        {
            extentY = extentY << 1;
        }
    }

    // Indiquem la distància entre les llesques de sortida que es podran anar obtenint.
    m_reslice->SetOutputSpacing( planeSizeX/extentX , planeSizeY/extentY , 1 );
    // Indiquem quin ha de ser considerat l'origen de les llesques de sortida
    m_reslice->SetOutputOrigin( 0.0 , 0.0 , 0.0 );
    // Gruix de les llesques de sortida
    m_reslice->SetOutputExtent( 0 , extentX-1 , 0 , extentY-1 , 0 , 0 ); // obtenim una única llesca
    // Fem efectius els canvis fets anteriorment sobre el vtkImageReslace
    m_reslice->Update();

    //TODO 
    // No se si cal aquesta interpolació
    m_reslice->SetInterpolationModeToCubic();
    m_2DViewer->refresh();
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
        disconnect( m_myData, SIGNAL( volumeChanged( Volume * ) ), this, SLOT( initializeImagePlanesCheckingData() ) );
        disconnect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), this, SLOT( checkImagePlaneBindUpdated( QString ) ) );
        disconnect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), this, SLOT( checkProjectedLineBindUpdated( QString ) ) );
    }

    delete m_myData;

    // Creem de nou les dades
    m_toolData = data;
    m_myData = qobject_cast<ImagePlaneProjectionToolData *>(data);
    Volume *volume = m_myData->getVolume();
    
    if ( m_volume != NULL && typeConfiguration == QString( "PRODUCER" ) )
    {
        // Quan es modifiquen les dades compartides de la tool cal tornar-les a actualitzar assignant 
        // el volum i el pla projectat de cada línia del viewer
        updateVolume( m_volume );
        initializeImagePlanes();
    }

    if ( typeConfiguration == QString( "PRODUCER&CONSUMER" ) )
    {
        connect( m_myData, SIGNAL( volumeChanged( Volume * ) ), SLOT( initializeImagePlanesCheckingData() ) );
        connect( m_myData, SIGNAL( imagePlaneUpdated( QString ) ), SLOT( checkImagePlaneBindUpdated( QString ) ) );
        initializeImagePlanesCheckingData();
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

    vtkMath::Cross( vec1 , vec2 , direction );
    this->getRotationAxisImagePlane( imagePlane, axis );
    double dot = vtkMath::Dot( direction , axis );

    axis[0] *= dot;
    axis[1] *= dot;
    axis[2] *= dot;

    vtkMath::Normalize( axis );
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
    imagePlane->push( vtkMath::Dot( v, normal ) );

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
