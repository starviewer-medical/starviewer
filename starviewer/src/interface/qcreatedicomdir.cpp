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
#include <QFile>
#include <QProcess>
#include <QContextMenuEvent>
#include <QShortcut>

#include "study.h"
#include "converttodicomdir.h"
#include "status.h"
#include "logging.h"
#include "status.h"
#include "cacheimagedal.h"
#include "imagemask.h"
#include "harddiskinformation.h"
#include "deletedirectory.h"

namespace udg {

QCreateDicomdir::QCreateDicomdir(QWidget *parent)
 : QWidget(parent)
{
    setupUi( this );
    
    QString sizeOfDicomdirText;
    
    m_dicomdirStudiesList->setColumnHidden( 7 , true );//Conte l'UID de l'estudi  
    
    createConnections();

    m_dicomdirSize = 0;

    setDicomdirSize();
    
    //com per defecte carreguem del combo l'opcio copia a disc o dispositiu extern, amaguem aquests dos objectes que només s'utilitzen per quan gravem en cd o dvd
    m_labelMbCdDvdOcupat->setVisible( false );
    m_progressBarOcupat->setVisible( false );
    
    //per defecte gravem al disc dur per tant, l'espai és il·limitat
    m_DiskSpace = ( unsigned long ) 9999999 * (unsigned long) ( 1024 * 1024 );
}

void QCreateDicomdir::createConnections()
{
    connect( m_buttonRemove , SIGNAL( clicked() ) , this , SLOT( removeSelectedStudy() ) );
    connect( m_buttonRemoveAll , SIGNAL( clicked() ) , this , SLOT( removeAllStudies() ) );
    connect( m_buttonExamineDisk , SIGNAL( clicked() ) , this , SLOT( examineDicomdirPath() ) );
    connect( m_buttonCreateDicomdir , SIGNAL( clicked() ) , this , SLOT( createDicomdir() ) );
    connect( m_comboBoxAction , SIGNAL( currentIndexChanged( int ) ) , this , SLOT( changedAction( int ) ) );
}

void QCreateDicomdir::changedAction( int index )
{
    float sizeInMB = (m_dicomdirSize / ( 1024 * 1024 ) );

    switch( index )
    {
        case 0 : //disc dur o dispositiu extrable
                 m_labelMbCdDvdOcupat->setVisible( false );
                 m_progressBarOcupat->setVisible( false );
                 m_labelSizeOfDicomdir->setVisible( true );
                 m_lineEditDicomdirPath->setVisible( true );
                 m_buttonExamineDisk->setVisible( true );
                 m_labelCreateDicomdir->setText( tr( "Create dicomdir at"  ) );
                 m_DiskSpace = ( unsigned long ) 9999999 * ( unsigned long ) ( 1024 * 1024 );//per gravar al disc no hi ha màxim
                 break;
        case 1 : //cd
                 if ( sizeInMB < 700 )
                 {
                    m_labelMbCdDvdOcupat->setVisible( true );
                    m_progressBarOcupat->setVisible( true );
                    m_labelSizeOfDicomdir->setVisible( false );
                    m_lineEditDicomdirPath->setVisible( false );
                    m_buttonExamineDisk->setVisible( false );
                    m_labelCreateDicomdir->setText( tr( "Cd occupied" ) );
                    m_progressBarOcupat->setMaximum( 700 );
                    m_DiskSpace = ( unsigned long ) 700 * ( unsigned long ) ( 1024 * 1024 ); // convertim a bytes capacaticat cd
                    m_progressBarOcupat->repaint();
                 }
                 else
                 {
                    QMessageBox::warning( this , tr( "StarViewer" ) , tr( "The device doesn't have enough space to copy all this studies, please remove some studies. The capacity of a cd is 700 Mb" ) );
                    m_comboBoxAction->setCurrentIndex( 0 );
                 }
                 break;
        case 2 : //dvd
                 if ( sizeInMB < 4400 )
                 {
                    m_labelMbCdDvdOcupat->setVisible( true );
                    m_progressBarOcupat->setVisible( true );
                    m_labelSizeOfDicomdir->setVisible( false );
                    m_lineEditDicomdirPath->setVisible( false );
                    m_buttonExamineDisk->setVisible( false );
                    m_labelCreateDicomdir->setText( tr( "Dvd Ocuppied" ) );
                    m_progressBarOcupat->setMaximum( 4400 );
                    m_DiskSpace = ( unsigned long ) 4400 * ( unsigned long ) ( 1024 * 1024 ); //convertim a bytes capacitat dvd
                    m_progressBarOcupat->repaint();
                 }
                 else
                 {
                    QMessageBox::warning( this , tr( "StarViewer" ) , tr( "The device doesn't have enough space to copy all this studies, please remove some studies. The capacity of a dvd is 4400 Mb" ) );
                    m_comboBoxAction->setCurrentIndex( 0 );
                 }
                 break;
    }
    
    //Si la mida del dicomdir excedeix el maxim de la barra de progrés, com a valor a la barra de progrés li assignem el seu màxim
    if ( index > 0 )
    {
        if ( sizeInMB < m_progressBarOcupat->maximum() ) 
        {
            m_progressBarOcupat->setValue( int ( sizeInMB ) );
        }
        else m_progressBarOcupat->setValue( m_progressBarOcupat->maximum() );
    }
}

void QCreateDicomdir::setDicomdirSize()
{
    QString sizeOfDicomdirText, sizeText;
    float sizeInMb;

    sizeInMb = m_dicomdirSize / ( 1024 * 1024 );//passem a Mb

    sizeOfDicomdirText.insert( 0 , tr( "The size of Dicomdir is " ) );
    sizeText.setNum( sizeInMb , 'f' , 2 );
    sizeOfDicomdirText.append( sizeText );
    sizeOfDicomdirText.append( " Mb" );
    m_labelSizeOfDicomdir->setText( sizeOfDicomdirText );
    
    if ( sizeInMb < m_progressBarOcupat->maximum() ) 
    {
        m_progressBarOcupat->setValue( int ( sizeInMb ) );
    }
    else m_progressBarOcupat->setValue( m_progressBarOcupat->maximum() );
    
    sizeText.setNum( sizeInMb , 'f' , 0 );
    sizeOfDicomdirText.clear();
    sizeOfDicomdirText.insert( 0 , sizeText );
    sizeOfDicomdirText.append( " Mb" );
    
    m_labelMbCdDvdOcupat->setText( sizeOfDicomdirText );
}

void QCreateDicomdir::addStudy( Study study )
{
    CacheImageDAL cacheImageDAL;
    ImageMask imageMask;
    unsigned long studySize;
    Status state;

    if ( !existsStudy( study.getStudyUID().c_str() ) )
    {
        //consultem la mida de l'estudi
        imageMask.setStudyUID( study.getStudyUID() );
        
        state = cacheImageDAL.imageSize( imageMask , studySize );        

        if ( !state.good() )
        {
            databaseError ( &state );
            return;
        }        
        
        //només comprovem l'espai si gravem a un cd o dvd
        if ( studySize + m_dicomdirSize > m_DiskSpace && m_comboBoxAction->currentIndex() != 0 )
        {
            QMessageBox::warning( this , tr( "StarViewer" ) , tr( "With this study the Dicomdir exceeds the size of the device. Please change the device or create the dicomdir" ) );
        }
        else
        {   //afegim la informació de l'estudi a la llista
            QTreeWidgetItem* item = new QTreeWidgetItem( m_dicomdirStudiesList );
            m_dicomdirSize = m_dicomdirSize + studySize;
            setDicomdirSize();
    
            item->setText( 0 , study.getStudyId().c_str() );
            item->setText( 1 , study.getPatientId().c_str() );
            item->setText( 2 , study.getPatientName().c_str() );
            item->setText( 3 , study.getStudyModality().c_str() );
            item->setText( 4 , formatDate( study.getStudyDate().c_str() ) );
            item->setText( 5 , formatHour( study.getStudyTime().c_str() ) );
            item->setText( 6 , study.getStudyDescription().c_str() );
            item->setText( 7 , study.getStudyUID().c_str() );
        }
    }
    else
    {
        QMessageBox::warning( this , tr( "StarViewer" ) , tr( "The study exists in the Dicomdir list" ) );
    }
}

void QCreateDicomdir::createDicomdir()
{
    switch( m_comboBoxAction->currentIndex() )
    {
        case 0 : //disc dur o dispositiu extrable
                 createDicomdirOnHard();
                 break;
        case 1 : //cd
                 if ( createDicomdirOnCdOrDvd().good() ) executek3b( cd );
                 break;
        case 2 : //dvd
                 if ( createDicomdirOnCdOrDvd().good() ) executek3b( dvd );
                 break;
    }
}

Status QCreateDicomdir::createDicomdirOnCdOrDvd()
{
    QDir temporaryDirPath;
    QString dicomdirPath, logMessage, readmetxtPath;
    Status state;   
    ConvertToDicomdir convertToDicomdir;
       
    dicomdirPath.insert( 0 , temporaryDirPath.tempPath() );
    dicomdirPath.append( "/DICOMDIR/DICOMDIR" ); // per la norma del IHE el dicomdir ha d'estar situat dins el directori DICOMDIR
        
    readmetxtPath.insert( 0 , temporaryDirPath.tempPath() );
    readmetxtPath.append( "/DICOMDIR" );//indiquem el path on s'ha de crear el fitxer README.TXT amb informacio de qui ha creat el DICOMDIR, només es genera quan es grava en cd o dvd
        
    //si el directori dicomdir ja existeix al temporal l'esborrem
    if ( temporaryDirPath.exists( dicomdirPath ) )
    {
        DeleteDirectory delDirectory;
        delDirectory.deleteDirectory( dicomdirPath , true );
    }
        
    INFO_LOG ( "Iniciant la creació del dicomdir en cd-dvd" );
    
    if ( !temporaryDirPath.mkpath( dicomdirPath ) )//Creem el directori temporal
    {
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "Can't create the temporary directory. Please check users permission" ) );
        logMessage = "Error al crear directori ";
        logMessage.append( dicomdirPath );
        DEBUG_LOG( logMessage.toAscii().constData() );
        return state.setStatus( "Can't create temporary dicomdir", false , 3002 );
    }
    else
    {        
        convertToDicomdir.createReadmeTxt( readmetxtPath );
        return startCreateDicomdir( dicomdirPath );
    }
}

