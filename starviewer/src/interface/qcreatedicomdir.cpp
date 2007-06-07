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
#include <QProgressDialog>

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
 : QDialog(parent)
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
        case 0  : //memòria usb o flash == 1
                 m_labelMbCdDvdOcupat->setVisible( false );
                 m_progressBarOcupat->setVisible( false );
                 m_labelSizeOfDicomdir->setVisible( true );
                 m_lineEditDicomdirPath->setVisible( true );
                 m_buttonExamineDisk->setVisible( true );
                 m_labelCreateDicomdir->setText( tr( "Create Dicomdir at"  ) );
                 m_DiskSpace = ( unsigned long ) 9999999 * ( unsigned long ) ( 1024 * 1024 );//per gravar al disc no hi ha màxim
                 break;
        case 1  : //memòria usb o flash == 1
                 m_labelMbCdDvdOcupat->setVisible( false );
                 m_progressBarOcupat->setVisible( false );
                 m_labelSizeOfDicomdir->setVisible( true );
                 m_lineEditDicomdirPath->setVisible( true );
                 m_buttonExamineDisk->setVisible( true );
                 m_labelCreateDicomdir->setText( tr( "Create Dicomdir at"  ) );
                 m_DiskSpace = ( unsigned long ) 9999999 * ( unsigned long ) ( 1024 * 1024 );//per gravar a usb o memòria flash, no hi ha màxim perqué no sabem l'usuari en quin dispositu ho vol gravar
                 break;
        case 2 : //cd
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
                    QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The selected device doesn't have enough space to copy all this studies, please remove some studies. The capacity of a cd is 700 Mb" ) );
                    m_comboBoxAction->setCurrentIndex( 0 );
                 }
                 break;
        case 3 : //dvd
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
                    QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The selected device doesn't have enough space to copy all this studies, please remove some studies. The capacity of a dvd is 4400 Mb" ) );
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
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "With this study the Dicomdir exceeds the maximum capacity of the selected device. Please change the selected device or create the Dicomdir" ) );
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
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The study exists in the Dicomdir list" ) );
    }
}

void QCreateDicomdir::createDicomdir()
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    Status state;

    switch( m_comboBoxAction->currentIndex() )
    {
        case 0 : //disc dur
                 createDicomdirOnHardDiskOrFlashMemories();
                 break;
        case 1 : //memòria usb o flash
                 createDicomdirOnHardDiskOrFlashMemories();
                 break;
        case 2 : //cd, si s'ha creat bé, executem el programa per gravar el dicomdir a cd's
                 state = createDicomdirOnCdOrDvd();
                 //error 4001 és el cas en que alguna imatge de l'estudi no compleix amb l'estàndard dicom tot i així el deixem gravar
                 if ( state.good() || ( !state.good() && state.code() == 4001 ) ) burnDicomdir( recordDeviceDicomDir(cd) );
                 break;
        case 3 : //dvd, si s'ha creat bé, executem el programa per gravar el dicomdir a dvd's
                 state = createDicomdirOnCdOrDvd();
                 //error 4001 és el cas en que alguna imatge de l'estudi no compleix amb l'estàndard dicom tot i així el deixem gravar
                 if ( state.good() || ( !state.good() && state.code() == 4001) ) burnDicomdir( recordDeviceDicomDir(dvd) );
                 break;
    }

    QApplication::restoreOverrideCursor();
}

Status QCreateDicomdir::createDicomdirOnCdOrDvd()
{
    QDir temporaryDirPath;
    QString dicomdirPath, logMessage;
    Status state;
    ConvertToDicomdir convertToDicomdir;

    dicomdirPath.insert( 0 , temporaryDirPath.tempPath() );
    dicomdirPath.append( "/DICOMDIR" ); // per la norma del IHE el dicomdir ha d'estar situat dins el directori DICOMDIR

    //si el directori dicomdir ja existeix al temporal l'esborrem
    if ( temporaryDirPath.exists( dicomdirPath ) )
    {
        DeleteDirectory delDirectory;
        delDirectory.deleteDirectory( dicomdirPath , true );
    }

    logMessage = "Iniciant la creació del Dicomdir en cd-dvd al directori temporal ";
    logMessage.append( dicomdirPath );
    INFO_LOG (  logMessage.toAscii().constData() );

    if ( !temporaryDirPath.mkpath( dicomdirPath ) )//Creem el directori temporal
    {
        QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Can't create the temporary directory to create Dicomdir. Please check users permission" ) );
        logMessage = "Error al crear directori ";
        logMessage.append( dicomdirPath );
        ERROR_LOG( logMessage.toAscii().constData() );
        return state.setStatus( "Can't create temporary Dicomdir", false , 3002 );
    }
    else
    {
        return startCreateDicomdir( dicomdirPath );
    }
}

