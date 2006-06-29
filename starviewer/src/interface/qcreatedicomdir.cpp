/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcreatedicomdir.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QDir>

#include "study.h"
#include "converttodicomdir.h"
#include "status.h"
#include "logging.h"

namespace udg {

QCreateDicomdir::QCreateDicomdir(QWidget *parent)
 : QWidget(parent)
{
    setupUi( this );
    
    m_dicomdirStudiesList->setColumnHidden( 7 , true );//Conte l'UID de l'estudi  
    
    createConnections();
}

void QCreateDicomdir::createConnections()
{
    connect( m_buttonRemove , SIGNAL( clicked() ) , this , SLOT( removeSelectedStudy() ) );
    connect( m_buttonRemoveAll , SIGNAL( clicked() ) , this , SLOT( removeAllStudies() ) );
    connect( m_buttonExamineDisk , SIGNAL( clicked() ) , this , SLOT( examineDicomdirPath() ) );
    connect( m_buttonCreateDicomdir , SIGNAL( clicked() ) , this , SLOT( createDicomdir() ) );
}

void QCreateDicomdir::addStudy( Study study )
{
    QTreeWidgetItem* item = new QTreeWidgetItem( m_dicomdirStudiesList );
    
    item->setText( 0 , study.getStudyId().c_str() );
    item->setText( 1 , study.getPatientId().c_str() );
    item->setText( 2 , study.getPatientName().c_str() );
    item->setText( 3 , study.getStudyModality().c_str() );
    item->setText( 4 , formatDate( study.getStudyDate().c_str() ) );
    item->setText( 5 , formatHour( study.getStudyTime().c_str() ) );
    item->setText( 6 , study.getStudyDescription().c_str() );
    item->setText( 7 , study.getStudyUID().c_str() );
}

void QCreateDicomdir::createDicomdir()
{
    ConvertToDicomdir convertToDicomdir;
    Status state;
    QString logMessage;

    if ( isCorrectDicomdirPath() ) //comprovem que el path on es vol guardar el dicom dir existeixi
    {
        QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( "*" , Qt::MatchWildcard, 0 ) );
        QTreeWidgetItem *item;
        
        if ( dicomdirStudiesList.count() == 0) //Comprovem que hi hagi estudis seleccionats per crear dicomdir
        {
            QMessageBox::information( this , tr( "StarViewer" ) , tr( "Please, first select the studies which you want to create a dicomdir" ) );
            return;
        }

        logMessage = "S'inicia la creació del dicomdir al directori";
        logMessage.append( m_lineEditDicomdirPath->text() );     
    
        INFO_LOG ( logMessage.toAscii().constData() );    
    
        for ( int i = 0; i < dicomdirStudiesList.count();i++ )
        {
            item = dicomdirStudiesList.at( i );
            convertToDicomdir.addStudy( item->text(7) );
    
            logMessage = "L'estudi ";
            logMessage.append( item->text(7) );
            logMessage.append( " s'afegirà al dicomdir " );
            INFO_LOG ( logMessage.toAscii().constData() );
        }
    
        state = convertToDicomdir.convert( m_lineEditDicomdirPath->text() );
        
        if ( !state.good() )
        {
            QMessageBox::critical( this , tr( "StarViewer" ) , tr( "Error creating Dicomdir" ) );
            logMessage = "Error al crear el Dicomdir ERROR : ";
            logMessage.append( state.text().c_str() );        
            ERROR_LOG ( logMessage.toAscii().constData() );
        }
        else INFO_LOG( "Finalitzada la creació del Dicomdir" );
    }
}

void QCreateDicomdir::examineDicomdirPath()
{
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./" , tr( "Dicomdir Directory" ) );
    QString path;
    
    dlg->setFileMode( QFileDialog::DirectoryOnly );
    
    if ( dlg->exec() == QDialog::Accepted )
    {
        if ( !dlg->selectedFiles().empty() ) m_lineEditDicomdirPath->setText( dlg->selectedFiles().takeFirst() );
    }
       
    delete dlg;
}

void QCreateDicomdir::removeAllStudies()
{
    m_dicomdirStudiesList->clear();
}

void QCreateDicomdir::removeSelectedStudy()
{
    if ( m_dicomdirStudiesList->currentItem() == NULL)
    {
        QMessageBox::information( this , tr( "StarViewer" ) , tr( "Please Select a study to remove" ) );
    }
    else delete m_dicomdirStudiesList->currentItem();
}

bool QCreateDicomdir::isCorrectDicomdirPath()
{
    QDir dicomdirPath( m_lineEditDicomdirPath->text() );

    if ( !dicomdirPath.exists() )
    {
            switch ( QMessageBox::question( this ,
                    tr( "Create directory ?" ) ,
                    tr( "The Dicomdir directory doesn't exists. Do you want to create it ?" ) ,
                    tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
            {
                case 0:
                    if ( !dicomdirPath.mkpath( m_lineEditDicomdirPath->text() ) )
                    {
                        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "Can't create the directory. Please check users permission" ) );
                        return false;
                    }
                    else return true;
                    break;
                case 1: 
                    return false;
                    break;
            }
    }
    else return true;

    
}

QString QCreateDicomdir::formatDate( const std::string date )
{
    QString formateDate , originalDate ( date.c_str() );
    
    formateDate.insert( 0 , originalDate.mid( 6 , 2 ) ); //dd
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 4 , 2 ) );
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 0 , 4 ) );
    
    return formateDate;
}

QString QCreateDicomdir::formatHour( const std::string hour )
{
    QString formatedHour,originalHour( hour.c_str() );
    
    formatedHour.insert( 0 , originalHour.mid( 0 , 2 ) );
    formatedHour.append( ":" );
    formatedHour.append( originalHour.mid( 2 , 2 ) );
    
    return formatedHour;
}

QCreateDicomdir::~QCreateDicomdir()
{
}

}
