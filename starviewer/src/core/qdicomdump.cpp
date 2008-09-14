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
#include "dicomtagreader.h"
#include <QDate>
#include <QTime>

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
            DICOMTagReader dicomReader;
            bool ok = dicomReader.setFile( currentImage->getPath() );

            if ( ok )
            {
                QString imageType = dicomReader.getAttributeByName( DCM_ImageType );

                if ( imageType.contains( "LOCALIZER" , Qt::CaseInsensitive ) )//Es tracta d'un survey
                {
                    m_qdicomDumpCTLocalizerWidget->setVisible( true );
                    m_qdicomDumpCTLocalizerWidget->setCurrentDisplayedImage( currentImage );
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
            else
            {
                //QWidget de ct Genèric
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

    if ( currentImage->getParentSeries()->getParentStudy()->getPatientAgeAsString() != "" )
    {
        m_labelPatientAgeValue->setText( currentImage->getParentSeries()->getParentStudy()->getPatientAgeAsString() );
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
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( currentImage->getPath() );

    QString value = dicomReader.getAttributeByName( DCM_ContentDate );
    if( !value.isEmpty() )
    {
        // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format yyyy.MM.dd
        m_labelImageDateValue->setText(QDate::fromString(value.remove("."), "yyyyMMdd").toString(Qt::LocaleDate));
    }

    value = dicomReader.getAttributeByName( DCM_ContentTime );
    if( !value.isEmpty() )
    {
        // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format hh:mm:ss.frac
        value = value.remove(":");

        QStringList split = value.split(".");
        QTime convertedTime = QTime::fromString(split[0], "hhmmss");

        if (split.size() == 2) //té fracció al final
        {
            // Trunquem a milisegons i no a milionèssimes de segons
            convertedTime = convertedTime.addMSecs( split[1].leftJustified(3,'0',true).toInt() );
        }
        m_labelImageTimeValue->setText(convertedTime.toString(Qt::LocaleDate));
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
    m_qdicomDumpCTLocalizerWidget->setVisible( false );
}

void QDicomDump::closeWindow()
{
    this->close();
}

};