void QCreateDicomdir::createDicomdirOnHard()
{
    QString dicomdirPath = m_lineEditDicomdirPath->text() , logMessage;
    DeleteDirectory delDirectory;
    QDir directoryDicomdirPath( dicomdirPath );

    //Comprovem si el directori ja es un dicomdir, si és el cas demanem a l'usuari si el desitja sobreecriue o, els estudis seleccionats s'afegiran ja al dicomdir existent
    
    INFO_LOG ( "Iniciant la creació del dicomdir en el disc dur o dispositiu extern" );

    if ( m_lineEditDicomdirPath->text().length() == 0 )
    {
        QMessageBox::information( this , tr( "StarViewer" ) , tr( "Please enter a diretory to create de dicomdir" ) );
        return;
    }
    
    if ( dicomdirPathIsADicomdir( dicomdirPath ) )
    {
        switch ( QMessageBox::question( this ,
                tr( "Create Dicomdir" ) ,
                tr( "The directory contains a dicomdir, do you want to overwrite and delete all the files in the directory ?" ) ,
                tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
        {
            case 0: // si vol sobreescriure, esborrem el contingut del directori 
                delDirectory.deleteDirectory( dicomdirPath , false );
                break;
            case 1:
                return; //no fem res, l'usuari no vol sobreescriure el directori, cancel·lem l'operacio i tornem el control a l'usuari
                break;
        }
    }
    else
    {   //el directori no és un dicomdir
        if ( !directoryDicomdirPath.exists() )//si el directori no existiex, preguntem si el vol crear
        {
                switch ( QMessageBox::question( this ,
                        tr( "Create directory ?" ) ,
                        tr( "The Dicomdir directory doesn't exists. Do you want to create it ?" ) ,
                        tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
                {
                    case 0:
                        if ( !directoryDicomdirPath.mkpath( dicomdirPath ) )
                        {
                            QMessageBox::critical( this , tr( "StarViewer" ) , tr( "Can't create the directory. Please check users permission" ) );
                            logMessage = "Error al crear directori ";
                            logMessage.append( dicomdirPath );
                            DEBUG_LOG( logMessage.toAscii().constData() );
                        }
                        break;
                    case 1: 
                        return; //cancel·lem
                        break;
                }
        }    
    }
        
    startCreateDicomdir( dicomdirPath );
}

Status QCreateDicomdir::startCreateDicomdir( QString dicomdirPath )
{
    ConvertToDicomdir convertToDicomdir;
    Status state;
    QString logMessage;

    INFO_LOG ( logMessage.toAscii().constData() );    

    if ( !enoughFreeSpace( dicomdirPath ) )// comprovem si hi ha suficient espai lliure al disc dur
    {
        QMessageBox::information( this , tr( "StarViewer" ) , tr( "Not enough free space to create dicom dir. Please free space" ) );
        
        logMessage = "Error al crear el Dicomdir, no hi ha suficient espai al disc ERROR : ";
        logMessage.append( state.text().c_str() );        
        ERROR_LOG ( logMessage.toAscii().constData() );
        return state.setStatus( "Not enough space to create dicomdir", false , 3000 );
    }         

    QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( "*" , Qt::MatchWildcard, 0 ) );
    QTreeWidgetItem *item;
    
    if ( dicomdirStudiesList.count() == 0 ) //Comprovem que hi hagi estudis seleccionats per crear dicomdir
    {
        QMessageBox::information( this , tr( "StarViewer" ) , tr( "Please, first select the studies which you want to create a dicomdir" ) );
        return state.setStatus( "No study selected to create dicomdir", false , 3001 );
    }

    for ( int i = 0; i < dicomdirStudiesList.count(); i++ ) 
    {
        item = dicomdirStudiesList.at( i );
        convertToDicomdir.addStudy( item->text( 7 ) ); // indiquem a la classe convertToDicomdir, quins estudis s'ha de convertir a dicomdir, passant el UID de l'estudi

        logMessage = "L'estudi ";
        logMessage.append( item->text( 7 ) );
        logMessage.append( " s'afegirà al dicomdir " );
        INFO_LOG ( logMessage.toAscii().constData() );
    }

    state = convertToDicomdir.convert( dicomdirPath );//s'inicia la conversió
    
    if ( !state.good() )
    {
        QMessageBox::critical( this , tr( "StarViewer" ) , tr( "Error creating Dicomdir. Be sure you have user permissions in " ) + m_lineEditDicomdirPath->text() + " and that the directory is empty " );
        logMessage = "Error al crear el Dicomdir ERROR : ";
        logMessage.append( state.text().c_str() );        
        ERROR_LOG ( logMessage.toAscii().constData() );
    }
    else 
    {
        INFO_LOG( "Finalitzada la creació del Dicomdir" );
        clearQCreateDicomdirScreen();
    }
    
    return state;
}

void QCreateDicomdir::clearQCreateDicomdirScreen()
{
    m_dicomdirStudiesList->clear();
    m_lineEditDicomdirPath->setText( "" );
    
    m_labelSizeOfDicomdir->setText( tr( "The size of Dicomdir is 0 Mb" ) );
    
    m_dicomdirSize = 0;
    setDicomdirSize();//Reiniciem la barra de progrés
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
    m_dicomdirSize = 0;
    setDicomdirSize();

    m_dicomdirStudiesList->clear();
    
    setDicomdirSize();
}

void QCreateDicomdir::removeSelectedStudy()
{
    ImageMask imageMask;
    CacheImageDAL cacheImageDAL;
    Status state;
    unsigned long studySize;
    QList<QTreeWidgetItem *> selectedStudies;
     
    selectedStudies = m_dicomdirStudiesList->selectedItems();
    
    
    if ( selectedStudies.count() == 0 )
    {
        QMessageBox::information( this , tr( "StarViewer" ) , tr( "Please Select a study to remove" ) );
    }
    else
    {
        for ( int i = 0; i < selectedStudies.count(); i++)
        {
            //consultem la mida de l'estudi
            imageMask.setStudyUID( selectedStudies.at( i )->text( 7 ).toAscii().constData() );
            
            state = cacheImageDAL.imageSize( imageMask , studySize );        
    
            if ( !state.good() )
            {
                databaseError ( &state );
                return;
            }        
    
            m_dicomdirSize = m_dicomdirSize - studySize;
            setDicomdirSize();
    
            delete selectedStudies.at( i );;
        }
    }
}

bool QCreateDicomdir::existsStudy( QString studyUID )
{
    QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( studyUID , Qt::MatchExactly, 7 ) );
    
    if ( dicomdirStudiesList.count() > 0 )
    {
        return true;
    }
    else return false;
}

void QCreateDicomdir::executek3b( recordDevice device )
{
    QProcess k3b;
    QStringList k3bParamatersList;
    QDir temporaryDirPath;    
    QString paramater;
    
    k3bParamatersList.push_back( "--nosplash" );
    
    switch( device )
    {
        case cd :   k3bParamatersList.push_back( "--datacd" );
                    paramater= temporaryDirPath.tempPath() + "/DICOMDIR/";
                    k3bParamatersList.push_back(  paramater );
                    //k3bParamatersList.push_back( "*" );
                    k3b.execute( "k3b" , k3bParamatersList );
                    break;
        case dvd:   k3bParamatersList.push_back( "--datadvd" );
                    paramater= temporaryDirPath.tempPath() + "/DICOMDIR/";
                    k3bParamatersList.push_back(  paramater );
                    k3b.execute( "k3b" , k3bParamatersList );
                    break;
    }
}

bool QCreateDicomdir::enoughFreeSpace( QString path )
{
    HardDiskInformation hardDisk;

    if ( hardDisk.getTotalNumberOfBytes( path.toAscii().constData() ) < m_dicomdirSize )
    {
        return false;
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

void QCreateDicomdir::clearTemporaryDir()
{
    QString dicomdirPath, logMessage;
    QDir temporaryDirPath;
       
    dicomdirPath.insert( 0 , temporaryDirPath.tempPath() );
    dicomdirPath.append( "/DICOMDIR" );
    
    if ( temporaryDirPath.exists( dicomdirPath ) )
    {
        DeleteDirectory delDirectory;
        delDirectory.deleteDirectory( dicomdirPath , true);
    }
}

void QCreateDicomdir::databaseError( Status *state )
{
    QString text,code;

    if ( !state->good() )
    {
        switch( state->code() )
        {  case 2001 :  text.insert( 0, tr( "Database is corrupted or SQL syntax error" ) );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;
            case 2005 : text.insert( 0, tr( "Database is locked" ) );
                        text.append( "\n" );
                        text.append( "To solve this error restart the user session" );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;
            case 2011 : text.insert( 0, tr( "Database is corrupted." ) );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10);
                        text.append( code );
                        break;
            case 2019 : text.insert( 0, tr( "Register duplicated." ) );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;
            case 2050 : text.insert( 0, "Not Connected to database" );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;            
            default :   text.insert( 0, tr( "Internal Database error" ) );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
        }
        QMessageBox::critical( this , tr( "StarViewer" ) , text );
    }    
}

bool QCreateDicomdir::dicomdirPathIsADicomdir( QString dicomdirPath )
{
    QFile dicomdirFile;
    
    return dicomdirFile.exists( dicomdirPath + "/DICOMDIR" );
}

QCreateDicomdir::~QCreateDicomdir()
{
}

}
