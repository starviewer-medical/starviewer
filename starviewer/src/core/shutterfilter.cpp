/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "shutterfilter.h"

#include <vtkImageData.h>
#include <vtkPolyDataToImageStencil.h>
#include "vtkAtamaiPolyDataToImageStencil2.h"
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkRegularPolygonSource.h>
#include <vtkImageMask.h>

#include "volume.h"
#include "logging.h"

// dcmtk: presentation state
//\TODO hem de fer aquest define perquè sinó no compila. Caldria descobrir perquè passa això i si cal fer un altre include previ
#define HAVE_CONFIG_H 1
#include <dvpstat.h> // DVPresentationState
#include <dcfilefo.h> // per carregar arxius dicom
#include "dcmtk/dcmdata/dcdatset.h"

namespace udg {

ShutterFilter::ShutterFilter()
{
    m_rectangularPolyDataStencil = 0;
    m_circularPolyDataStencil = 0;
    m_polygonalPolyDataStencil = 0;
    m_inputData = 0;
    // per defecte el background és negre
    m_background = 0;
}

ShutterFilter::~ShutterFilter()
{
}

void ShutterFilter::setInput( Volume *volume )
{
    m_inputData = volume;
}

void ShutterFilter::setPresentationStateShutters( const char *presentationStateFile )
{
    if( m_inputData )
    {
        DVPresentationState *presentationStateHandler = new DVPresentationState;
        DcmDataset *presentationStateData = NULL;
        DcmFileFormat presentationStateFileFormat;
        OFCondition status = presentationStateFileFormat.loadFile( presentationStateFile );
        if( status.good() )
        {
            presentationStateData = presentationStateFileFormat.getDataset();
            status = presentationStateHandler->read( *presentationStateData );
            if( status.good() )
            {
                // obtenim els paràmetres de les coordenades per mapejar bé els punts a l'espai
                m_inputData->updateInformation();
                double origin[3];
                double spacing[3];
                m_inputData->getOrigin( origin );
                m_inputData->getSpacing( spacing );
                // càlcul d'un background adequat amb el rang de dades. Els valors de background poden anar de 0 o 65535 i el rang de dades pot ser qualsevol altre, com per exemple 127..255;
                double range[2];
                int backValue = presentationStateHandler->getShutterPresentationValue();
                m_inputData->getVtkData()->GetScalarRange( range );
                if( backValue == 0 )
                    m_background = range[0];
                else
                    m_background = backValue *(range[1] / backValue) + range[0];

                DEBUG_LOG( qPrintable( QString("Valor de background del Shutter donat pel presentation state: %1").arg( m_background ) ) );

                if( presentationStateHandler->haveShutter( DVPSU_rectangular ) )
                {
                    DEBUG_LOG("Hi ha RECTANGULAR shutter al presentation state");
                    this->setRectangularShutter( origin[0] + (presentationStateHandler->getRectShutterLV()-1)*spacing[0],
                                origin[0] + (presentationStateHandler->getRectShutterRV()-1)*spacing[0],
                                origin[1] + (presentationStateHandler->getRectShutterUH()-1)*spacing[1],
                                origin[1] + (presentationStateHandler->getRectShutterLH()-1)*spacing[1]
                                );
                }
                else
                    DEBUG_LOG("No hi ha RECTANGULAR shutter al presentation state");

                if( presentationStateHandler->haveShutter( DVPSU_circular ) )
                {
                    DEBUG_LOG("Hi ha CIRCULAR shutter al presentation state");

                    double center[2] = {
                        origin[0] + (presentationStateHandler->getCenterOfCircularShutter_x()-1)*spacing[0],
                        origin[1] + (presentationStateHandler->getCenterOfCircularShutter_y()-1)*spacing[1]
                    };
                    // Radius of the cicular shutter with to respect to pixels in the image given as a number of pixels along the row direction-> per tant cal calcular bé quan mesura respecte les coordenades de món
                    double radius = (origin[0] + (presentationStateHandler->getRadiusOfCircularShutter()-1)*spacing[0]) - origin[0];
                    this->setCircularShutter( center, radius );
                }
                else
                    DEBUG_LOG("No hi ha CIRCULAR shutter al presentation state");

                if( presentationStateHandler->haveShutter( DVPSU_polygonal ) )
                {
                    int vertices = presentationStateHandler->getNumberOfPolyShutterVertices();
                    QString msg = "Hi ha POLYGONAL shutter al presentation state, #de punts:";
                    msg += QString("%1 amb coordenades: ").arg( vertices );
                    //\TODO caldria passar això al mètode propi de la classe?
                    // creem els punts
                    vtkPoints *points = vtkPoints::New();
                    vtkCellArray *vertexs = vtkCellArray::New();
                    vertexs->InsertNextCell( vertices + 1 );
                    Sint32 polyX, polyY;
                    double x, y;
                    for( int i = 0; i < vertices; i++ )
                    {
                        if( EC_Normal == presentationStateHandler->getPolyShutterVertex( i, polyX, polyY ) )
                        {
                            x = origin[0] + (polyX-1.0)*spacing[0];
                            y = origin[1] + (polyY-1.0)*spacing[1];
                            msg += QString("[%1,%2]\n").arg( x ).arg( y );
                            points->InsertPoint( i , x, y, origin[2] - 1.0 );
                            vertexs->InsertCellPoint( i );
                        }
                        else
                            msg += "???\\???,\n";
                    }
                    // per tancar el polígon
                    vertexs->InsertCellPoint( 0 );
                    vtkPolyData *polyData = vtkPolyData::New();
                    polyData->SetPoints( points );
                    polyData->SetLines( vertexs );

                    vtkLinearExtrusionFilter *extruder = vtkLinearExtrusionFilter::New();
                    extruder->SetScaleFactor(10);
                    extruder->SetExtrusionTypeToNormalExtrusion();
                    extruder->SetVector( 0 , 0 , 1 ); // assumirem ara per ara, només es fa en axial
                    extruder->SetInput( polyData );

                    // creem l'stencil
                    m_polygonalPolyDataStencil = vtkAtamaiPolyDataToImageStencil2::New();
                    m_polygonalPolyDataStencil->SetInput( extruder->GetOutput() );
                    extruder->Delete();

                    DEBUG_LOG( qPrintable( QString( msg ) ) );
                }
                else
                    DEBUG_LOG( "No hi ha POLYGONAL shutter al presentation state" );

                if( presentationStateHandler->haveShutter( DVPSU_bitmap ) )
                {
                    DEBUG_LOG( "Hi ha BITMAP shutter al presentation state" );
                    int overlays = presentationStateHandler->getNumberOfOverlaysInPresentationState();
                    DEBUG_LOG( qPrintable( QString("Nombre d'overlays en el PS: %1").arg( overlays ) ) );

                    for( int overlayIndex = 0; overlayIndex < overlays; overlayIndex++ )
                    {
                        if( presentationStateHandler->overlayIsSuitableAsBitmapShutter( overlayIndex ) )
                        {
                            int layer = presentationStateHandler->getOverlayInPresentationStateActivationLayer( overlayIndex );

                            DEBUG_LOG( qPrintable( QString("L'overlay %1 és bo com a bitmap shutter i es pinta al layer #%2").arg( overlayIndex ).arg( layer ) ) );

                            if( presentationStateHandler->overlayInPresentationStateIsROI( overlayIndex ) )
                                std::cout << "L'overlay és un ROI!!!!!!!!!!" << std::endl;
                            else
                                std::cout << "L'overlay és un BITMAP SHUTTER :D !!!!" << std::endl;

                            OFCondition status;
                            status = presentationStateHandler->activateOverlayAsBitmapShutter( overlayIndex );
                            // The overlay must not be activated on a graphic layer (i.e. getOverlayInPresentationStateActivationLayer(idx) != DVPS_IDX_NONE, otherwise this method fails.
                            //     idx  index of the overlay, must be < getNumberOfOverlaysInPresentationState().
                            std::cout << "STATUS quo: " << status.text() << std::endl;

                            bool isROI = false;
                            unsigned int width = 0, height = 0, left = 0, top = 0;
                            const void *data = 0;
                            Uint16 foreground = 0; // color amb que es pinta l'overlay
                            status = presentationStateHandler->getOverlayData(
                                    layer,
                                    overlayIndex,
                                    data,
                                    width,
                                    height,
                                    left,
                                    top,
                                    isROI,
                                    foreground
                                        );

                            std::cout << status.text() << std::endl;
                            this->setBitmapShutter( (unsigned char *)data, width, height, left, top, foreground );
                        }
                    }
                }
                else
                    DEBUG_LOG( "No hi ha BITMAP shutter al presentation state" );
            }
            else
                DEBUG_LOG( qPrintable( QString("No s'han pogut carregar les dades del presentation state al corresponenr handler: ") + status.text() ) );
        }
        else
            DEBUG_LOG( qPrintable( QString("No s'ha pogut carregar el fitxer de presentation state: ") + status.text() ) );
    }
    else
        DEBUG_LOG( "No hi ha dades d'input, no es pot aplicar cap shutter" );
}

void ShutterFilter::setRectangularShutter( double leftVertical, double rightVertical, double upperHorizontal, double lowerHorizontal )
{
    DEBUG_LOG( qPrintable( QString("Coordenades shutter RECTANGULAR: [LV,RV,UH,LH] %1,%2,%3,%4")
                .arg( leftVertical ).arg( rightVertical ).arg( upperHorizontal ).arg( lowerHorizontal ) ) );
    m_inputData->updateInformation();
    double origin[3];
    m_inputData->getOrigin( origin );

    vtkPoints *points = vtkPoints::New();
    // \TODO tenim un petit problema quan la coordenada x és 0 (potser és quan coincideix amb l'origen 0) Deu ser degut a un bug del filtre d'stencil. Per evitar això fem que si coincideix l'origen x, li fem un petit increment perquè funcioni el retallat
    if( leftVertical == origin[0] )
        leftVertical += 0.001;
    // comencem per la cantonada inferior esquerre i continuem en sentit anti-horari
    points->InsertPoint( 0, leftVertical, lowerHorizontal , origin[2]-1. );
    points->InsertPoint( 1, rightVertical, lowerHorizontal , origin[2]-1. );
    points->InsertPoint( 2, rightVertical, upperHorizontal , origin[2]-1. );
    points->InsertPoint( 3, leftVertical, upperHorizontal , origin[2]-1. );

    vtkCellArray *vertexs = vtkCellArray::New();
    vertexs->InsertNextCell( 5 );
    for( int i = 0; i < 5; i++ )
        vertexs->InsertCellPoint( i % 4 ); // 0,1,2,3,0

    vtkPolyData *polyData = vtkPolyData::New();
    polyData->SetPoints( points );
    polyData->SetLines( vertexs );

    vtkLinearExtrusionFilter *extruder = vtkLinearExtrusionFilter::New();
    extruder->SetScaleFactor(2);
    extruder->SetExtrusionTypeToNormalExtrusion();
    extruder->SetVector( 0 , 0 , 1 ); // assumirem ara per ara, només es fa en axial
    extruder->SetInput( polyData );

    m_rectangularPolyDataStencil = vtkPolyDataToImageStencil::New();
    m_rectangularPolyDataStencil->SetInput( extruder->GetOutput() );
    extruder->Delete();
}

void ShutterFilter::setPolygonalShutter( std::vector< double[2] > vertexs )
{
    //\TODO per implementar
    DEBUG_LOG("Mètode no implementat");
}

void ShutterFilter::setCircularShutter( double center[2], double radius )
{
    DEBUG_LOG( qPrintable( QString("CIRCULAR shutter, centre: %1,%2 radi: %3").arg( center[0] ).arg( center[1] ).arg( radius ) ) );
    m_inputData->updateInformation();
    double origin[3];
    double spacing[3];
    m_inputData->getOrigin( origin );
    m_inputData->getSpacing( spacing );

    vtkRegularPolygonSource *circle = vtkRegularPolygonSource::New();
    circle->SetCenter( center[0], center[1], origin[2] - 1.0 );
    circle->SetRadius( radius );
    circle->SetNumberOfSides( 35 ); // com més sides, més línies, per tant el cercle tindria més resolució.
    circle->GeneratePolygonOff();

    vtkLinearExtrusionFilter *extruder = vtkLinearExtrusionFilter::New();
    extruder->SetScaleFactor(2);
    extruder->SetExtrusionTypeToNormalExtrusion();
    extruder->SetVector( 0 , 0 , 1 ); // assumirem ara per ara, només es fa en axial
    extruder->SetInput( circle->GetOutput() );

    m_circularPolyDataStencil = vtkPolyDataToImageStencil::New();
    m_circularPolyDataStencil->SetInput( extruder->GetOutput() );
    extruder->Delete();
}

void ShutterFilter::setBitmapShutter( vtkImageData *bitmap )
{
    //\TODO per implementar
    DEBUG_LOG("Mètode no implementat");
}

void ShutterFilter::setBitmapShutter( unsigned char *data, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int foreground )
{
    // creem la màscara que farà de bitmap shutter
    // coses a tenir en compte
    // La màscara ha de tenir el mateix extent que l'input
    //      ara mateix només ho fem per una imatge per tant anem a saco
    // S'ha de tenir en compte que podem tenir més shutters a part ( poly, circle, etc ) Fer bé la combinació.

    // creem el vtkImageData a partir del bitmap del DICOM
    vtkImageData *bitmapShutter = vtkImageData::New();
    m_inputData->updateInformation();
    bitmapShutter->SetDimensions( m_inputData->getDimensions() );
    bitmapShutter->SetWholeExtent( m_inputData->getWholeExtent() );
//                 bitmapShutter->SetExtent( m_inputData->GetExtent() );
//                 bitmapShutter->SetUpdateExtent( m_inputData->GetUpdateExtent() );
    bitmapShutter->SetSpacing( m_inputData->getSpacing() );
    bitmapShutter->SetOrigin( m_inputData->getOrigin() );
//                 bitmapShutter->SetScalarType( m_inputData->GetScalarType() );
    bitmapShutter->SetScalarTypeToUnsignedChar();
    bitmapShutter->SetNumberOfScalarComponents( 1 );
    bitmapShutter->AllocateScalars();

    int wholeExtent[6];
    unsigned char *unsignedData = (unsigned char *)data;
    m_inputData->getWholeExtent( wholeExtent );
    for( int i = 0 ; i <= wholeExtent[1]; i++ )
    {
        for( int j = 0 ; j <= wholeExtent[3]; j++ )
        {
            for( int k = 0; k <= wholeExtent[5]; k++ )
            {
                int index = ((wholeExtent[1]+1)*j * + i ) + k*( (wholeExtent[1]+1) * (wholeExtent[3]+1) ); //  \TODO mirar si això està bé

                // get scalar pointer to current pixel
                unsigned char *currentVoxel = (unsigned char *) bitmapShutter->GetScalarPointer( i , j , k );

                // set scalar value accordingly
//                             *currentVoxel = (unsigned char *)data[index];
                *currentVoxel = unsignedData[index];
                std::cout << "índex: " << i << "," << "," << j << "," << k << " = " << index << ": valor: " << &currentVoxel << std::endl;
            }
        }
    }

    vtkImageMask *bitmapShutterMask = vtkImageMask::New();
    bitmapShutterMask->SetImageInput( m_inputData->getVtkData() );

//                 std::cout << "Overlays available (non-shadowed) in attached image: " << max << std::endl;
//                 for( size_t oidx = 0; oidx < max; oidx++ )
//                 {
//                     std::cout << "  Overlay #" << oidx+1 << ": group=0x" << hex << m_defaultPresentationStateHandler->getOverlayInImageGroup( oidx ) << dec << " label=\"";
//                     c = m_defaultPresentationStateHandler->getOverlayInImageLabel( oidx );
//                     if( c )
//                         std::cout << c;
//                     else
//                         std::cout << "(none)";
//                     std::cout << "\" description=\"";
//                     c = m_defaultPresentationStateHandler->getOverlayInImageDescription( oidx );
//                     if( c )
//                         std::cout << c;
//                     else
//                         std::cout << "(none)";
//                     std::cout << "\" type=";
//                     if( m_defaultPresentationStateHandler->overlayInImageIsROI( oidx ) )
//                         std::cout << "ROI";
//                     else
//                         std::cout << "graphic";
//                     std::cout << std::endl;
//                 }
//                 std::cout << std::endl;
}

void ShutterFilter::clearAllShutters()
{
    this->clearCircularShutter();
    this->clearRectangularShutter();
    this->clearPolygonalShutter();
    this->clearBitmapShutter();
}

void ShutterFilter::clearShutter( int shutterID )
{
    switch( shutterID )
    {
    case CircularShutter:
        clearCircularShutter();
    break;

    case PolygonalShutter:
        clearPolygonalShutter();
    break;

    case RectangularShutter:
        clearRectangularShutter();
    break;

    case BitmapShutter:
        clearBitmapShutter();
    break;
    }
}

void ShutterFilter::clearCircularShutter()
{
    if( m_circularPolyDataStencil )
    {
        m_circularPolyDataStencil->Delete();
        m_circularPolyDataStencil = 0;
    }
}

void ShutterFilter::clearPolygonalShutter()
{
    if( m_polygonalPolyDataStencil )
    {
        m_polygonalPolyDataStencil->Delete();
        m_polygonalPolyDataStencil = 0;
    }
}

void ShutterFilter::clearRectangularShutter()
{
    if( m_rectangularPolyDataStencil )
    {
        m_rectangularPolyDataStencil->Delete();
        m_rectangularPolyDataStencil = 0;
    }
}

void ShutterFilter::clearBitmapShutter()
{
    //\TODO per implementar
    DEBUG_LOG("Mètode no implementat");
}

vtkImageData *ShutterFilter::getOutput()
{
    // si hi ha algun shutter l'apliquem sobre la imatge
    // \TODO falta verificar que la concatenació d'stencils sigui correcta
    // \TODO fer còpia local de l'output i no cal refer el pipeline si es demana més d'un cop
    if( m_inputData )
    {
        vtkImageStencil *rectangularStencil = 0, *polygonalStencil = 0, *circularStencil = 0;
        if( m_rectangularPolyDataStencil )
        {
            rectangularStencil = vtkImageStencil::New();
            rectangularStencil->SetInput( m_inputData->getVtkData() );
            rectangularStencil->ReverseStencilOn();
            rectangularStencil->SetBackgroundValue( m_background );
            rectangularStencil->SetStencil( m_rectangularPolyDataStencil->GetOutput() );
        }

        if( m_polygonalPolyDataStencil )
        {
            polygonalStencil = vtkImageStencil::New();
            if( rectangularStencil ) // si hi ha shutter rectangular el concatenem
                polygonalStencil->SetInput( rectangularStencil->GetOutput() );
            else
                polygonalStencil->SetInput( m_inputData->getVtkData() );
            // això sembla que s'ha de fer així pel sentit en que ens donen els punts
            polygonalStencil->ReverseStencilOff();
            polygonalStencil->SetBackgroundValue( m_background );
            polygonalStencil->SetStencil( m_polygonalPolyDataStencil->GetOutput() );
        }

        if( m_circularPolyDataStencil )
        {
            circularStencil = vtkImageStencil::New();
            if( polygonalStencil ) // si hi ha shutter poligonal el concatenem
                circularStencil->SetInput( polygonalStencil->GetOutput() );
            else if( rectangularStencil ) // sinó mire si n'hi de rectamgular per concatenar
                circularStencil->SetInput( rectangularStencil->GetOutput() );
            else
                circularStencil->SetInput( m_inputData->getVtkData() );

            circularStencil->ReverseStencilOn();
            circularStencil->SetBackgroundValue( m_background );
            circularStencil->SetStencil( m_circularPolyDataStencil->GetOutput() );
        }

        // \TODO faltaria la part del bitmap shutter, encara per resoldre
        vtkImageData *output = 0;
        if( circularStencil )
            output = circularStencil->GetOutput();
        else if( polygonalStencil )
            output = polygonalStencil->GetOutput();
        else if( rectangularStencil )
            output = rectangularStencil->GetOutput();

        if( output )
            output->Update();

        return output;
    }
    else
    {
        DEBUG_LOG( "L'input és NUL. No es pot reproduir el shutter" );
        return 0;
    }
}

}