void QCreateDicomdir::createDicomdirOnHardDiskOrFlashMemories()
{
    QString dicomdirPath = m_lineEditDicomdirPath->text() , logMessage;
    DeleteDirectory delDirectory;
    QDir directoryDicomdirPath( dicomdirPath );

    //Comprovem si el directori ja es un dicomdir, si és el cas demanem a l'usuari si el desitja sobreecriue o, els estudis seleccionats s'afegiran ja al dicomdir existent

    if ( m_lineEditDicomdirPath->text().length() == 0 )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Please enter a diretory to create de Dicomdir" ) );
        return;
    }


    logMessage = "Iniciant la creació del Dicomdir en discdur o usb al directori ";
    logMessage.append( dicomdirPath );
    INFO_LOG (  logMessage.toAscii().constData() );

    if ( dicomdirPathIsADicomdir( dicomdirPath ) )
    {
        switch ( QMessageBox::question( this ,
                tr( "Create Dicomdir" ) ,
                tr( "The directory contains a Dicomdir, do you want to overwrite and delete all the files in the directory ?" ) ,
                tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
        {
            case 0: // si vol sobreescriure, esborrem el contingut del directori
                delDirectory.deleteDirectory( dicomdirPath , false );
                break;
            case 1:
                INFO_LOG( "El directori no està buit, i l'usuari no dona permís per esborrar el seu contingut") ;
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
                            QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Can't create the directory. Please check users permission" ) );
                            logMessage = "Error al crear directori ";
                            logMessage.append( dicomdirPath );
                            ERROR_LOG( logMessage.toAscii().constData() );
                        }
                        break;
                    case 1:
                        INFO_LOG( "El directori especificat per l'usuari no existeix, i no el vol crear per tant cancel·lem la creació del DICOMDIR" );
                        return; //cancel·lem;
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
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Not enough free space to create Dicomdir. Please free space" ) );

        logMessage = "Error al crear el Dicomdir, no hi ha suficient espai al disc ERROR : ";
        logMessage.append( state.text().c_str() );
        ERROR_LOG ( logMessage.toAscii().constData() );
        return state.setStatus( "Not enough space to create Dicomdir", false , 3000 );
    }

    QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( "*" , Qt::MatchWildcard, 0 ) );
    QTreeWidgetItem *item;

    if ( dicomdirStudiesList.count() == 0 ) //Comprovem que hi hagi estudis seleccionats per crear dicomdir
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Please, first select the studies which you want to create a Dicomdir" ) );
        return state.setStatus( "No study selected to create Dicomdir", false , 3001 );
    }

    for ( int i = 0; i < dicomdirStudiesList.count(); i++ )
    {
        item = dicomdirStudiesList.at( i );
        convertToDicomdir.addStudy( item->text( 7 ) ); // indiquem a la classe convertToDicomdir, quins estudis s'ha de convertir a dicomdir, passant el UID de l'estudi

        logMessage = "L'estudi ";
        logMessage.append( item->text( 7 ) );
        logMessage.append( " s'afegirà al Dicomdir " );
        INFO_LOG ( logMessage.toAscii().constData() );
    }

    switch( m_comboBoxAction->currentIndex() )
    {
        case 0 : //disc dur o dispositiu extrable
                 state = convertToDicomdir.convert( dicomdirPath, recordDeviceDicomDir( harddisk ) );
                 break;
        case 1 : //usb o memòria flash
                 state = convertToDicomdir.convert( dicomdirPath, recordDeviceDicomDir( usb ) );
                 break;
        case 2 : //cd
                 state = convertToDicomdir.convert( dicomdirPath, recordDeviceDicomDir( cd ) );
                 break;
        case 3 : //dvd
                 state = convertToDicomdir.convert( dicomdirPath, recordDeviceDicomDir( dvd ) );
                 break;
    }

    if ( !state.good() )
    {
        if ( state.code() == 4001 ) //alguna de les imatges no compleix l'estandard dicom però es pot continuar endavant
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::information( this , tr( "Starviewer" ), tr( "Some images are not 100 % Dicom compliance. It can be possible that some viewers have problems to visualizate them " ) );
            QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
        }
        else
        {
            QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Error creating Dicomdir. Be sure you have user permissions in " ) + m_lineEditDicomdirPath->text() + " and that the directory is empty " );
            logMessage = "Error al crear el Dicomdir ERROR : ";
            logMessage.append( state.text().c_str() );
            ERROR_LOG ( logMessage.toAscii().constData() );

            return state;
        }
    }

    //Cas que sigui un cd o dvd li copiem el README.TXT
    if ( m_comboBoxAction->currentIndex() == 1 || m_comboBoxAction->currentIndex() == 2)
    {
        convertToDicomdir.createReadmeTxt();
    }

    INFO_LOG( "Finalitzada la creació del Dicomdir" );
    clearQCreateDicomdirScreen();

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
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Please select a study to remove of the list" ) );
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

