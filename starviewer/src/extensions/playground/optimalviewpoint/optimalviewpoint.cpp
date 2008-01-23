// S'HAN D'AFEGIR ELS MIRALLS AL RENDERER ABANS QUE ELS VOLUMS (SINÓ ELS MIRALLS
// SEMPRE ES VEUEN PER SOBRE).
// SI CAL AFEGIR MIRALLS DESPRÉS D'AFEGIR ELS VOLUMS, DESPRÉS D'AFEGIR-LOS S'HAN
// DE TREURE ELS VOLUMS I TORNAR-LOS A POSAR
/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "optimalviewpoint.h"
#include "povspherecloud.h"
#include "vector3.h"
#include "transferfunctionio.h"
#include "slicer.h"
#include "optimalviewpointparameters.h"
#include "volume.h"
#include "logging.h"
#include "optimalviewpointvolume.h"
#include "optimalviewpointplane.h"
#include <cmath>
//vtk
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageActor.h>
#include <vtkVolume.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageCast.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include "vtkInteractorStyleSwitchGgg.h"
// Qt
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QTime>

namespace udg {

OptimalViewpoint::OptimalViewpoint( QObject * parent )
    : QObject( parent )
{
    m_renderer = 0;
//     m_renderer = vtkRenderer::New();    // creem el renderer principal
//     m_renderer->SetBackground( 0.0, 0.0, 0.0 );       // posem el fons gris

    m_camera = vtkCamera::New();  // creem la càmera principal
//     m_renderer->SetActiveCamera( m_camera );

    m_planes = new PlaneVector(); // creem el vector de plans
    m_planes->resize( 1, 0 );

    m_interactor = 0;   // l'interactor és null inicialment

//     m_helper = OptimalViewpointHelper::New(); // creem l'actualitzador
//     m_helper->setOptimalViewpoint( this );

    m_numberOfPlanes = 0;  // inicialment cap mirall
    m_resultsChanged = false;

    m_readExtentFromFile = false;

}

OptimalViewpoint::~OptimalViewpoint()
{
    for ( unsigned char i = 0; i < m_planes->size(); i++ )
        delete (*m_planes)[i];
/*
    renderer->Delete();
    rendererMirall1->Delete();
    rendererMirall2->Delete();
    renderWindow->Delete();
*/
    //if ( m_viewer ) delete m_viewer; // aquí o a OptimalViewpointDirector?
    if ( m_renderer ) m_renderer->Delete();


    // FALTA ESBORRAR ELS FITXERS TEMPORALS
}

vtkRenderer * OptimalViewpoint::getMainRenderer() const
{
    return m_renderer;
}

void OptimalViewpoint::setMainRenderer( vtkRenderer * mainRenderer )
{
    m_renderer = mainRenderer; m_renderer->Register( 0 );
    m_renderer->SetActiveCamera( m_camera );
//     m_renderer->SetBackground( 0.5, 0.5, 0.5 );       // posem el fons gris
}

void OptimalViewpoint::setInteractor( vtkRenderWindowInteractor * interactor )
{
    m_interactor = interactor;

    vtkInteractorStyleSwitchGgg * interactorStyle =
            vtkInteractorStyleSwitchGgg::SafeDownCast( m_interactor->GetInteractorStyle() );
    interactorStyle->SetCurrentStyleToJoystickActor();
//     interactorStyle->GetCurrentStyle()->AddObserver( vtkCommand::EndInteractionEvent, m_helper );
    interactorStyle->SetCurrentStyleToTrackballActor();
//     interactorStyle->GetCurrentStyle()->AddObserver( vtkCommand::EndInteractionEvent, m_helper );
    interactorStyle->SetCurrentStyleToJoystickCamera(); // de moment, aquest per defecte
}

void OptimalViewpoint::setImage( vtkImageData * image )
{
    // fem un casting a int perquè a vegades hi ha problemes amb l'scalar range
    vtkImageCast * imageCaster = vtkImageCast::New();
    imageCaster->SetInput( image );
    imageCaster->SetOutputScalarType( VTK_INT );
    imageCaster->Update();

//     vtkImageData * castedImage = imageCaster->GetOutput();

//     double srange[2];
//     castedImage->GetScalarRange( srange );
//     double min = srange[0];
//     double max = srange[1];
//
//     std::cout << "min = " << min << ", max = " << max << std::endl;
//
// //     double diff = max - min;
// //     double slope = 255.0 / diff;
// //     double inter = -slope * min;
// //     double shift = inter / slope;
//
//     double shift = 0.0;
//     double slope = 255.0 / max;
//
//     vtkImageShiftScale * shifter = vtkImageShiftScale::New();
//     shifter->SetInput( castedImage );
//     shifter->SetShift( shift );
//     shifter->SetScale( slope );
//     shifter->SetOutputScalarTypeToUnsignedChar();
//     shifter->ClampOverflowOn();
//     shifter->Update();

    // creem el nou volum
//     m_volume = new OptimalViewpointVolume( shifter->GetOutput() );
    m_volume = new OptimalViewpointVolume( imageCaster->GetOutput() );

    DEBUG_LOG( "emit scalarRange" );
    emit scalarRange( m_volume->getRangeMin(), m_volume->getRangeMax() );

    DEBUG_LOG( "imageCaster->Delete()" );
    imageCaster->Delete();
//     shifter->Delete();

    vtkVolume * volume = m_volume->getMainVolume();

    // ajustem la posició de les càmeres segons la mida del volum
    m_camera->SetPosition( 0.0, 0.0, volume->GetLength() * 4.0 );

    // calculem la mida òptima pels miralls
    this->m_planeSize = (unsigned short) ceil( volume->GetLength() );

    DEBUG_LOG( "last connect" );
    connect( m_volume, SIGNAL( adjustedTransferFunctionDefined(const TransferFunction&) ), SLOT( setAdjustedTransferFunction(const TransferFunction&) ) );
    DEBUG_LOG( "end setImage" );
}

void OptimalViewpoint::setSegmentationFileName( QString name )
{
    m_volume->setSegmentationFileName( name );
}

void OptimalViewpoint::setNumberOfPlanes( unsigned char numberOfPlanes )
{
    int size = m_planes->size(); // nombre de plans actual

    if ( numberOfPlanes > m_numberOfPlanes )    // s'han d'afegir plans
    {
        // primer, plans creats prèviament
//         for ( unsigned char i = 1 + m_numberOfPlanes; i < size && i <= numberOfPlanes; i++ )
//             m_renderer->AddViewProp( (*m_planes)[i]->getPlane() );

        // després, miralls nous
        if ( 1 + numberOfPlanes > size ) // s'hauran d'afegir plans
        {
            // ampliem els vectors amb nulls
            m_planes->resize( 1 + numberOfPlanes, 0 );

            for ( unsigned char i = size; i < 1 + numberOfPlanes; i++ )
            {
                (*m_planes)[i] = new OptimalViewpointPlane( i, m_planeSize );
                (*m_planes)[i]->getRenderer()->AddViewProp( m_volume->getPlaneVolume() );
                (*m_planes)[i]->setDistance( m_volume->getMainVolume()->GetLength() );
//                 m_renderer->AddViewProp( (*m_planes)[i]->getPlane() );
                (*m_planes)[i]->setEntropyL( m_parameters->getVisualizationBlockLength() );
                (*m_planes)[i]->setEntropyN( m_numberOfClusters );
                connect( (*m_planes)[i], SIGNAL( excessEntropyComputed(double) ), SLOT( newResults() ) );
                connect( (*m_planes)[i], SIGNAL( goingToRecompute() ), m_volume, SLOT( setComputing() ) );

                (*m_planes)[i]->setVolume( m_volume );
            }

//             m_renderer->RemoveViewProp( m_volume->getMainVolume() );
//             m_renderer->AddViewProp( m_volume->getMainVolume() );
        }
    }
    else    // s'hauran de treure miralls (o no)
    {
        for ( int i = size - 1; i >= 1 + numberOfPlanes; i-- )
        {
//             m_renderer->RemoveViewProp( (*m_planes)[i]->getPlane() );
            (*m_planes)[i]->hide();
        }
    }

    m_renderer->RemoveViewProp( m_volume->getMainVolume() );
    m_renderer->AddViewProp( m_volume->getMainVolume() );

    m_numberOfPlanes = numberOfPlanes;

    ////////////////////////////////////////////////////////////////////////////
    //////////////////// PROVA DE POSICIONAMENT DELS PLANS /////////////////////
    ////////////////////////////////////////////////////////////////////////////

    switch ( m_numberOfPlanes )
    {
        case 3:
            for ( unsigned char i = 1; i <= 3; i++ )
                (*m_planes)[i]->setDistance( m_volume->getMainVolume()->GetLength() );
            (*m_planes)[1]->setLatitude( 0.0 ); (*m_planes)[1]->setLongitude( 180.0 );
            (*m_planes)[2]->setLatitude( 0.0 ); (*m_planes)[2]->setLongitude( 90.0 );
            (*m_planes)[3]->setLatitude( -90.0 ); (*m_planes)[3]->setLongitude( 0.0 );
            break;

        case 4:
            for ( unsigned char i = 1; i <= 4; i++ )
                (*m_planes)[i]->setDistance( m_volume->getMainVolume()->GetLength() );
            (*m_planes)[1]->setLatitude( 0.0 ); (*m_planes)[1]->setLongitude( 0.0 );
            (*m_planes)[2]->setLatitude( 60.0 ); (*m_planes)[2]->setLongitude( 180.0 );
            (*m_planes)[3]->setLatitude( -30.0 ); (*m_planes)[3]->setLongitude( -120.0 );
            (*m_planes)[4]->setLatitude( -30.0 ); (*m_planes)[4]->setLongitude( 120.0 );
            break;

        case 6:
            for ( unsigned char i = 1; i <= 6; i++ )
                (*m_planes)[i]->setDistance( m_volume->getMainVolume()->GetLength() );
            (*m_planes)[1]->setLatitude( 0.0 ); (*m_planes)[1]->setLongitude( 0.0 );
            (*m_planes)[2]->setLatitude( 90.0 ); (*m_planes)[2]->setLongitude( 0.0 );
            (*m_planes)[3]->setLatitude( -90.0 ); (*m_planes)[3]->setLongitude( 0.0 );
            (*m_planes)[4]->setLatitude( 0.0 ); (*m_planes)[4]->setLongitude( -90.0 );
            (*m_planes)[5]->setLatitude( 0.0 ); (*m_planes)[5]->setLongitude( 90.0 );
            (*m_planes)[6]->setLatitude( 0.0 ); (*m_planes)[6]->setLongitude( 180.0 );
            break;

        case 8:
            for ( unsigned char i = 1; i <= 8; i++ )
                (*m_planes)[i]->setDistance( m_volume->getMainVolume()->GetLength() );
            (*m_planes)[1]->setLatitude( 45.0 ); (*m_planes)[1]->setLongitude( -45.0 );
            (*m_planes)[2]->setLatitude( 45.0 ); (*m_planes)[2]->setLongitude( 45.0 );
            (*m_planes)[3]->setLatitude( -45.0 ); (*m_planes)[3]->setLongitude( -45.0 );
            (*m_planes)[4]->setLatitude( -45.0 ); (*m_planes)[4]->setLongitude( 45.0 );
            (*m_planes)[5]->setLatitude( 45.0 ); (*m_planes)[5]->setLongitude( -135.0 );
            (*m_planes)[6]->setLatitude( 45.0 ); (*m_planes)[6]->setLongitude( 135.0 );
            (*m_planes)[7]->setLatitude( -45.0 ); (*m_planes)[7]->setLongitude( -135.0 );
            (*m_planes)[8]->setLatitude( -45.0 ); (*m_planes)[8]->setLongitude( 135.0 );
            break;

        case 12:
            {
                for ( unsigned char i = 1; i <= 12; i++ )
                    (*m_planes)[i]->setDistance( 2.0 * m_volume->getMainVolume()->GetLength() );
                POVSphereCloud cloud( 1.0, 0 );
                cloud.createPOVCloud();
                const QVector< Vector3 > & geographicVertices = cloud.getGeographicVertices();
                if ( geographicVertices.size() != 12 )
                    QMessageBox::warning( 0, "No hi ha 12 punts!", QString::number( geographicVertices.size() ) );
                QVector< Vector3 >::const_iterator it;
                unsigned char i;

                for ( it = geographicVertices.begin(), i = 1; i <= 12; it++, i++ )
                {
                    (*m_planes)[i]->setLatitude( it->y ); (*m_planes)[i]->setLongitude( it->z );
                }
            }
            break;

        case 20:
            {
                for ( unsigned char i = 1; i <= 20; i++ )
                    (*m_planes)[i]->setDistance( 2.0 * m_volume->getMainVolume()->GetLength() );
                double b = ( sqrt( 5.0 ) + 1.0 ) / 2.0;
                double psi = acos( sqrt( b * b * b / ( 3.0 * sqrt( 5.0 ) ) ) ); // colatitud (rad)
                double phi = psi + 2 * acos( b / sqrt( 3.0 ) );                 // colatitud (rad)
                double phi1 = M_PIl - phi;                                      // colatitud (rad)
                double psi1 = M_PIl - psi;                                      // colatitud (rad)
                double sigma = M_PIl / 5.0;                                     // longitud (rad)
                // conversió a latitud
                psi = M_PI_2l - psi;
                phi = M_PI_2l - phi;
                phi1 = M_PI_2l - phi1;
                psi1 = M_PI_2l - psi1;
                // conversió a graus
                psi *= 180.0 * M_1_PIl;
                phi *= 180.0 * M_1_PIl;
                phi1 *= 180.0 * M_1_PIl;
                psi1 *= 180.0 * M_1_PIl;
                sigma *= 180.0 * M_1_PIl;
                /*(*m_planes)[1]->setLatitude( psi ); (*m_planes)[1]->setLongitude( 0.0 );
                (*m_planes)[2]->setLatitude( psi ); (*m_planes)[2]->setLongitude( 2.0 * sigma );
                (*m_planes)[3]->setLatitude( psi ); (*m_planes)[3]->setLongitude( 4.0 * sigma );
                (*m_planes)[4]->setLatitude( psi ); (*m_planes)[4]->setLongitude( 6.0 * sigma );
                (*m_planes)[5]->setLatitude( psi ); (*m_planes)[5]->setLongitude( 8.0 * sigma );
                (*m_planes)[6]->setLatitude( phi ); (*m_planes)[6]->setLongitude( 0.0 );
                (*m_planes)[7]->setLatitude( phi ); (*m_planes)[7]->setLongitude( 2.0 * sigma );
                (*m_planes)[8]->setLatitude( phi ); (*m_planes)[8]->setLongitude( 4.0 * sigma );
                (*m_planes)[9]->setLatitude( phi ); (*m_planes)[9]->setLongitude( 6.0 * sigma );
                (*m_planes)[10]->setLatitude( phi ); (*m_planes)[10]->setLongitude( 8.0 * sigma );
                (*m_planes)[11]->setLatitude( phi1 ); (*m_planes)[11]->setLongitude( sigma );
                (*m_planes)[12]->setLatitude( phi1 ); (*m_planes)[12]->setLongitude( 3.0 * sigma );
                (*m_planes)[13]->setLatitude( phi1 ); (*m_planes)[13]->setLongitude( 5.0 * sigma );
                (*m_planes)[14]->setLatitude( phi1 ); (*m_planes)[14]->setLongitude( 7.0 * sigma );
                (*m_planes)[15]->setLatitude( phi1 ); (*m_planes)[15]->setLongitude( 9.0 * sigma );
                (*m_planes)[16]->setLatitude( psi1 ); (*m_planes)[16]->setLongitude( sigma );
                (*m_planes)[17]->setLatitude( psi1 ); (*m_planes)[17]->setLongitude( 3.0 * sigma );
                (*m_planes)[18]->setLatitude( psi1 ); (*m_planes)[18]->setLongitude( 5.0 * sigma );
                (*m_planes)[19]->setLatitude( psi1 ); (*m_planes)[19]->setLongitude( 7.0 * sigma );
                (*m_planes)[20]->setLatitude( psi1 ); (*m_planes)[20]->setLongitude( 9.0 * sigma );*/
                double d;
                d = 0.0;
                for ( unsigned char i = 1; i <= 5; i++, d += 2.0 )
                {
                    (*m_planes)[i]->setLatitude( psi ); (*m_planes)[i]->setLongitude( d * sigma );
                }
                d = 0.0;
                for ( unsigned char i = 6; i <= 10; i++, d += 2.0 )
                {
                    (*m_planes)[i]->setLatitude( phi ); (*m_planes)[i]->setLongitude( d * sigma );
                }
                d = 1.0;
                for ( unsigned char i = 11; i <= 15; i++, d += 2.0 )
                {
                    (*m_planes)[i]->setLatitude( phi1 ); (*m_planes)[i]->setLongitude( d * sigma );
                }
                d = 1.0;
                for ( unsigned char i = 16; i <= 20; i++, d += 2.0 )
                {
                    (*m_planes)[i]->setLatitude( psi1 ); (*m_planes)[i]->setLongitude( d * sigma );
                }
            }
            break;

        case 42:
            {
                for ( unsigned char i = 1; i <= 42; i++ )
                    (*m_planes)[i]->setDistance( 2.0 * m_volume->getMainVolume()->GetLength() );
                POVSphereCloud cloud( 1.0, 1 );
                cloud.createPOVCloud();
                const QVector< Vector3 > & geographicVertices = cloud.getGeographicVertices();
                if ( geographicVertices.size() != 42 )
                    QMessageBox::warning( 0, "No hi ha 42 punts!", QString::number( geographicVertices.size() ) );
                QVector< Vector3 >::const_iterator it;
                unsigned char i;
                const QVector<Vector3> & vertices = cloud.getVertices();

                for ( it = geographicVertices.begin(), i = 1; i <= 42; it++, i++ )
                {
                    (*m_planes)[i]->setLatitude( it->y ); (*m_planes)[i]->setLongitude( it->z );
                    const Vector3 & v = vertices[i-1];
                    DEBUG_LOG( QString( "%1: (%2, %3, %4)" ).arg( i ).arg( v.x ).arg( v.y ).arg( v.z ) );
                    DEBUG_LOG( QString( "%1: (%2, %3, %4)" ).arg( i ).arg( it->x ).arg( it->y ).arg( it->z ) );
                }
            }
            break;

        case 162:
            {
                for ( unsigned char i = 1; i <= 162; i++ )
                    (*m_planes)[i]->setDistance( 4.0 * m_volume->getMainVolume()->GetLength() );
                POVSphereCloud cloud( 1.0, 2 );
                cloud.createPOVCloud();
                const QVector< Vector3 > & geographicVertices = cloud.getGeographicVertices();
                if ( geographicVertices.size() != 162 )
                    QMessageBox::warning( 0, "No hi ha 162 punts!", QString::number( geographicVertices.size() ) );
                QVector< Vector3 >::const_iterator it;
                unsigned char i;

                for ( it = geographicVertices.begin(), i = 1; i <= 162; it++, i++ )
                {
                    (*m_planes)[i]->setLatitude( it->y ); (*m_planes)[i]->setLongitude( it->z );
                }
            }
            break;
    }

    ////////////////////////////////////////////////////////////////////////////
    //////////////////// PROVA DE POSICIONAMENT DELS PLANS /////////////////////
    ////////////////////////////////////////////////////////////////////////////

//     POVSphereCloud cloud( 1.0, 4 );
//     cloud.createPOVCloud();
//     const std::vector< Vector3 > & vertices = cloud.getVertices();
//     const std::vector< Vector3 > & geographicVertices = cloud.getGeographicVertices();
//     std::cout << vertices.size() << " vertices:" << std::endl;
//     for ( unsigned short i = 0; i < vertices.size(); i++ )
//         std::cout << vertices[i] << ", length = " << vertices[i].length() << ", geo = " << geographicVertices[i] << std::endl;


}

void OptimalViewpoint::setShade( bool shade )
{
    m_volume->setShade( shade );
}

void OptimalViewpoint::setImageSampleDistance( double imageSampleDistance )
{
    m_volume->setImageSampleDistance( imageSampleDistance );
}

void OptimalViewpoint::setSampleDistance( double sampleDistance )
{
    m_volume->setSampleDistance( sampleDistance );
}

void OptimalViewpoint::setBlockLength( unsigned char blockLength )
{
    for ( unsigned char i = 1; i <= m_numberOfPlanes; i++ )
    {
        (*m_planes)[i]->setEntropyL( blockLength );
    }
}

void OptimalViewpoint::setTransferFunction( const TransferFunction & transferFunction )
{
    //typedef QPair<qreal, QColor> QGradientStop;
    //typedef QVector<QGradientStop> QGradientStops;
    //typedef QGradientStops TransferFunction;

//     vtkPiecewiseFunction * opacityTransferFunction = vtkPiecewiseFunction::New();
//     vtkColorTransferFunction * colorTransferFunction = vtkColorTransferFunction::New();
//
//     for ( unsigned char k = 0; k < transferFunction.count(); k++ )
//     {
//         opacityTransferFunction->AddPoint(
//                 transferFunction[k].first * 255.0,
//                 transferFunction[k].second.alpha() / 255.0 );
//         colorTransferFunction->AddRGBPoint(
//                 transferFunction[k].first * 255.0,
//                 transferFunction[k].second.red() / 255.0,
//                 transferFunction[k].second.green() / 255.0,
//                 transferFunction[k].second.blue() / 255.0 );
//     }
//
//     m_volume->setOpacityTransferFunction( opacityTransferFunction );
//     m_volume->setColorTransferFunction( colorTransferFunction );
//     m_volume->setOpacityTransferFunction( transferFunction.getOpacityTransferFunction() );
//     m_volume->setColorTransferFunction( transferFunction.getColorTransferFunction() );
//     TransferFunctionIO::toFile( "/scratch/tf0.tf", transferFunction );
    m_volume->setTransferFunction( transferFunction );
}

void OptimalViewpoint::updatePlanes()
{
    m_volume->synchronize();

    switch ( m_updatePlane )
    {
        case -1:    // All
        {
            QTime t;
            t.start();
            for ( unsigned char i = 1; i <= m_numberOfPlanes; i++ )
            {
                QObject::connect( m_volume, SIGNAL( visited(int,unsigned char) ),
                                  (*m_planes)[i], SLOT( compute(int,unsigned char) ),
                                  Qt::DirectConnection );
                QObject::connect( m_volume, SIGNAL( rayEnd(int) ),
                                  (*m_planes)[i], SLOT( endLBlock(int) ),
                                  Qt::DirectConnection );
                if ( m_compute ) (*m_planes)[i]->updateAndRecompute();
                else (*m_planes)[i]->update();
                QObject::disconnect( m_volume, SIGNAL( visited(int,unsigned char) ),
                                  (*m_planes)[i], SLOT( compute(int,unsigned char) ) );
                QObject::disconnect( m_volume, SIGNAL( rayEnd(int) ),
                                  (*m_planes)[i], SLOT( endLBlock(int) ) );
            }
            int elapsed = t.elapsed();
            DEBUG_LOG( QString( "Time elapsed: %1 s" ).arg( elapsed / 1000.0 ) );
            INFO_LOG( QString( "Time elapsed: %1 s" ).arg( elapsed / 1000.0 ) );
        }
            break;

        case 0:     // None
            break;

        default:
            QObject::connect( m_volume, SIGNAL( visited(int,unsigned char) ),
                              (*m_planes)[m_updatePlane], SLOT( compute(int,unsigned char) ),
                              Qt::DirectConnection );
            QObject::connect( m_volume, SIGNAL( rayEnd(int) ),
                              (*m_planes)[m_updatePlane], SLOT( endLBlock(int) ),
                              Qt::DirectConnection );
            if ( m_compute ) (*m_planes)[m_updatePlane]->updateAndRecompute();
            else (*m_planes)[m_updatePlane]->update();
            QObject::disconnect( m_volume, SIGNAL( visited(int,unsigned char) ),
                              (*m_planes)[m_updatePlane], SLOT( compute(int,unsigned char) ) );
            QObject::disconnect( m_volume, SIGNAL( rayEnd(int) ),
                              (*m_planes)[m_updatePlane], SLOT( endLBlock(int) ) );




            if ( m_parameters->getGroupingMethod() > 0 )
            {
                
                // slicer
                Slicer slicer( m_updatePlane );
    //             slicer.setInput( m_volume->getLabeledImage() );
                slicer.setInput( m_volume->getImage() );




                ///\warning Això només funcionarà si són 42 plans!!!
                POVSphereCloud cloud( 1.0, 1 );
                cloud.createPOVCloud();
                const QVector< Vector3 > & vertices = cloud.getVertices();
                slicer.setVector( vertices[m_updatePlane - 1] );





                slicer.setMatrix( (*m_planes)[m_updatePlane]->getTransformMatrix() );
                slicer.setSpacing( m_volume->getImageSampleDistance(), m_volume->getImageSampleDistance(), m_volume->getSampleDistance() );
                slicer.setReadExtentFromFile( m_readExtentFromFile );
                slicer.reslice();
    //             slicer.computeSmi();
    //             slicer.method1A( m_similarityThreshold );
    //             slicer.method1B( m_similarityThreshold );
                QTime t;
                t.start();
                switch ( m_parameters->getGroupingMethod() )
                {
                    case 1: slicer.groupingMethodC( m_similarityThreshold ); break;
                    case 2: slicer.groupingMethodC_JS( m_similarityThreshold ); break;
                    case 3: slicer.splittingMethodC( m_similarityThreshold ); break;
                    case 4: slicer.splittingMethodC_JS( m_similarityThreshold ); break;
                }
                int elapsed = t.elapsed();
                DEBUG_LOG( QString( "Time elapsed: %1 s" ).arg( elapsed / 1000.0 ) );
                INFO_LOG( QString( "Time elapsed: %1 s" ).arg( elapsed / 1000.0 ) );
            }

            break;
    }

    m_volume->setComputing( false );
}

void OptimalViewpoint::renderPlanes( short plane )
{
    m_volume->synchronize();

    switch ( plane )
    {
        case -1:    // All
            for ( unsigned char i = 1; i <= m_numberOfPlanes; i++ )
            {
                (*m_planes)[i]->update( true );
            }
            break;

        case 0:     // None
            break;

        default:
            (*m_planes)[plane]->update( true );
            break;
    }
}

bool OptimalViewpoint::resultsChanged() const
{
    return m_resultsChanged;
}

std::vector<double> * OptimalViewpoint::getResults()
{
    return getExcessEntropyResults();
}

std::vector<double> * OptimalViewpoint::getEntropyRateResults()
{
    m_resultsChanged = false;

    std::vector<double> * results = new std::vector<double>( 1 + m_numberOfPlanes );

    for ( unsigned char i = 1; i <= m_numberOfPlanes; i++ )
    {
        (*results)[i] = (*m_planes)[i]->getEntropyRate();
    }

    return results;
}

std::vector<double> * OptimalViewpoint::getExcessEntropyResults()
{
    m_resultsChanged = false;

    std::vector<double> * results = new std::vector<double>( 1 + m_numberOfPlanes );

    for ( unsigned char i = 1; i <= m_numberOfPlanes; i++ )
    {
        (*results)[i] = (*m_planes)[i]->getExcessEntropy();
    }

    return results;
}

const TransferFunction & OptimalViewpoint::getAdjustedTransferFunction() const
{
    std::cout << "OV::gatf: own adjusted transfer function:" << std::endl;
//     for ( int i = 0; i < m_adjustedTransferFunction.size(); i++ )
//     {
//         std::cout << m_adjustedTransferFunction[i].first << ": " << qPrintable( m_adjustedTransferFunction[i].second.name() ) << std::endl;
//     }
    m_adjustedTransferFunction.print();
    std::cout << "----------------------" << std::endl;

    return m_adjustedTransferFunction;
}

void OptimalViewpoint::newResults()
{
    m_resultsChanged = true;
}

void OptimalViewpoint::setAdjustedTransferFunction( const TransferFunction & adjustedTransferFunction )
{
    m_adjustedTransferFunction = adjustedTransferFunction;
    std::cout << "OV::satf(): atf:" << std::endl;
    m_adjustedTransferFunction.print();
    std::cout << "----------------------" << std::endl;
}

// nous paràmetres
void OptimalViewpoint::setOpacityForComputing( bool on )
{
    m_volume->setOpacityForComputing( on );
}

void OptimalViewpoint::setInterpolation( int interpolation )
{
    m_volume->setInterpolation( interpolation );
}

void OptimalViewpoint::setSpecular( bool specular )
{
    m_volume->setSpecular( specular );
}

void OptimalViewpoint::setSpecularPower( double specularPower )
{
    m_volume->setSpecularPower( specularPower );
}

void OptimalViewpoint::setUpdatePlane( short updatePlane )
{
    m_updatePlane = updatePlane;
}

void OptimalViewpoint::setCompute( bool compute )
{
    m_compute = compute;
}

bool OptimalViewpoint::doLoadSegmentation( const QString & fileName )
{
    m_numberOfClusters = m_volume->loadSegmentationFromFile( fileName );
    if ( m_numberOfClusters > 0 )
    {
        m_parameters->setAdjustedTransferFunction( m_adjustedTransferFunction );
//         m_parameters->setNumberOfClusters( m_numberOfClusters );
    }
    return m_numberOfClusters > 0;
}

void OptimalViewpoint::doAutomaticSegmentation(
                                                unsigned short iterations,
                                                unsigned char blockLength,
                                                unsigned char numberOfClusters,
                                                double noise,
                                                double imageSampleDistance,
                                                double sampleDistance
                                              )
{
    m_volume->setImageSampleDistance( imageSampleDistance );
    m_volume->setSampleDistance( sampleDistance );

    OptimalViewpointPlane * plane = new OptimalViewpointPlane( 0, m_planeSize );
    plane->getRenderer()->AddViewProp( m_volume->getPlaneVolume() );
    plane->setDistance( m_volume->getMainVolume()->GetLength() );
    plane->setEntropyL( blockLength );
    plane->setEntropyN( numberOfClusters );
    plane->setVolume( m_volume );

    QObject::connect( m_volume, SIGNAL( needsExcessEntropy() ),
                      plane, SLOT( updateAndRecompute() ) );
    QObject::connect( m_volume, SIGNAL( visited(int,unsigned char) ),
                      plane, SLOT( compute(int,unsigned char) ),
                      Qt::DirectConnection );
    QObject::connect( m_volume, SIGNAL( rayEnd(int) ),
                      plane, SLOT( endLBlock(int) ),
                      Qt::DirectConnection );
    QObject::connect( plane, SIGNAL( excessEntropyComputed(double) ),
                      m_volume, SLOT( setExcessEntropy(double) ) );

    m_volume->setComputing( true );
    m_numberOfClusters = m_volume->segmentateVolume( iterations,
                                                     numberOfClusters,
                                                     noise );
    m_volume->setComputing( false );

    QObject::disconnect( m_volume, SIGNAL( needsExcessEntropy() ),
                      plane, SLOT( updateAndRecompute() ) );
    QObject::disconnect( m_volume, SIGNAL( visited(int,unsigned char) ),
                      plane, SLOT( compute(int,unsigned char) ) );
    QObject::disconnect( m_volume, SIGNAL( rayEnd(int) ),
                      plane, SLOT( endLBlock(int) ) );
    QObject::disconnect( plane, SIGNAL( excessEntropyComputed(double) ),
                      m_volume, SLOT( setExcessEntropy(double) ) );

    delete plane;

    m_parameters->setAdjustedTransferFunction( m_adjustedTransferFunction );
//     m_parameters->setNumberOfClusters( m_numberOfClusters );
}

void OptimalViewpoint::setSimilarityThreshold( double similarityThreshold )
{
    m_similarityThreshold = similarityThreshold;
}

void OptimalViewpoint::setRenderCluster( bool renderCluster )
{
    m_volume->setRenderCluster( renderCluster );
}

void OptimalViewpoint::setClusterLimits( unsigned short first, unsigned short last )
{
    m_volume->setClusterLimits( first, last );
}

void OptimalViewpoint::setReadExtentFromFile( bool readExtentFromFile )
{
    m_readExtentFromFile = readExtentFromFile;
}

void OptimalViewpoint::setParameters( OptimalViewpointParameters * parameters )
{
    m_parameters = parameters;
    connect( m_parameters, SIGNAL( changed(int) ), SLOT( readParameter(int) ) );
}

void OptimalViewpoint::readParameter( int parameter )
{
    DEBUG_LOG( "readParameter" );
    switch ( parameter )
    {
        case OptimalViewpointParameters::NumberOfPlanes:
            setNumberOfPlanes( m_parameters->getNumberOfPlanes() );
            break;
        case OptimalViewpointParameters::Shade:
            setShade( m_parameters->getShade() );
            break;
        case OptimalViewpointParameters::VisualizationImageSampleDistance:
            setImageSampleDistance( m_parameters->getVisualizationImageSampleDistance() );
            break;
        case OptimalViewpointParameters::VisualizationSampleDistance:
            setSampleDistance( m_parameters->getVisualizationSampleDistance() );
            break;
        case OptimalViewpointParameters::VisualizationBlockLength:
            setBlockLength( m_parameters->getVisualizationBlockLength() );
            break;
        case OptimalViewpointParameters::TransferFunctionObject:
            setTransferFunction( m_parameters->getTransferFunctionObject() );
            break;
        case OptimalViewpointParameters::Interpolation:
            setInterpolation( m_parameters->getInterpolation() );
            break;
    }
}

void OptimalViewpoint::newMethod2( int step, bool normalized )
{
    m_volume->synchronize();

    double result;

    switch ( m_updatePlane )
    {
        case -1:    // All
        {
            QVector< double > results;
            for ( unsigned char i = 1; i <= m_numberOfPlanes; i++ )
            {
                Slicer slicer( i );
//             slicer.setInput( m_volume->getLabeledImage() );
                slicer.setInput( m_volume->getImage() );
                slicer.setMatrix( (*m_planes)[i]->getTransformMatrix() );
                slicer.setSpacing( m_volume->getImageSampleDistance(), m_volume->getImageSampleDistance(), m_volume->getSampleDistance() );
                slicer.setReadExtentFromFile( m_readExtentFromFile );
                slicer.reslice( false );
                result = slicer.newMethod2( step, normalized );
                DEBUG_LOG( QString( "[OV] (%1) newMethod2(%2, %3) = %4" ).arg( i ).arg( step ).arg( normalized ).arg( result ) );
                results << result;
            }

            // Printar resultats i guardar-los en un fitxer
            QFile outFile( QDir::tempPath().append( QString( "/nm2_%1%2.txt" ).arg( step ).arg( normalized ? "_norm" : "" ) ) );
            if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QTextStream out( &outFile );
                out << "step = " << step << "\n";
                for ( int i = 0; i < results.size(); i++ )
                {
                    out << "plane " << i + 1 << " : " << results[i] << "\n";
                }
                outFile.close();
            }
        }
            break;

        case 0:     // None
            break;

        default:
            Slicer slicer( m_updatePlane );
//             slicer.setInput( m_volume->getLabeledImage() );
            slicer.setInput( m_volume->getImage() );
            slicer.setMatrix( (*m_planes)[m_updatePlane]->getTransformMatrix() );
            slicer.setSpacing( m_volume->getImageSampleDistance(), m_volume->getImageSampleDistance(), m_volume->getSampleDistance() );
            slicer.setReadExtentFromFile( m_readExtentFromFile );
            slicer.reslice( false );
            result = slicer.newMethod2( step, normalized );
            DEBUG_LOG( QString( "[OV] (%1) newMethod2(%2, %3) = %4" ).arg( m_updatePlane ).arg( step ).arg( normalized ).arg( result ) );
            break;
    }
}

void OptimalViewpoint::doRegularSegmentation( unsigned char numberOfBins )
{
    m_numberOfClusters = m_volume->rescale( numberOfBins );
    if ( m_numberOfClusters > 0 )
    {
        m_parameters->setAdjustedTransferFunction( m_adjustedTransferFunction );
//         m_parameters->setNumberOfClusters( m_numberOfClusters );
    }
}


void OptimalViewpoint::computeObscurances()
{
    m_volume->computeObscurances();
}


}; // end namespace udg
