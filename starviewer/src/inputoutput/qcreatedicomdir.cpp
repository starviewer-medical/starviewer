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
#include <QCloseEvent>
#include <QHeaderView>
#include <QSignalMapper>

#include "dicomstudy.h"
#include "converttodicomdir.h"
#include "status.h"
#include "logging.h"
#include "status.h"
#include "cacheimagedal.h"
#include "dicommask.h"
#include "harddiskinformation.h"
#include "deletedirectory.h"
#include "starviewersettings.h"

namespace udg {

QCreateDicomdir::QCreateDicomdir(QWidget *parent)
 : QDialog(parent)
{
    setupUi( this );

    QString sizeOfDicomdirText;

    m_dicomdirStudiesList->setColumnHidden( 7 , true );//Conte l'UID de l'estudi

    m_dicomdirSize = 0;
    setDicomdirSize();

    // crear les accions
    createActions();
    createConnections();

    //per defecte gravem al disc dur per tant, l'espai és il·limitat
    m_DiskSpace = ( unsigned long ) 9999999 * (unsigned long) ( 1024 * 1024 );

    setWidthColumns();
}

QCreateDicomdir::~QCreateDicomdir()
{
}

void QCreateDicomdir::createActions()
{
    m_signalMapper = new QSignalMapper;
    connect( m_signalMapper, SIGNAL( mapped(int) ), this , SLOT( deviceChanged(int) ) );

    m_cdromAction = new QAction(0);
    m_cdromAction->setText( tr("CD-ROM") );
    m_cdromAction->setStatusTip( tr("Record DICOMDIR on a CD-ROM device") );
    m_cdromAction->setIcon( QIcon(":/images/cdrom.png") );
    m_cdromAction->setCheckable( true );
    m_signalMapper->setMapping( m_cdromAction , CDROM );
    connect( m_cdromAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_cdromDeviceToolButton->setDefaultAction( m_cdromAction );

    m_dvdromAction = new QAction(0);
    m_dvdromAction->setText( tr("DVD-ROM") );
    m_dvdromAction->setStatusTip( tr("Record DICOMDIR on a DVD-ROM device") );
    m_dvdromAction->setIcon( QIcon(":/images/dvd.png") );
    m_dvdromAction->setCheckable( true );
    m_signalMapper->setMapping( m_dvdromAction , DVDROM );
    connect( m_dvdromAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_dvdromDeviceToolButton->setDefaultAction( m_dvdromAction );

    m_hardDiskAction = new QAction(0);
    m_hardDiskAction->setText( tr("Hard Disk") );
    m_hardDiskAction->setStatusTip( tr("Record DICOMDIR on a Hard Disk") );
    m_hardDiskAction->setIcon( QIcon(":/images/harddrive.png") );
    m_hardDiskAction->setCheckable( true );
    m_signalMapper->setMapping( m_hardDiskAction , HardDisk );
    connect( m_hardDiskAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_hardDiskDeviceToolButton->setDefaultAction( m_hardDiskAction );

    m_pendriveAction = new QAction(0);
    m_pendriveAction->setText( tr("Pen Drive") );
    m_pendriveAction->setStatusTip( tr("Record DICOMDIR on a USB Pen drive device") );
    m_pendriveAction->setIcon( QIcon(":/images/usbpendrive.png") );
    m_pendriveAction->setCheckable( true );
    m_signalMapper->setMapping( m_pendriveAction , PenDrive );
    connect( m_pendriveAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_pendriveDeviceToolButton->setDefaultAction( m_pendriveAction );

    m_devicesActionGroup = new QActionGroup( 0 );
    m_devicesActionGroup->setExclusive( true );
    m_devicesActionGroup->addAction( m_cdromAction );
    m_devicesActionGroup->addAction( m_dvdromAction );
    m_devicesActionGroup->addAction( m_hardDiskAction );
    m_devicesActionGroup->addAction( m_pendriveAction );

    // primer activem el CD com a dispositiu per defecte
    m_cdromAction->trigger();
}

void QCreateDicomdir::createConnections()
{
    connect( m_buttonRemove , SIGNAL( clicked() ) , this , SLOT( removeSelectedStudy() ) );
    connect( m_buttonRemoveAll , SIGNAL( clicked() ) , this , SLOT( removeAllStudies() ) );
    connect( m_buttonExamineDisk , SIGNAL( clicked() ) , this , SLOT( examineDicomdirPath() ) );
    connect( m_buttonCreateDicomdir , SIGNAL( clicked() ) , this , SLOT( createDicomdir() ) );
}

void QCreateDicomdir::setWidthColumns()
{
    StarviewerSettings settings;

    for ( int index = 0; index < m_dicomdirStudiesList->columnCount(); index++ )
    {   //Al haver un QSplitter el nom del Pare del TabCache és l'splitter
            m_dicomdirStudiesList->header()->resizeSection( index ,settings.getQCreateDicomdirColumnWidth( index ) );
    }
}

void QCreateDicomdir::setDicomdirSize()
{
    QString sizeOfDicomdirText, sizeText;
    float sizeInMb;

    sizeInMb = m_dicomdirSize / ( 1024 * 1024 );//passem a Mb
    sizeText.setNum( sizeInMb , 'f' , 2 );

    sizeOfDicomdirText = tr("DICOMDIR size: %1 Mb").arg(sizeText);
    m_dicomdirSizeOnDiskLabel->setText( sizeOfDicomdirText );

    if ( sizeInMb < m_progressBarOcupat->maximum() )
        m_progressBarOcupat->setValue( int( sizeInMb ) );
    else
        m_progressBarOcupat->setValue( m_progressBarOcupat->maximum() );

    sizeText.setNum( sizeInMb , 'f' , 0 );

    sizeOfDicomdirText = tr("%1 Mb").arg( sizeText );
    m_labelMbCdDvdOcupat->setText( sizeOfDicomdirText );
}

void QCreateDicomdir::addStudy( DICOMStudy study )
{
    CacheImageDAL cacheImageDAL;
    DicomMask imageMask;
    unsigned long studySize;
    Status state;

    if ( !existsStudy( study.getStudyUID() ) )
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
        if ( studySize + m_dicomdirSize > m_DiskSpace && (m_currentDevice == CDROM || m_currentDevice == DVDROM )  )
        {
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "With this study the DICOMDIR exceeds the maximum capacity of the selected device. Please change the selected device or create the DICOMDIR" ) );
        }
        else
        {   //afegim la informació de l'estudi a la llista
            QTreeWidgetItem* item = new QTreeWidgetItem( m_dicomdirStudiesList );
            m_dicomdirSize = m_dicomdirSize + studySize;
            setDicomdirSize();

            item->setText( 0 , study.getStudyId() );
            item->setText( 1 , study.getPatientId() );
            item->setText( 2 , study.getPatientName() );
            item->setText( 3 , study.getStudyModality() );
            item->setText( 4 , formatDate( study.getStudyDate() ) );
            item->setText( 5 , formatHour( study.getStudyTime() ) );
            item->setText( 6 , study.getStudyDescription() );
            item->setText( 7 , study.getStudyUID() );
        }
    }
    else
    {
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The study exists in the DICOMDIR list" ) );
    }
}

void QCreateDicomdir::createDicomdir()
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    Status state;

    switch( m_currentDevice )
    {
        case PenDrive:
        case HardDisk:
                 createDicomdirOnHardDiskOrFlashMemories();
                 break;
        case DVDROM:
        case CDROM: //cd, si s'ha creat bé, executem el programa per gravar el dicomdir a cd's
                 state = createDicomdirOnCdOrDvd();
                 //error 4001 és el cas en que alguna imatge de l'estudi no compleix amb l'estàndard dicom tot i així el deixem gravar
                 if ( state.good() || ( !state.good() && state.code() == 4001 ) )
                    burnDicomdir( recordDeviceDicomDir(cd) );
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

    logMessage = "Iniciant la creació del DICOMDIR en cd-dvd al directori temporal ";
    logMessage.append( dicomdirPath );
    INFO_LOG ( logMessage );

    if ( !temporaryDirPath.mkpath( dicomdirPath ) )//Creem el directori temporal
    {
        QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Can't create the temporary directory to create DICOMDIR. Please check users permission" ) );
        logMessage = "Error al crear directori ";
        logMessage.append( dicomdirPath );
        ERROR_LOG( logMessage );
        return state.setStatus( "Can't create temporary DICOMDIR", false , 3002 );
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
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Please enter a diretory to create de DICOMDIR" ) );
        return;
    }


    logMessage = "Iniciant la creació del DICOMDIR en discdur o usb al directori ";
    logMessage.append( dicomdirPath );
    INFO_LOG ( logMessage );

    if ( dicomdirPathIsADicomdir( dicomdirPath ) )
    {
        switch ( QMessageBox::question( this ,
                tr( "Create DICOMDIR" ) ,
                tr( "The directory contains a DICOMDIR, do you want to overwrite and delete all the files in the directory ?" ) ,
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
                        tr( "The DICOMDIR directory doesn't exists. Do you want to create it ?" ) ,
                        tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
                {
                    case 0:
                        if ( !directoryDicomdirPath.mkpath( dicomdirPath ) )
                        {
                            QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Can't create the directory. Please check users permission" ) );
                            logMessage = "Error al crear directori ";
                            logMessage.append( dicomdirPath );
                            ERROR_LOG( logMessage );
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

    INFO_LOG( logMessage );

    if ( !enoughFreeSpace( dicomdirPath ) )// comprovem si hi ha suficient espai lliure al disc dur
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Not enough free space to create DICOMDIR. Please free space" ) );

        logMessage = "Error al crear el DICOMDIR, no hi ha suficient espai al disc ERROR : ";
        logMessage.append( state.text() );
        ERROR_LOG( logMessage );
        return state.setStatus( "Not enough space to create DICOMDIR", false , 3000 );
    }

    QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( "*" , Qt::MatchWildcard, 0 ) );
    QTreeWidgetItem *item;

    if ( dicomdirStudiesList.count() == 0 ) //Comprovem que hi hagi estudis seleccionats per crear dicomdir
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Please, first select the studies which you want to create a DICOMDIR" ) );
        return state.setStatus( "No study selected to create DICOMDIR", false , 3001 );
    }

    for ( int i = 0; i < dicomdirStudiesList.count(); i++ )
    {
        item = dicomdirStudiesList.at( i );
        convertToDicomdir.addStudy( item->text( 7 ) ); // indiquem a la classe convertToDicomdir, quins estudis s'ha de convertir a dicomdir, passant el UID de l'estudi

        logMessage = "L'estudi ";
        logMessage.append( item->text( 7 ) );
        logMessage.append( " s'afegirà al DICOMDIR " );
        INFO_LOG( logMessage );
    }

    switch( m_currentDevice )
    {
        case HardDisk: //disc dur o dispositiu extrable
                 state = convertToDicomdir.convert( dicomdirPath, recordDeviceDicomDir( harddisk ) );
                 break;
        case PenDrive: //usb o memòria flash
                 state = convertToDicomdir.convert( dicomdirPath, recordDeviceDicomDir( usb ) );
                 break;
        case CDROM: //cd
                 state = convertToDicomdir.convert( dicomdirPath, recordDeviceDicomDir( cd ) );
                 break;
        case DVDROM: //dvd
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
            QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Error creating DICOMDIR. Be sure you have user permissions in " ) + m_lineEditDicomdirPath->text() + " and that the directory is empty " );
            logMessage = "Error al crear el DICOMDIR ERROR : ";
            logMessage.append( state.text() );
            ERROR_LOG( logMessage );

            return state;
        }
    }

    if ( m_currentDevice == CDROM || m_currentDevice == DVDROM )
        convertToDicomdir.createReadmeTxt();

    INFO_LOG( "Finalitzada la creació del DICOMDIR" );
    clearQCreateDicomdirScreen();

    return state;
}

void QCreateDicomdir::clearQCreateDicomdirScreen()
{
    m_dicomdirStudiesList->clear();
    m_lineEditDicomdirPath->setText( "" );

    m_dicomdirSizeOnDiskLabel->setText( tr( "The size of DICOMDIR is 0 Mb" ) );

    m_dicomdirSize = 0;
    setDicomdirSize();//Reiniciem la barra de progrés
}

void QCreateDicomdir::examineDicomdirPath()
{
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./" , tr( "DICOMDIR Directory" ) );
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
    // TODO perquè es fa un segon setDicomdirSize?
    setDicomdirSize();
}

void QCreateDicomdir::removeSelectedStudy()
{
    DicomMask imageMask;
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
            imageMask.setStudyUID( selectedStudies.at( i )->text( 7 ) );

            state = cacheImageDAL.imageSize( imageMask , studySize );

            if ( !state.good() )
            {
                databaseError ( &state );
                return;
            }

            m_dicomdirSize = m_dicomdirSize - studySize;
            setDicomdirSize();

            delete selectedStudies.at( i );
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
    QProgressDialog *progressBar = new QProgressDialog( tr( "Creating DICOMDIR Image..." ) , "" , 0 , 10 );;
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
        default:
            break;
    }

    progressBar->close();
}

bool QCreateDicomdir::enoughFreeSpace( QString path )
{
    HardDiskInformation hardDisk;

    if ( hardDisk.getNumberOfFreeMBytes( path ) < static_cast<quint64> (m_dicomdirSize / ( 1024 * 1204 ) ) )
    {
        return false;
    }
    else return true;
}

QString QCreateDicomdir::formatDate( const QString date )
{
    QString formateDate , originalDate ( date );

    formateDate.insert( 0 , originalDate.mid( 6 , 2 ) ); //dd
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 4 , 2 ) );
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 0 , 4 ) );

    return formateDate;
}