void QCreateDicomdir::burnDicomdir( recordDeviceDicomDir device )
{
    QProcess k3b, mkisofs;
    QStringList k3bParamatersList, mkisofsParamaterList;
    QDir temporaryDirPath;
    QString dicomdirPath, outputIsoPathParameter, isoPath;

    //com que de moment no hi ha comunicacio amb el mkisofs es crea aquest progress bar per donar algo de feeling a l'usuari, per a que no es pensi que s'ha penjat l'aplicació
    QProgressDialog *progressBar = new QProgressDialog( tr( "Creating Dicomdir Image..." ) , "" , 0 , 10 );;
    progressBar->setMinimumDuration( 0 );
    progressBar->setCancelButton( 0 );
    progressBar->setValue( 7 );

    dicomdirPath = temporaryDirPath.tempPath() + "/DICOMDIR/";

    //indiquem al directori i nom de la imatge a crear
    isoPath = dicomdirPath;
    isoPath.append( "/dicomdir.iso" );

    //el mkisofs per indica el nom de la iso a crear se li ha de posar amb el parametre -o
    outputIsoPathParameter = "-o";
    outputIsoPathParameter.append( isoPath );

    mkisofsParamaterList.push_back( "-V STARVIEWER DICOMDIR" );//indiquem que el label de la imatge és STARVIEWER DICOMDIR
    mkisofsParamaterList.push_back( outputIsoPathParameter ); //nom i directori on guardarem la imatge
    mkisofsParamaterList.push_back( dicomdirPath );//path a convertir en iso

    mkisofs.execute( "mkisofs" , mkisofsParamaterList );//creem la imatge
    mkisofs.waitForFinished( -1 ); //esperem que s'hagi generat la imatge

    k3bParamatersList.push_back( "--nosplash" );//que no s'engegui l'splash del k3b

    switch( device )
    {
        case recordDeviceDicomDir(cd) :
                k3bParamatersList.push_back( "--cdimage" );
                k3bParamatersList.push_back( isoPath );
                k3b.execute( "k3b" , k3bParamatersList );
                break;
        case recordDeviceDicomDir(dvd):
                k3bParamatersList.push_back( "--dvdimage" );
                k3bParamatersList.push_back( isoPath );
                k3b.execute( "k3b" , k3bParamatersList );
                break;
    }

    progressBar->close();
}

bool QCreateDicomdir::enoughFreeSpace( QString path )
{
    HardDiskInformation hardDisk;

    if ( hardDisk.getNumberOfFreeMBytes( path.toAscii().constData() ) < static_cast<quint64> (m_dicomdirSize / ( 1024 * 1204 ) ) )
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
        QMessageBox::critical( this , tr( "Starviewer" ) , text );
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
