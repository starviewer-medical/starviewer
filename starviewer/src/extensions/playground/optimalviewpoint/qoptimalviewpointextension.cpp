/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "qoptimalviewpointextension.h"

#include <QMessageBox>

#include "optimalviewpoint.h"
#include "optimalviewpointparameters.h"

#include "logging.h"
#include "volume.h"

#include <iostream>


namespace udg {


QOptimalViewpointExtension::QOptimalViewpointExtension( QWidget * parent )
    : QWidget( parent )
{
    setupUi( this );

    m_parameters = new OptimalViewpointParameters( this );

    m_inputParametersWidget->setParameters( m_parameters );

    m_method = new OptimalViewpoint();
    m_method->setParameters( m_parameters );
    m_method->setMainRenderer( m_viewerWidget->getRenderer() );

    connect( m_inputParametersWidget, SIGNAL( segmentationRequested() ), SLOT( doSegmentation() ) );
    connect( m_inputParametersWidget, SIGNAL( executionRequested() ), SLOT( execute() ) );
}


QOptimalViewpointExtension::~QOptimalViewpointExtension()
{
    delete m_method;
}


void QOptimalViewpointExtension::setInput( Volume * input )
{
    Q_ASSERT( input );
    Q_ASSERT( !m_parameters->getVolumeObject() );
    m_parameters->setVolumeObject( input );
    int dims[3];
    input->getDimensions( dims );
    m_inputParametersWidget->setNumberOfSlices( dims[2] );
}


void QOptimalViewpointExtension::doSegmentation()
{
    switch ( m_parameters->getSegmentation() )
    {
        case OptimalViewpointParameters::LoadSegmentation:
            if ( !m_method->loadSegmentationFromFile() )
            {
                QMessageBox::critical( this, tr("Segmentation error"),
                                       QString( tr("Cannot load segmentation from file %1.") ).arg( m_parameters->getSegmentationFileName() ) );
                return;
            }
            break;

        case OptimalViewpointParameters::AutomaticSegmentation:
            m_method->doAutomaticSegmentation();
            break;
    }

    // De moment cal posar aquestes dues línies. Potser es pot arreglar perquè no calguin.
    m_method->setNumberOfPlanes( 0 );
    m_method->setTransferFunction( m_parameters->getTransferFunctionObject() );
    m_viewerWidget->render();
}


void QOptimalViewpointExtension::execute()
{
    // nous paràmetres

    m_method->setOpacityForComputing( m_parameters->getComputeWithOpacity() );
    m_method->setInterpolation( m_parameters->getInterpolation() );
    m_method->setSpecular( m_parameters->getSpecular() );
    m_method->setSpecularPower( m_parameters->getSpecularPower() );
    m_method->setUpdatePlane( m_parameters->getUpdatePlane() );
    m_method->setCompute( m_parameters->getCompute() );
    m_method->setSimilarityThreshold( m_parameters->getSimilarityThreshold() );

    bool renderCluster = m_parameters->getCluster();
    if ( renderCluster ) m_method->setClusterLimits( m_parameters->getClusterFirst(), m_parameters->getClusterLast() );
    m_method->setRenderCluster( renderCluster );

    m_method->setReadExtentFromFile( m_parameters->getReadExtentFromFile() );



    m_method->updatePlanes();
    std::cout << "OVD: update planes" << std::endl;
//     m_viewer->render();
    m_viewerWidget->render();

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
