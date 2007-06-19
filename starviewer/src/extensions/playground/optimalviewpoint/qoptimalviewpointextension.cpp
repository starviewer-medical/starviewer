/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#include "qoptimalviewpointextension.h"

#include <iostream>

#include "optimalviewpoint.h"
// #include "optimalviewpointdirector.h"
#include "optimalviewpointparameters.h"
// #include "optimalviewpointinputparametersform.h"
#include "optimalviewpointviewer.h"


#include <QVTKWidget.h> // perquè funcioni m_method->setInteractor( m_viewer->getInteractor() );
#include "volume.h"
#include <QMessageBox>



namespace udg {



QOptimalViewpointExtension::QOptimalViewpointExtension( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );


    // Creem el director i els paràmetres
//     m_optimalViewpointDirector = new OptimalViewpointDirector( m_viewerWidget, this );
    m_parameters = new OptimalViewpointParameters( this );

    // Li assignem al director quins són els seus paràmetres
//     m_optimalViewpointDirector->setParameters( m_optimalViewpointParameters );

    // Creem els widgets que aniran al toolbox
//     QHBoxLayout * controlLayout = new QHBoxLayout( m_controlWidget );
//     m_optimalViewpointInputParametersForm = new OptimalViewpointInputParametersForm( m_controlWidget );
//     controlLayout->addWidget( m_optimalViewpointInputParametersForm );
//     controlLayout->setMargin( 0 );
//     m_optimalViewpointInputParametersForm->setName( "Optimal Viewpoint Page" );
    // li assignem els paràmetres
//     m_optimalViewpointInputParametersForm->setParameters( m_parameters );
    m_inputParametersWidget->setParameters( m_parameters );

    QHBoxLayout * viewerLayout = new QHBoxLayout( m_viewerWidget );
    viewerLayout->setMargin( 0 );

    // connectem l'acció amb el director
//     m_optimalViewpointAction = new Q3Action( this );
//     m_optimalViewpointAction->setMenuText( tr("&Optimal Viewpoint") );


    m_method = 0;
    m_viewer = 0;


//     connect( m_optimalViewpointAction, SIGNAL( activated() ), m_optimalViewpointDirector, SLOT( execute() ) );
//     connect( m_optimalViewpointInputParametersForm, SIGNAL( executionRequested() ), SLOT( execute() ) );
    connect( m_inputParametersWidget, SIGNAL( executionRequested() ), SLOT( execute() ) );
    // connectem els paràmetres amb les interfícies
    // quan un paràmetre s'actualitzi, s'actualitzarà a tots els widgets que el tinguin com a input
//     connect( m_parameters, SIGNAL( changed(int) ), m_optimalViewpointInputParametersForm, SLOT( readParameter(int) ) );
    connect( m_parameters, SIGNAL( changed(int) ), m_inputParametersWidget, SLOT( readParameter(int) ) );


    connect( m_inputParametersWidget, SIGNAL( loadSegmentationRequested() ), SLOT( loadSegmentation() ) );
    connect( m_inputParametersWidget, SIGNAL( automaticSegmentationRequested() ), SLOT( automaticSegmentation() ) );
}



QOptimalViewpointExtension::~QOptimalViewpointExtension()
{
    std::cout << "destructor QOptimalViewpointExtension" << std::endl;
    delete m_method;
}



void QOptimalViewpointExtension::setInput( Volume * input )
{
    m_parameters->setVolumeObject( input );
//     m_optimalViewpointInputParametersForm->readParameter( OptimalViewpointParameters::VolumeObject );
}



