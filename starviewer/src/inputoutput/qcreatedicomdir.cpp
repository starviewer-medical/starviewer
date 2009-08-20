/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcreatedicomdir.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QProcess>
#include <QCloseEvent>
#include <QSignalMapper>

#include "converttodicomdir.h"
#include "status.h"
#include "logging.h"
#include "status.h"
#include "harddiskinformation.h"
#include "deletedirectory.h"
#include "starviewerapplication.h"
#include "study.h"
#include "patient.h"
#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "isoimagefilecreator.h"
#include "dicommask.h"
#include "image.h"
#include "dicomdirburningapplication.h"

namespace udg {

QCreateDicomdir::QCreateDicomdir(QWidget *parent)
 : QDialog(parent)
{
    setupUi( this );
    setWindowFlags( this->windowFlags() ^ Qt::WindowContextHelpButtonHint );
    QString sizeOfDicomdirText;

    m_dicomdirStudiesList->setColumnHidden( 7 , true );//Conte l'UID de l'estudi

    m_dicomdirSizeBytes = 0;
    setDicomdirSize();

    // Crear les accions
    createActions();
    createConnections();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::CreateDicomdirStudyListColumnsWidth,m_dicomdirStudiesList);

    // Per defecte creem els dicomdir al discdur
    m_hardDiskAction->trigger();
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
    m_signalMapper->setMapping( m_cdromAction , CreateDicomdir::CdRom );
    connect( m_cdromAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_cdromDeviceToolButton->setDefaultAction( m_cdromAction );

    m_dvdromAction = new QAction(0);
    m_dvdromAction->setText( tr("DVD-ROM") );
    m_dvdromAction->setStatusTip( tr("Record DICOMDIR on a DVD-ROM device") );
    m_dvdromAction->setIcon( QIcon(":/images/dvd.png") );
    m_dvdromAction->setCheckable( true );
    m_signalMapper->setMapping( m_dvdromAction , CreateDicomdir::DvdRom );
    connect( m_dvdromAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_dvdromDeviceToolButton->setDefaultAction( m_dvdromAction );

    m_hardDiskAction = new QAction(0);
    m_hardDiskAction->setText( tr("Hard Disk") );
    m_hardDiskAction->setStatusTip( tr("Record DICOMDIR on a Hard Disk") );
    m_hardDiskAction->setIcon( QIcon(":/images/harddrive.png") );
    m_hardDiskAction->setCheckable( true );
    m_signalMapper->setMapping( m_hardDiskAction , CreateDicomdir::HardDisk );
    connect( m_hardDiskAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_hardDiskDeviceToolButton->setDefaultAction( m_hardDiskAction );

    m_pendriveAction = new QAction(0);
    m_pendriveAction->setText( tr("Pen Drive") );
    m_pendriveAction->setStatusTip( tr("Record DICOMDIR on a USB Pen drive device") );
    m_pendriveAction->setIcon( QIcon(":/images/usbpendrive.png") );
    m_pendriveAction->setCheckable( true );
    m_signalMapper->setMapping( m_pendriveAction , CreateDicomdir::UsbPen );
    connect( m_pendriveAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_pendriveDeviceToolButton->setDefaultAction( m_pendriveAction );

    m_devicesActionGroup = new QActionGroup( 0 );
    m_devicesActionGroup->setExclusive( true );
    m_devicesActionGroup->addAction( m_cdromAction );
    m_devicesActionGroup->addAction( m_dvdromAction );
    m_devicesActionGroup->addAction( m_hardDiskAction );
    m_devicesActionGroup->addAction( m_pendriveAction );
}

void QCreateDicomdir::createConnections()
{
    connect( m_buttonRemove , SIGNAL( clicked() ) , this , SLOT( removeSelectedStudy() ) );
    connect( m_buttonRemoveAll , SIGNAL( clicked() ) , this , SLOT( removeAllStudies() ) );
    connect( m_buttonExamineDisk , SIGNAL( clicked() ) , this , SLOT( examineDicomdirPath() ) );
    connect( m_buttonCreateDicomdir , SIGNAL( clicked() ) , this , SLOT( createDicomdir() ) );
}

//TODO: De manera temporal no es mostrarà la mida del dicomdir, degut a que les imatges descarregades amb la transfer synstax de JpegLossLess s'han de passar a LittleEndian i ocupen més espai, com ara de moment no està implementat un sistema per calcular que ocuparan les imatges a LittleEndian de moment no ho calculem
void QCreateDicomdir::setDicomdirSize()
{
    QString sizeOfDicomdirText, sizeText;
    double sizeInMb;

    sizeInMb = m_dicomdirSizeBytes / ( 1024 * 1024 );//passem a Mb
    sizeText.setNum( sizeInMb , 'f' , 2 );

    sizeOfDicomdirText = tr("DICOMDIR size: %1 Mb").arg(sizeText);
    m_dicomdirSizeOnDiskLabel->setText( sizeOfDicomdirText );

    if ( sizeInMb < m_progressBarOcupat->maximum() )
        m_progressBarOcupat->setValue( QString::number(sizeInMb).toInt() );
    else
        m_progressBarOcupat->setValue( m_progressBarOcupat->maximum() );

    sizeText.setNum( sizeInMb , 'f' , 0 );

    sizeOfDicomdirText = tr("%1 Mb").arg( sizeText );
    m_labelMbCdDvdOcupat->setText( sizeOfDicomdirText );
}

void QCreateDicomdir::addStudy(Study *study)
{
    qint64 studySizeBytes;
    Status state;

    if ( !studyExists( study->getInstanceUID() ) )
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

        // \TODO Xapussa perquè ara, a primera instància, continui funcionant amb les classes Study i demés. Caldria unificar el tema
        // "a quin directori està aquest study"?
        studySizeBytes = getStudySizeInBytes(false, study->getInstanceUID());

        //només comprovem l'espai si gravem a un cd o dvd
        if ( ( (studySizeBytes + m_dicomdirSizeBytes)  > m_DiskSpaceBytes) && (m_currentDevice == CreateDicomdir::CdRom || m_currentDevice == CreateDicomdir::DvdRom )  )
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning( this , ApplicationNameString , tr( "The study can be adde to Dicomdir list, with this study the DICOMDIR exceeds the maximum capacity of the selected device. Please change the selected device or create the DICOMDIR" ) );
        }
        else
        {  //Afegim la informació de l'estudi a la llista
            QTreeWidgetItem* item = new QTreeWidgetItem( m_dicomdirStudiesList );
            m_dicomdirSizeBytes = m_dicomdirSizeBytes + studySizeBytes;
            setDicomdirSize();
        
            Patient *patient = study->getParentPatient();
            item->setText( 0, patient->getFullName() );
            item->setText( 1, patient->getID() );
            item->setText( 2, study->getPatientAge() );
            item->setText( 3, study->getDescription() );
            item->setText( 4, study->getModalitiesAsSingleString() );
            item->setText( 5, study->getDate().toString(Qt::ISODate) );
            item->setText( 6, study->getTime().toString(Qt::ISODate) );
            item->setText( 7, study->getInstanceUID() );

            QApplication::restoreOverrideCursor();
        }
    }
    else QMessageBox::warning( this , ApplicationNameString , tr( "The study already exists in the DICOMDIR list" ) );
}

void QCreateDicomdir::createDicomdir()
{
    Status state;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    switch( m_currentDevice )
    {
        case CreateDicomdir::UsbPen:
        case CreateDicomdir::HardDisk:
                createDicomdirOnHardDiskOrFlashMemories();
                break;
        case CreateDicomdir::DvdRom:
        case CreateDicomdir::CdRom: // Cd, si s'ha creat bé, executem el programa per gravar el dicomdir a cd's
                 state = createDicomdirOnCdOrDvd();
                 // Error 4001 és el cas en que alguna imatge de l'estudi no compleix amb l'estàndard dicom tot i així el deixem gravar
                 if ( state.good() || ( !state.good() && state.code() == 4001 ) )
                    burnDicomdir( m_currentDevice );
                 break;
    }
    QApplication::restoreOverrideCursor();
}

Status QCreateDicomdir::createDicomdirOnCdOrDvd()
{
    QDir temporaryDirPath;
    QString dicomdirPath;
    Status state;

    // Per la norma del IHE el dicomdir ha d'estar situat dins el directori DICOMDIR
    dicomdirPath = QDir::tempPath() + "/DICOMDIR";
    // Si el directori dicomdir ja existeix al temporal l'esborrem
    if ( temporaryDirPath.exists( dicomdirPath ) )
    {
        DeleteDirectory delDirectory;
        delDirectory.deleteDirectory( dicomdirPath , true );
    }

    INFO_LOG( "Iniciant la creació del DICOMDIR en cd-dvd al directori temporal " + dicomdirPath );

    if ( !temporaryDirPath.mkpath( dicomdirPath ) ) // Creem el directori temporal
    {
        QMessageBox::critical( this , ApplicationNameString , tr( "Can't create the temporary directory to create DICOMDIR. Please check users permission" ) );
        ERROR_LOG( "Error al crear directori " + dicomdirPath );
        return state.setStatus( "Can't create temporary DICOMDIR", false , 3002 );
    }
    else
    {
        return startCreateDicomdir( dicomdirPath );
    }
}

void QCreateDicomdir::createDicomdirOnHardDiskOrFlashMemories()
{
    QString dicomdirPath = m_lineEditDicomdirPath->text();
    DeleteDirectory delDirectory;
    QDir directoryDicomdirPath( dicomdirPath );

    // Comprovem si el directori ja es un dicomdir, si és el cas demanem a l'usuari si el desitja sobreecriue o, els estudis seleccionats s'afegiran ja al dicomdir existent

    if ( m_lineEditDicomdirPath->text().length() == 0 )
    {
        QMessageBox::information( this , ApplicationNameString , tr( "No directory specified to create the DICOMDIR" ) );
        return;
    }

    INFO_LOG ( "Iniciant la creació del DICOMDIR en discdur o usb al directori " + dicomdirPath );

    if ( dicomdirPathIsADicomdir( dicomdirPath ) )
    {
        switch ( QMessageBox::question( this ,
                tr( "Create DICOMDIR" ) ,
                tr( "The directory contains a DICOMDIR, do you want to overwrite and delete all the files in the directory ?" ) ,
                tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
        {
            case 0: // Si vol sobreescriure, esborrem el contingut del directori
                delDirectory.deleteDirectory( dicomdirPath , false );
                break;
            case 1:
                INFO_LOG( "El directori no està buit, i l'usuari no dona permís per esborrar el seu contingut") ;
                return; // No fem res, l'usuari no vol sobreescriure el directori, cancel·lem l'operacio i tornem el control a l'usuari
                break;
        }
    }
    else if (!dicomdirPathIsEmpty(dicomdirPath))
    {
        QMessageBox::information(this, ApplicationNameString, tr("The destination directory is not empty, please choose an empty directory."));
        return;
    }
    else
    {   // El directori no és un dicomdir
        if ( !directoryDicomdirPath.exists() ) // Si el directori no existiex, preguntem si el vol crear
        {
                switch ( QMessageBox::question( this ,
                        tr( "Create directory ?" ) ,
                        tr( "The DICOMDIR directory doesn't exists. Do you want to create it ?" ) ,
                        tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
                {
                    case 0:
                        if ( !directoryDicomdirPath.mkpath( dicomdirPath ) )
                        {
                            QMessageBox::critical( this , ApplicationNameString , tr( "Can't create the directory. Please check users permissions." ) );
                            ERROR_LOG( "Error al crear directori " + dicomdirPath );
                        }
                        break;
                    case 1:
                        INFO_LOG( "El directori especificat per l'usuari no existeix, i no el vol crear per tant cancel·lem la creació del DICOMDIR" );
                        return; // Cancel·lem;
                        break;
                }
        }
    }

    startCreateDicomdir( dicomdirPath );

    // Guardem la ruta de l'ultim directori on l'usuari ha creat el dicomdir
    m_lastDicomdirDirectory = dicomdirPath;
}

Status QCreateDicomdir::startCreateDicomdir( QString dicomdirPath )
{
    ConvertToDicomdir convertToDicomdir;
    Status state;

    if ( !enoughFreeSpace( dicomdirPath ) )// Comprovem si hi ha suficient espai lliure al disc dur
    {
        QMessageBox::information( this , ApplicationNameString , tr( "Not enough free space to create DICOMDIR. Please free space." ) );
        ERROR_LOG( "Error al crear el DICOMDIR, no hi ha suficient espai al disc ERROR : " + state.text() );
        return state.setStatus( "Not enough space to create DICOMDIR", false , 3000 );
    }

    QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( "*" , Qt::MatchWildcard, 0 ) );
    QTreeWidgetItem *item;

    if ( dicomdirStudiesList.count() == 0 ) // Comprovem que hi hagi estudis seleccionats per crear dicomdir
    {
        QMessageBox::information( this , ApplicationNameString , tr( "Please, first select the studies you want to create a DICOMDIR." ) );
        return state.setStatus( "No study selected to create the DICOMDIR", false , 3001 );
    }

    for ( int i = 0; i < dicomdirStudiesList.count(); i++ )
    {
        item = dicomdirStudiesList.at( i );
        convertToDicomdir.addStudy( item->text( 7 ) ); // Indiquem a la classe convertToDicomdir, quins estudis s'ha de convertir a dicomdir, passant el UID de l'estudi
        INFO_LOG( "L'estudi " + item->text( 7 ) + " s'afegirà al DICOMDIR " );
    }

    state = convertToDicomdir.convert(dicomdirPath, m_currentDevice);

    if ( !state.good() )
    {
        if ( state.code() == 4001 ) // Alguna de les imatges no compleix l'estandard dicom però es pot continuar endavant
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::information( this , ApplicationNameString, tr( "Some images are not 100 % DICOM compliant. It could be possible that some viewers have problems to visualize them." ) );
            QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
        }
        else
        {
            QMessageBox::critical( this , ApplicationNameString , tr( "Error creating DICOMDIR. Be sure you have user permissions in %1 and the directory is empty." ).arg( m_lineEditDicomdirPath->text() ) );
            ERROR_LOG( "Error al crear el DICOMDIR ERROR : " + state.text() );
            return state;
        }
    }

    if ( m_currentDevice == CreateDicomdir::CdRom || m_currentDevice == CreateDicomdir::DvdRom )
        convertToDicomdir.createReadmeTxt();

    INFO_LOG( "Finalitzada la creació del DICOMDIR" );
    clearQCreateDicomdirScreen();

    return state;
}

void QCreateDicomdir::clearQCreateDicomdirScreen()
{
    m_dicomdirStudiesList->clear();
    m_lineEditDicomdirPath->clear();

    m_dicomdirSizeBytes = 0;
    setDicomdirSize();// Reiniciem la barra de progrés
}

void QCreateDicomdir::examineDicomdirPath()
{
    QString initialDirectory;
    QDir dicomdirPath;

    if ( !m_lineEditDicomdirPath->text().isEmpty() )//Si hi ha entrat un directori
    {
        if ( dicomdirPath.exists( m_lineEditDicomdirPath->text() ) )//si el directori existeix, serà el directori inicial al obrir
        {
            initialDirectory = m_lineEditDicomdirPath->text();
        }// Si no existeix directori entrat el directori inicial serà el home
        else initialDirectory = QDir::homePath();
    }
    else
    {
        if ( m_lastDicomdirDirectory.isEmpty() )//si no tenim last directory anem al directori home
        {
            initialDirectory = QDir::homePath();
        }
        else 
        {
            dicomdirPath.setPath( m_lastDicomdirDirectory );
            dicomdirPath.cdUp();
            initialDirectory = dicomdirPath.path();
        }
    }

    QString path = QFileDialog::getExistingDirectory( this, tr( "Choose an empty directory..." ), initialDirectory );
    if( !path.isEmpty() )
        m_lineEditDicomdirPath->setText(QDir::toNativeSeparators(path));
}

void QCreateDicomdir::removeAllStudies()
{
    m_dicomdirSizeBytes = 0;
    setDicomdirSize();
    m_dicomdirStudiesList->clear();
    // TODO perquè es fa un segon setDicomdirSize?
    setDicomdirSize();
}

void QCreateDicomdir::removeSelectedStudy()
{
    if (m_dicomdirStudiesList->selectedItems().count() != 0)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        foreach(QTreeWidgetItem *selectedStudy, m_dicomdirStudiesList->selectedItems())
        {
            m_dicomdirSizeBytes -= getStudySizeInBytes(false, selectedStudy->text(7));//La columna 7 de m_dicomdirStudiesList conté Study Instance UID
            setDicomdirSize();

            delete selectedStudy;
        }
        QApplication::restoreOverrideCursor();
    }
    else QMessageBox::information(this, ApplicationNameString, tr("Please select a study to remove of the list."));
}

bool QCreateDicomdir::studyExists( QString studyUID )
{
    QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( studyUID , Qt::MatchExactly, 7 ) );

    if ( dicomdirStudiesList.count() > 0 )
    {
        return true;
    }
    else return false;
}

void QCreateDicomdir::burnDicomdir( CreateDicomdir::recordDeviceDicomDir device )
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    
    // Com que de moment no hi ha comunicacio amb el mkisofs es crea aquest progress bar per donar algo de feeling a l'usuari, per a que no es pensi que s'ha penjat l'aplicaciÃ³
    QProgressDialog *progressBar = new QProgressDialog( QObject::tr( "Creating DICOMDIR Image..." ), "", 0, 10 );
    progressBar->setMinimumDuration( 0 );
    progressBar->setCancelButton( 0 );
    progressBar->setValue( 7 );

    // TODO comprovar primer si el device que ens passen és un CD o DVD, si no no pot funcionar
    QString dicomdirPath, isoPath;

    // Indiquem al directori del qual volem generar una imatge
    dicomdirPath = QDir::tempPath() + "/DICOMDIR";
    // Indiquem al directori i nom de la imatge a crear
    isoPath = dicomdirPath + "/dicomdir.iso";

    IsoImageFileCreator isoImageFileCreator;
    isoImageFileCreator.setInputPath(dicomdirPath);
    isoImageFileCreator.setOutputIsoImageFilePath(isoPath);
    isoImageFileCreator.setIsoImageLabel(QString("%1 DICOMDIR").arg(ApplicationNameString));

    if( isoImageFileCreator.createIsoImageFile() ) // Hi ha hagut problemes al crear la imatge ISO
    {
        DICOMDIRBurningApplication dicomdirBurningApplication;
        dicomdirBurningApplication.setCurrentDevice(device);
        dicomdirBurningApplication.setIsoPath(isoPath);
        
        if( !dicomdirBurningApplication.burnIsoImageFile() )
        {
            QMessageBox::critical(this, tr("DICOMDIR Burning Failure"), tr("There was an error during the burning of the ISO image file. ") + dicomdirBurningApplication.getLastErrorDescription() + "\n\n" + tr("Please, contact your system administrator to solve this problem.") );
            ERROR_LOG( "Error al gravar la imatge ISO amb descripció: " + dicomdirBurningApplication.getLastErrorDescription() );
        }
    }
    else
    {
        QMessageBox::critical(this, tr("DICOMDIR Creation Failure"), tr("There was an error during the creation of the ISO image file. ") + isoImageFileCreator.getLastErrorDescription() + "\n\n" + tr("Please, contact your system administrator to solve this problem.") );
        ERROR_LOG( "Error al crear ISO amb descripció: " + isoImageFileCreator.getLastErrorDescription() );
    }
    progressBar->close();
    QApplication::restoreOverrideCursor();
}

void QCreateDicomdir::showProcessErrorMessage( const QProcess &process, QString name )
{
    QString errorMessage;
    switch( process.error() )
    {
        case QProcess::FailedToStart:
            errorMessage = tr("The process [ %1 ] failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.").arg(name);
            break;

        case QProcess::Crashed:
            errorMessage = tr("The process [ %1 ] crashed some time after starting successfully.").arg(name);
            break;

//             case QProcess::Timedout:
//                 errorMessage = tr("The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.").arg(name);
//                 break;

            case QProcess::WriteError:
                errorMessage = tr("An error occurred when attempting to write to the process [ %1 ]. For example, the process may not be running, or it may have closed its input channel.").arg(name);
                break;

            case QProcess::ReadError:
                errorMessage = tr("An error occurred when attempting to read from the process [ %1 ]. For example, the process may not be running.").arg(name);
                break;

//             case QProcess::UnknownError:
//                 errorMessage = tr("An unknown error occurred with the process [ %1 ]").arg("mkisofs");
//                 break;
                
            default: 
                break;
    }
    QMessageBox::critical(this, tr("DICOMDIR Creation Failure"), tr("There was an error during the creation of the DICOMDIR") + "\n\n" + errorMessage + "\n\n" + tr("Please, contact your system administrator to solve this problem.") );

}

bool QCreateDicomdir::enoughFreeSpace( QString path )
{
    HardDiskInformation hardDisk;

    if ( hardDisk.getNumberOfFreeMBytes( path ) < static_cast<quint64> (m_dicomdirSizeBytes / ( 1024 * 1204 ) ) )
    {
        return false;
    }
    else return true;
}

void QCreateDicomdir::clearTemporaryDir()
{
    QString dicomdirPath, logMessage;
    QDir temporaryDirPath;

    dicomdirPath = QDir::tempPath() + "/DICOMDIR";

    if ( temporaryDirPath.exists( dicomdirPath ) )
    {
        DeleteDirectory delDirectory;
        delDirectory.deleteDirectory( dicomdirPath , true );
    }
}

void QCreateDicomdir::showDatabaseErrorMessage( const Status &state )
{
    if( !state.good() )
    {
        QMessageBox::critical( this , ApplicationNameString , state.text() + tr("\nError Number: %1").arg(state.code()) );
    }
}

bool QCreateDicomdir::dicomdirPathIsEmpty(QString dicomdirPath)
{
    QDir dir(dicomdirPath);

    return dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).count() == 0;//llista de fitxers del directori
}

