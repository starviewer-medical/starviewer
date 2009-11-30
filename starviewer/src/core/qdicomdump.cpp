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
#include "qdicomdumpmammographywidget.h"
#include "qdicomdumpctlocalizerwidget.h"
#include "qdicomdumpcthelixwidget.h"
#include "qdicomdumpctwidget.h"
#include "qdicomdumpmrwidget.h"
#include <QDate>
#include <QTime>
#include <QBoxLayout>

namespace udg {

QDicomDump::QDicomDump(QWidget *parent)
 : QDialog(parent), m_lastInsertedDumpWidget(0)
{
    setupUi( this );
    // Obtenim el layout amb el que treballarem
    m_widgetLayout = qobject_cast<QBoxLayout *>( this->layout() );
    if( !m_widgetLayout )
    {
        DEBUG_LOG("no s'ens ha tornat el layout esperat! (QBoxLayout) Revisar el layout de QDicomDumpBase.ui!");
        Q_ASSERT( m_widgetLayout );
    }
    
    createConnections();
}

QDicomDump::~QDicomDump()
{

}

void QDicomDump::createConnections()
{
    // Connectem els butons
    connect( m_pushButtonAccept , SIGNAL( clicked() ) , SLOT( close() ) );
}

void QDicomDump::setCurrentDisplayedImage ( Image *currentImage )
{
    initialize();

    // Si teníem un widget d'una imatge anterior, l'hem d'eliminar abans per poder posar la nova informació
    if( m_lastInsertedDumpWidget )
    {
        m_widgetLayout->removeWidget( m_lastInsertedDumpWidget );
        delete m_lastInsertedDumpWidget;
        m_lastInsertedDumpWidget = 0;
    }
    
    // Segons el tipus d'imatge que tinguem, afegirem dinàmicament el widget amb la informació corresponent
    if ( currentImage != NULL )
    {        
        QString seriesModality = currentImage->getParentSeries()->getModality();
        
        setCommonImageTagsValue( currentImage ); // Descodifiquem els tags comuns per totes les imatges
        
        if ( seriesModality == "MR" ) // En funció de la modalitat cridem el QWidget que ens implementi el dicomdump per la modalitat
        {
            QDicomDumpMRWidget *widget = new QDicomDumpMRWidget;
            widget->setCurrentDisplayedImage( currentImage );

            m_lastInsertedDumpWidget = widget;

        }
        else if ( seriesModality == "CT" ) // Per a CT en funció del tipus d'imatge hem de mostrar informació diferent pel dicomdump
        {
            DICOMTagReader dicomReader;
            bool ok = dicomReader.setFile( currentImage->getPath() );

            if ( ok )
            {
                QString imageType = dicomReader.getAttributeByName( DCM_ImageType );

                if ( imageType.contains( "LOCALIZER" , Qt::CaseInsensitive ) )// Es tracta d'un survey
                {
                    QDicomDumpCTLocalizerWidget *widget = new QDicomDumpCTLocalizerWidget;
                    widget->setCurrentDisplayedImage( currentImage );                    
                    
                    m_lastInsertedDumpWidget = widget;
                }
                else if ( imageType.contains( "HELIX" , Qt::CaseInsensitive ) )// Es tracta d'una imatge helicoïdal
                {
                    QDicomDumpCTHelixWidget *widget = new QDicomDumpCTHelixWidget;
                    widget->setCurrentDisplayedImage( currentImage );

                    m_lastInsertedDumpWidget = widget;
                }
                else // QWidget de ct Genèric
                {
                    QDicomDumpCTWidget *widget = new QDicomDumpCTWidget;
                    widget->setCurrentDisplayedImage( currentImage );

                    m_lastInsertedDumpWidget = widget;
                }
            }
            else
            {
                // QWidget de ct Genèric
                QDicomDumpCTWidget *widget = new QDicomDumpCTWidget;
                widget->setCurrentDisplayedImage( currentImage );

                m_lastInsertedDumpWidget = widget;
            }
        }
        else if ( seriesModality == "MG" )
        {
            QDicomDumpMammographyWidget *widget = new QDicomDumpMammographyWidget;
            widget->setCurrentDisplayedImage( currentImage );
            if( m_lastInsertedDumpWidget )
                m_widgetLayout->removeWidget( m_lastInsertedDumpWidget );

            m_lastInsertedDumpWidget = widget;
        }

        // Si s'ha creat algun widget nou, l'inserim
        if( m_lastInsertedDumpWidget )
            m_widgetLayout->insertWidget(1,m_lastInsertedDumpWidget);
    }
}

void QDicomDump::initialize()
{
    // Definim  el valor dels tags de Pacient
    m_labelPatientIDValue->setText( "-" );
    m_labelPatientAgeValue->setText( "-" );
    m_labelPatientNameValue->setText( "-" );
    m_labelPatientSexValue->setText( "-" );
    m_labelImageDateValue->setText( "-" );
    m_labelImageTimeValue->setText( "-" );
    m_labelImageNumberValue->setText( "-" );
    m_birthDateValueLabel->setText( "-" );
}

void QDicomDump::setCommonImageTagsValue( Image *currentImage )
{
    Patient *currentPatient = currentImage->getParentSeries()->getParentStudy()->getParentPatient(); 

    // Definim  el valor dels tags de Pacient
    m_labelPatientIDValue->setText( currentPatient->getID() );

    if ( !currentImage->getParentSeries()->getParentStudy()->getPatientAge().isEmpty() )
    {
        m_labelPatientAgeValue->setText( currentImage->getParentSeries()->getParentStudy()->getPatientAge() );
    }

    if ( !currentPatient->getBirthDateAsString().isEmpty() )
    {
        m_birthDateValueLabel->setText( currentPatient->getBirthDateAsString() );
    }

    if ( !currentPatient->getFullName().isEmpty() )
    {
        m_labelPatientNameValue->setText( currentPatient->getFullName() );
    }

	if ( !currentPatient->getSex().isEmpty() )
    {
        m_labelPatientSexValue->setText( currentPatient->getSex() );
    }

    // Definim el valor dels tags d'imatge
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

        if (split.size() == 2) // Té fracció al final
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

};
