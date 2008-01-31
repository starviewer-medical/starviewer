/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdump.h"
#include "series.h"
#include "image.h"
#include "patient.h"
#include "logging.h"

namespace udg {

QDicomDump::QDicomDump(QWidget *parent)
 : QDialog(parent)
{
    setupUi( this );

    createConnections();
}

QDicomDump::~QDicomDump()
{

}

void QDicomDump::createConnections()
{
    //connectem els butons
    connect( m_pushButtonAccept , SIGNAL( clicked() ) , SLOT( closeWindow() ) );
}

void QDicomDump::setCurrentDisplayedImage ( Image *currentImage )
{
    initialize();
    setNoVisibleAllDicomDumpWidgets();//Fem tots els widgets del formulari invisibles

    if ( currentImage != NULL )
    {
        QString seriesModality = currentImage->getParentSeries()->getModality();
        
        setCommonImageTagsValue( currentImage ); //Descodifiquem els tags comuns per totes les imatges
        
        if ( seriesModality == "MR" ) //En funció de la modalitat cridem el QWidget que ens implementi el dicomdump per la modalitat
        {
            m_qdicomDumpMRWidget->setVisible( true );
            m_qdicomDumpMRWidget->setCurrentDisplayedImage( currentImage );
        }
        else if ( seriesModality == "CT" ) //Per a CT en funció del tipus d'imatge hem de mostrar informació diferent pel dicomdump
        {
            QString imageType = currentImage->getImageType();
    
            if ( imageType.contains( "LOCALIZER" , Qt::CaseInsensitive ) )//Es tracta d'un survey
            {
                m_qdicomDumpCTSurveyWidget->setVisible( true );
                m_qdicomDumpCTSurveyWidget->setCurrentDisplayedImage( currentImage );
            }
            else if ( imageType.contains( "HELIX" , Qt::CaseInsensitive ) )//Es tracta d'una imatge helicoïdal
            {
                m_qdicomDumpCTHelixWidget->setVisible( true );
                m_qdicomDumpCTHelixWidget->setCurrentDisplayedImage( currentImage );
            }
            else //QWidget de ct Genèric
            {
                m_qdicomDumpCTWidget->setVisible( true );
                m_qdicomDumpCTWidget->setCurrentDisplayedImage( currentImage );
            }
        }
    }
}

void QDicomDump::initialize()
{
    //Definim  el valor dels tags de Pacient
    m_labelPatientIDValue->setText( "-" );
    m_labelPatientAgeValue->setText( "-" );
    m_labelPatientNameValue->setText( "-" );
    m_labelPatientSexValue->setText( "-" );
    m_labelImageDateValue->setText( "-" );
    m_labelImageTimeValue->setText( "-" );
    m_labelImageNumberValue->setText( "-" );
}

void QDicomDump::setCommonImageTagsValue( Image *currentImage )
{
    Patient *currentPatient = currentImage->getParentSeries()->getParentStudy()->getParentPatient(); 

    //Definim  el valor dels tags de Pacient
    m_labelPatientIDValue->setText( currentPatient->getID() );
    
    //Millor no mostrar-la en el cas que sigui 0 que no dona informació errònia, pq quan el tag patieng age no existeix sempre val 0
    if ( currentImage->getParentSeries()->getParentStudy()->getPatientAge() != 0 )
    {
        m_labelPatientAgeValue->setText( QString::number( currentImage->getParentSeries()->getParentStudy()->getPatientAge() , 10 ) );
    }

    if ( currentPatient->getFullName() != "" )
    {
        m_labelPatientNameValue->setText( currentPatient->getFullName() );
    }

    if ( currentPatient->getSex() != "" )
    {
        m_labelPatientSexValue->setText( currentPatient->getSex() );
    }

    //Definim el valor dels tags d'imatge
    if ( currentImage->getContentDateAsString() != "" )
    {
        m_labelImageDateValue->setText( currentImage->getContentDateAsString() );
    }

    if ( currentImage->getContentTimeAsString() != "" )
    {
        m_labelImageTimeValue->setText( currentImage->getContentTimeAsString() );
    }

    if ( currentImage->getInstanceNumber() != "" )
    {
        m_labelImageNumberValue->setText( currentImage->getInstanceNumber() );
    }
}

void QDicomDump::setNoVisibleAllDicomDumpWidgets()
{
    m_qdicomDumpCTHelixWidget->setVisible( false );
    m_qdicomDumpMRWidget->setVisible( false );
    m_qdicomDumpCTWidget->setVisible( false );
    m_qdicomDumpCTSurveyWidget->setVisible( false );
}

void QDicomDump::closeWindow()
{
    this->close();
}

};