bool QCreateDicomdir::dicomdirPathIsADicomdir(QString dicomdirPath)
{
    QDir dir(dicomdirPath);
    QStringList fileList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot); 

    //És un directori dicomdir creat per l'starviewer quan només conté un fitxer anomenat DICOMDIR i un directori anomenat DICOM
    if (fileList.count() == 1)
    {
        if (fileList.contains("DICOM", Qt::CaseInsensitive)) 
        {
            //si conté directori anomenat dicom
            fileList = dir.entryList(QDir::Files);
            if (fileList.count() == 1) 
            {
                //si conté un fitxer anomenat dicomdir
                return fileList.contains("DICOMDIR", Qt::CaseInsensitive);
            }
            else return false;
        }
        else return false;
    }
    else return false;
}

void QCreateDicomdir::closeEvent( QCloseEvent* ce )
{
    Settings settings;
    settings.saveColumnsWidths( InputOutputSettings::CreateDicomdirStudyListColumnsWidth, m_dicomdirStudiesList );
    ce->accept();
}

void QCreateDicomdir::deviceChanged( int index )
{
    float sizeInMB = (m_dicomdirSizeBytes / ( 1024 * 1024 ) );

    m_currentDevice = (CreateDicomdir::recordDeviceDicomDir) index;
    switch( m_currentDevice )
    {
        case CreateDicomdir::UsbPen:
        case CreateDicomdir::HardDisk:
            m_stackedWidget->setCurrentIndex(1);
            // per gravar al disc no hi ha màxim TODO això no es del tot cert, caldria comprovar l'espai de disc
            m_DiskSpaceBytes = HardDiskSizeBytes;
            break;
        case CreateDicomdir::CdRom:
        case CreateDicomdir::DvdRom:
                checkDICOMDIRBurningApplicationConfiguration();
                m_stackedWidget->setCurrentIndex(0);
                int maximumCapacity;

                if (m_currentDevice == CreateDicomdir::CdRom) 
                {
                    maximumCapacity = CDRomSizeMb;
                }
                else
                {
                    maximumCapacity = DVDRomSizeMb;
                }

                if( sizeInMB < maximumCapacity )
                {
                    m_progressBarOcupat->setMaximum( maximumCapacity );
                    m_DiskSpaceBytes = maximumCapacity; // convertim a bytes capacaticat cd/dvd
                    m_progressBarOcupat->repaint();
              }
                else
                {
                    QMessageBox::warning( this , ApplicationNameString , tr( "The selected device doesn't have enough space to copy all this studies, please remove some studies. The capacity of a cd is %1 Mb" ).arg(maximumCapacity) );
                }
            break;
    }

    if ( sizeInMB < m_progressBarOcupat->maximum() )
        m_progressBarOcupat->setValue( int( sizeInMB ) );
    else
        m_progressBarOcupat->setValue( m_progressBarOcupat->maximum() );
}

