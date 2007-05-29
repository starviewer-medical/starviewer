/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "optimalviewpointdirector.h"

#include <iostream>

#include <QMessageBox>
#include <QLayout>

#include <QVTKWidget.h> // perquè funcioni m_method->setInteractor( m_viewer->getInteractor() );

#include "qapplicationmainwindow.h"
#include "volume.h"
#include "volumerepository.h"

#include "optimalviewpoint.h"
#include "optimalviewpointparameters.h"
#include "optimalviewpointviewer.h"

namespace udg {

OptimalViewpointDirector::OptimalViewpointDirector( QWidget * viewerWidget, QObject * parent )
    : QObject( parent )
{
    m_viewerWidget = viewerWidget;
    m_method = 0;
    m_parameters = 0;
    m_viewer = 0;
}

OptimalViewpointDirector::~OptimalViewpointDirector()
{
    delete m_method;
//     delete m_viewer; // sembla que el destrueix algú altre (Qt automàticament?)
}

/// Assigna l'objecte que conté els paràmetres del mètode de visualització.
void OptimalViewpointDirector::setParameters( OptimalViewpointParameters * parameters )
{
    m_parameters = parameters;
}

/**
 * Aquest mètode aplica el mètode de visualització. Comprova primer si té
 * els paràmetres necessaris, sinó no fa res. Slot públic heredat de
 * Director.
 */
void OptimalViewpointDirector::execute()
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
            if ( !m_method )
            {
                m_method = new OptimalViewpoint();
                m_viewer = new OptimalViewpointViewer( m_viewerWidget );
                m_viewerWidget->layout()->addWidget( m_viewer );
                m_viewer->setRenderer( m_method->getMainRenderer() );
                m_method->setInteractor( m_viewer->getInteractor() );
//                 m_applicationWindow->addWorkingAreaWidget( m_viewer, tr("Optimal Viewpoint") );
//                 VolumeRepository * repository = VolumeRepository::getRepository();
                m_method->setImage( volume->getVtkData() );
//                 m_method->setImageFileName( repository->getVolume( volumeId )->getFileName() );
                m_method->setSegmentationFileName( m_parameters->getSegmentationFileName() );

                // segmentació del model
                unsigned char n = m_method->segmentateImage(
                        m_parameters->getSegmentationIterations(),
                        m_parameters->getSegmentationBlockLength(),
                        m_parameters->getSegmentationNumberOfClusters(),
                        m_parameters->getSegmentationNoise(),
                        m_parameters->getSegmentationImageSampleDistance(),
                        m_parameters->getSegmentationSampleDistance() );

                // funció de transferència ajustada
                m_parameters->setAdjustedTransferFunction( m_method->getAdjustedTransferFunction() );
                m_parameters->setNumberOfClusters( n );
                std::cout << "number of clusters: " << (short) n << std::endl;
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
            m_method->setTransferFunction( m_parameters->getTransferFunction() );
            std::cout << "OVD: set transfer function" << std::endl;



            // nous paràmetres

            m_method->setOpacityForComputing( m_parameters->getComputeWithOpacity() );
            m_method->setInterpolation( m_parameters->getInterpolation() );
            m_method->setSpecular( m_parameters->getSpecular() );
            m_method->setSpecularPower( m_parameters->getSpecularPower() );
            m_method->setUpdatePlane( m_parameters->getUpdatePlane() );
            m_method->setCompute( m_parameters->getCompute() );



            m_method->updatePlanes();
            std::cout << "OVD: update planes" << std::endl;
            m_viewer->update();

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

}; // end namespace udg
