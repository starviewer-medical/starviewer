/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

 
#include "mutualinformationinputparametersform.h"

#include "mutualinformationparameters.h"
// per podr accedir al combo
#include "qvolumerepositorycombobox.h"
// per poder connectar signals
#include <QPushButton>
#include <QLineEdit>

// CPP -> cerr
#include <iostream>

namespace udg {

MutualInformationInputParametersForm::MutualInformationInputParametersForm( QWidget *parent )
 : QInputParameters( parent )
{
    setupUi( this );
    m_parameters = 0;
    // aquests valors són temporals, més endavant vindrien donats per signals
    // i slots al escollir els volums a tractar
    
    this->disableIndividualSincronization();
}


MutualInformationInputParametersForm::~MutualInformationInputParametersForm()
{
}

void MutualInformationInputParametersForm::setParameters( MutualInformationParameters* parameters )
{
    m_parameters = parameters;
}

/**
    El mètode abstracte que cal re-implementar
    en el cas que sincronitzem els paràmetres de cop
*/
void MutualInformationInputParametersForm::writeAllParameters()
{
    if( !m_parameters )
    {
        // provocar err, excepcio...
        std::cerr << "MutualInformationInputParametersForm:: No hi ha parametres establerts " << std::endl;
    }
    else
    {
        // actualitzem els valors dels paràmetres
        
        // aquests seran de mentres així
        // cuidado perquè això no hauria de ser així exactament
        // s'hauria de verificar que aquests volums hi són al repositori realment
        // o be la validacio s'hauria de fer en el moment anterior de l'execució
        //m_parameters->setFixedImageID( m_fixedID );
        //m_parameters->setMovingImageID( m_movingID );
        m_parameters->setMovingImageID( m_volumeRepositoryComboBox->getVolumeID() );
        
        // gaussian filter
        m_parameters->setFixedImageVariance( m_txtFixedImageVariance->text().toDouble() );
        m_parameters->setMovingImageVariance( m_txtMovingImageVariance->text().toDouble() );
        
        // optimizer
        m_parameters->setLearningRate( m_txtLearningRate->text().toDouble() );
        m_parameters->setNumberOfIterations( m_txtNumberOfIterations->text().toInt() );
        
        // metric
        m_parameters->setFixedImageStandardDeviation( m_txtFixedImageStandardDeviation->text().toDouble() );
        m_parameters->setMovingImageStandardDeviation( m_txtMovingImageStandardDeviation->text().toDouble() );
        m_parameters->setSpatialSamples( m_txtNumberOfSamples->text().toInt() );
           
    }
}


void
MutualInformationInputParametersForm::readParameter( int index )
{
    switch( index )
    {
    
    case MutualInformationParameters::FixedImageID :
        //m_fixedID = m_parameters->getFixedImageID();
    break;
    
    case MutualInformationParameters::MovingImageID :
//         m_movingID = m_parameters->getMovingImageID();
        m_volumeRepositoryComboBox->setVolumeID( m_parameters->getMovingImageID() );
    break;
    
    case MutualInformationParameters::FixedImageVariance :
        m_txtFixedImageVariance->setText( QString::number( m_parameters->getFixedImageVariance() ) );
    break;
    case MutualInformationParameters::MovingImageVariance :
        m_txtMovingImageVariance->setText( QString::number( m_parameters->getMovingImageVariance() ) );
    break;
    
    
    case MutualInformationParameters::LearningRate :
        m_txtLearningRate->setText( QString::number( m_parameters->getLearningRate() ) );
    break;
    case MutualInformationParameters::NumberOfIterations :
        m_txtNumberOfIterations->setText( QString::number( m_parameters->getNumberOfIterations() ) );
    break;
    
    case MutualInformationParameters::FixedImageStandardDeviation :
        m_txtFixedImageStandardDeviation->setText( QString::number( m_parameters->getFixedImageStandardDeviation() ) );
    break;
    case MutualInformationParameters::MovingImageStandardDeviation :
        m_txtMovingImageVariance->setText( QString::number( m_parameters->getMovingImageVariance() ) );
    break;
    case MutualInformationParameters::SpatialSamples :
        m_txtNumberOfSamples->setText( QString::number( m_parameters->getSpatialSamples() ) );
    break;
    
    }
}





}; // end namespace udg