void QCreateDicomdir::dvdCdDicomdirDesactivatedOnWindows()
{
    QMessageBox::information(this, ApplicationNameString, 
                             tr("The creation of dicomdir on cd/dvd in Windows is desactivated.\n\n"
                                "To create a cd/dvd with a Dicomdir, you have to create first the Dicomdir on your hard disk and "
                                "then copy the directory where you have created it to a cd/dvd using a burning cd/dvd software."));

    //Marquem la opció de crear el dicomdir al disc dur
    m_hardDiskAction->trigger();
}

void QCreateDicomdir::checkDICOMDIRBurningApplicationConfiguration()
{
    Settings settings;
    if( !QFile::exists((settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey)).toString()) )
    {
        QMessageBox::warning( this, ApplicationNameString, 
                                tr( "Invalid DICOMDIR burning application configuration.\n" 
                                    "It can be solved in Tools -> Configuration -> DICOMDIR menu.") );
    
        //Marquem la opció de crear el dicomdir al disc dur
        m_hardDiskAction->trigger();
    }
}

quint64 QCreateDicomdir::getStudySizeInBytes(bool transferSyntaxInLittleEndian, QString studyInstanceUID)
{
    LocalDatabaseManager localDatabaseManager;

    if (transferSyntaxInLittleEndian)
    {
        //Si les imatges es transformaran a transfer syntax LittleEndian fem un càlcul aproximat del que ocuparà el dicomdir
        DicomMask imageMask;
        quint64 studySizeInLittleEndianTransferSyntax = 0;
        
        imageMask.setStudyUID(studyInstanceUID);

        QList<Image*> imagesOfStudy = localDatabaseManager.queryImage(imageMask);//Agafem les imatges de l'estudi per fer l'estimació del que ocuparà el dicomdir

        foreach(Image *image, imagesOfStudy)
        {
            studySizeInLittleEndianTransferSyntax += getImageSizeInBytesInLittleEndianTransferSyntax(image); 
        }
        
        return studySizeInLittleEndianTransferSyntax;
    }
    else
    {
        return HardDiskInformation::getDirectorySizeInBytes(localDatabaseManager.getStudyPath(studyInstanceUID));
    }
}

quint64 QCreateDicomdir::getImageSizeInBytesInLittleEndianTransferSyntax(Image *image)
{
    /*Per calcular la mida que ocupa el pixel Data un estudi en LittleEndian és  
        size = numberOfFrames * (bits-allocated / 8) * rows * columns * samples-per-pixel;
        size += size % 2;
      Llavors hem d'afegir la capçalera que és frame count * dicom header size */

    quint64 imageSizeInBytesInLittleEndianTransferSyntax = 0;

    imageSizeInBytesInLittleEndianTransferSyntax = image->getNumberOfFrames() * ( image->getBitsAllocated() / 8) * image->getRows() * image->getColumns() * image->getSamplesPerPixel();
    imageSizeInBytesInLittleEndianTransferSyntax += imageSizeInBytesInLittleEndianTransferSyntax % 2;

    //afegim el tamany de la capçalera
    imageSizeInBytesInLittleEndianTransferSyntax += image->getNumberOfFrames() * dicomHeaderSizeBytes;

    return imageSizeInBytesInLittleEndianTransferSyntax;
}

}