QString QCreateDicomdir::formatHour( const QString hour )
{
    QString formatedHour,originalHour( hour );

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

void QCreateDicomdir::saveColumnsWidth()
{
    StarviewerSettings settings;
    for ( int i = 0; i < m_dicomdirStudiesList->columnCount(); i++ )
    {
        settings.setQCreateDicomdirColumnWidth( i , m_dicomdirStudiesList->columnWidth( i ) );
    }
}

void QCreateDicomdir::closeEvent( QCloseEvent* ce )
{
    saveColumnsWidth();

    ce->accept();
}

void QCreateDicomdir::deviceChanged( int index )
{
    float sizeInMB = (m_dicomdirSize / ( 1024 * 1024 ) );

    m_currentDevice = index;
    switch( m_currentDevice )
    {
        case PenDrive:
        case HardDisk:
            m_stackedWidget->setCurrentIndex(1);
            // per gravar al disc no hi ha màxim TODO això no es del tot cert, caldria comprovar l'espai de disc
            m_DiskSpace = ( unsigned long ) 9999999 * ( unsigned long ) ( 1024 * 1024 );
            break;
        case CDROM:
                m_stackedWidget->setCurrentIndex(0);
                if( sizeInMB < 700 )
                {
                    m_progressBarOcupat->setMaximum( 700 );
                    m_DiskSpace = ( unsigned long ) 700 * ( unsigned long ) ( 1024 * 1024 ); // convertim a bytes capacaticat cd
                    m_progressBarOcupat->repaint();
                }
                else
                {
                    QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The selected device doesn't have enough space to copy all this studies, please remove some studies. The capacity of a cd is 700 Mb" ) );
                }
                break;
        case DVDROM:
                m_stackedWidget->setCurrentIndex(0);
                if( sizeInMB < 4400 )
                {
                    m_progressBarOcupat->setMaximum( 4400 );
                    m_DiskSpace = ( unsigned long ) 4400 * ( unsigned long ) ( 1024 * 1024 ); //convertim a bytes capacitat dvd
                    m_progressBarOcupat->repaint();
                }
                else
                {
                    QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The selected device doesn't have enough space to copy all this studies, please remove some studies. The capacity of a dvd is 4400 Mb" ) );
                }
                break;
    }

    if ( sizeInMB < m_progressBarOcupat->maximum() )
        m_progressBarOcupat->setValue( int( sizeInMB ) );
    else
        m_progressBarOcupat->setValue( m_progressBarOcupat->maximum() );

}

}