void QOptimalViewpointExtension::execute()
{
    if ( !m_parameters )
    {
        std::cerr << "OptimalViewpointDirector::execute(): No hi ha paràmetres establerts" << std::endl;
    }
    else
    {
        Volume * volume = m_parameters->getVolumeObject();

        if ( volume )
        {
//             if ( !m_method )    // ja no hi hauria d'entrar
//             {
//                 m_method = new OptimalViewpoint();
//                 m_viewer = new OptimalViewpointViewer( m_viewerWidget );
//                 m_viewerWidget->layout()->addWidget( m_viewer );
//                 m_viewer->setRenderer( m_method->getMainRenderer() );
//                 m_method->setInteractor( m_viewer->getInteractor() );
// //                 m_applicationWindow->addWorkingAreaWidget( m_viewer, tr("Optimal Viewpoint") );
// //                 VolumeRepository * repository = VolumeRepository::getRepository();
//                 m_method->setImage( volume->getVtkData() );
// //                 m_method->setImageFileName( repository->getVolume( volumeId )->getFileName() );
//                 m_method->setSegmentationFileName( m_parameters->getSegmentationFileName() );
// 
//                 // segmentació del model
//                 unsigned char n = m_method->segmentateImage(
//                         m_parameters->getSegmentationIterations(),
//                         m_parameters->getSegmentationBlockLength(),
//                         m_parameters->getSegmentationNumberOfClusters(),
//                         m_parameters->getSegmentationNoise(),
//                         m_parameters->getSegmentationImageSampleDistance(),
//                         m_parameters->getSegmentationSampleDistance() );
// 
//                 // funció de transferència ajustada
//                 m_parameters->setAdjustedTransferFunction( m_method->getAdjustedTransferFunction() );
//                 m_parameters->setNumberOfClusters( n );
//                 std::cout << "number of clusters: " << (short) n << std::endl;
//             }

            if ( !m_viewer )
            {
                m_viewer = new OptimalViewpointViewer( m_viewerWidget );
                m_viewerWidget->layout()->addWidget( m_viewer );
                m_viewer->setRenderer( m_method->getMainRenderer() );
//                 m_method->setInteractor( m_viewer->getInteractor() );
            }

            m_method->setNumberOfPlanes( m_parameters->getNumberOfPlanes() );
            std::cout << "OVD: set number of planes" << std::endl;
            m_method->setShade( m_parameters->getShade() );
            std::cout << "OVD: set shade" << std::endl;
            m_method->setImageSampleDistance( m_parameters->getVisualizationImageSampleDistance() );
            std::cout << "OVD: set image sample distance" << std::endl;
            m_method->setSampleDistance( m_parameters->getVisualizationSampleDistance() );
            std::cout << "OVD: set sample distance" << std::endl;
            m_method->setBlockLength( m_parameters->getVisualizationBlockLength() );
            std::cout << "OVD: set block length" << std::endl;
            m_method->setTransferFunction( m_parameters->getTransferFunctionObject() );
            std::cout << "OVD: set transfer function" << std::endl;



            // nous paràmetres

            m_method->setOpacityForComputing( m_parameters->getComputeWithOpacity() );
            m_method->setInterpolation( m_parameters->getInterpolation() );
            m_method->setSpecular( m_parameters->getSpecular() );
            m_method->setSpecularPower( m_parameters->getSpecularPower() );
            m_method->setUpdatePlane( m_parameters->getUpdatePlane() );
            m_method->setCompute( m_parameters->getCompute() );
            m_method->setSimilarityThreshold( m_parameters->getSimilarityThreshold() );



            m_method->updatePlanes();
            std::cout << "OVD: update planes" << std::endl;
            m_viewer->render();

            if ( m_method->resultsChanged() )
            {
                std::vector<double> * entropyRateResults = m_method->getEntropyRateResults();
                std::vector<double> * excessEntropyResults = m_method->getExcessEntropyResults();

                QMessageBox * resultsDialog = new QMessageBox( tr("Results"), "", QMessageBox::Information,
                        QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, m_viewerWidget );
                resultsDialog->setModal( false );
                resultsDialog->setAttribute( Qt::WA_DeleteOnClose );

                QString text = "<table cellspacing=\"8\"><tr><td></td><td align=\"center\"><b><i><u>entropy rate</u></i></b></td><td align=\"center\"><b><i><u>excess entropy</u></i></b></td></tr>";
                QString planeString = tr("Plane");
                for ( unsigned char i = 1; i <= m_parameters->getNumberOfPlanes(); i++ )
                    text += "<tr><td><b>" + planeString + QString( " %1:</b></td><td align=\"center\">%2</td><td align=\"center\">%3</td></tr>" ).arg( i ).arg( (*entropyRateResults)[i], 0, 'g', 7 ).arg( (*excessEntropyResults)[i], 0, 'g', 7 );
                text += "</table>";

                resultsDialog->setText( text );
                resultsDialog->show();

                delete entropyRateResults;
                delete excessEntropyResults;
            }
        }
    }
}



void QOptimalViewpointExtension::loadSegmentation()
{
    if ( !m_parameters )
    {
        std::cerr << "QOptimalViewpointExtension::loadSegmentation(): No hi ha paràmetres establerts" << std::endl;
        return;
    }

    Volume * volume = m_parameters->getVolumeObject();
    if ( !volume )
    {
        std::cerr << "QOptimalViewpointExtension::loadSegmentation(): No s'ha assignat el volum" << std::endl;
        return;
    }

    if ( !m_method ) m_method = new OptimalViewpoint();

    m_method->setImage( volume->getVtkData() );
    signed char numberOfClusters = m_method->loadSegmentationFromFile( m_parameters->getSegmentationFileName() );

    if ( numberOfClusters < 0 )
    {
        QMessageBox::critical( this, tr("Segmentation error"),
                               QString( tr("Cannot load segmentation from file ") )
                                + m_parameters->getSegmentationFileName() + "." );
        return;
    }

    m_parameters->setAdjustedTransferFunction( m_method->getAdjustedTransferFunction() );
    m_parameters->setNumberOfClusters( numberOfClusters );
    std::cout << "number of clusters: " << (short) numberOfClusters << std::endl;

    // TODO podríem fer la primera visualització per defecte
}



void QOptimalViewpointExtension::automaticSegmentation()
{
    if ( !m_parameters )
    {
        std::cerr << "QOptimalViewpointExtension::loadSegmentation(): No hi ha paràmetres establerts" << std::endl;
        return;
    }

    Volume * volume = m_parameters->getVolumeObject();
    if ( !volume )
    {
        std::cerr << "QOptimalViewpointExtension::loadSegmentation(): No s'ha assignat el volum" << std::endl;
        return;
    }

    if ( !m_method ) m_method = new OptimalViewpoint();

    m_method->setImage( volume->getVtkData() );

    unsigned char numberOfClusters = m_method->doAutomaticSegmentation(
            m_parameters->getSegmentationIterations(),
            m_parameters->getSegmentationBlockLength(),
            m_parameters->getSegmentationNumberOfClusters(),
            m_parameters->getSegmentationNoise(),
            m_parameters->getSegmentationImageSampleDistance(),
            m_parameters->getSegmentationSampleDistance() );

    m_parameters->setAdjustedTransferFunction( m_method->getAdjustedTransferFunction() );
    m_parameters->setNumberOfClusters( numberOfClusters );
    std::cout << "number of clusters: " << (short) numberOfClusters << std::endl;

    // TODO podríem fer la primera visualització per defecte
}



}

