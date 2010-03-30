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
#include <QTimer>

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
#include "copydirectory.h"

namespace udg {

const int QCreateDicomdir::CDRomSizeMb = 700;
const int QCreateDicomdir::DVDRomSizeMb = 4800;
const quint64 QCreateDicomdir::CDRomSizeBytes = ( quint64 ) CDRomSizeMb * ( quint64 ) ( 1024 * 1024 );
const quint64 QCreateDicomdir::DVDRomSizeBytes = ( quint64 ) DVDRomSizeMb * ( quint64 ) ( 1024 * 1024 );
const int QCreateDicomdir::DicomHeaderSizeBytes = 23000;

QCreateDicomdir::QCreateDicomdir(QWidget *parent)
 : QDialog(parent)
{
    setupUi( this );
    setWindowFlags( this->windowFlags() ^ Qt::WindowContextHelpButtonHint );
    QString sizeOfDicomdirText;

    resetDICOMDIRList();

    // Crear les accions
    createActions();
    createConnections();

    initializeControls();
}

QCreateDicomdir::~QCreateDicomdir()
{
    clearTemporaryDICOMDIRPath();
}

void QCreateDicomdir::initializeControls()
{
    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::CreateDicomdirStudyListColumnsWidth,m_dicomdirStudiesList);

    m_dicomdirStudiesList->setColumnHidden( 7 , true );//Conte l'UID de l'estudi

    if (!settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString().isEmpty())
    {
        m_copyFolderContentToDICOMDIRCdDvdCheckBox->setEnabled(true);
        m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setEnabled(true);
        m_copyFolderContentToDICOMDIRCdDvdCheckBox->setText( tr("Copy the content of \"%1\" to DICOMDIR.").arg( QDir::toNativeSeparators( settings.getValue( InputOutputSettings::DICOMDIRFolderPathToCopy).toString() ) ) );
        m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setText( tr("Copy the content of \"%1\" to DICOMDIR.").arg( QDir::toNativeSeparators( settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy ).toString() ) ) );
        m_copyFolderContentToDICOMDIRCdDvdCheckBox->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIRCdDvd).toBool());
        m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIRUsbHardDisk).toBool());

    }
    else
    {
        //Si no ens han especificat Path a copiar descativem els checkbox
        m_copyFolderContentToDICOMDIRCdDvdCheckBox->setEnabled(false);
        m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->setEnabled(false);
    }

    // Per defecte creem els dicomdir al discdur
    m_hardDiskAction->trigger();
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
    connect( m_buttonRemoveAll , SIGNAL( clicked() ) , this , SLOT( resetDICOMDIRList() ) );
    connect( m_buttonExamineDisk , SIGNAL( clicked() ) , this , SLOT( examineDicomdirPath() ) );
    connect( m_buttonCreateDicomdir , SIGNAL( clicked() ) , this , SLOT( createDicomdir() ) );
    connect( m_copyFolderContentToDICOMDIRCdDvdCheckBox , SIGNAL( stateChanged(int) ) , this , SLOT( copyContentFolderToDICOMDIRCheckBoxsStateChanged() ) );
    connect( m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox , SIGNAL( stateChanged(int) ) , this , SLOT( copyContentFolderToDICOMDIRCheckBoxsStateChanged() ) );
}

void QCreateDicomdir::showDICOMDIRSize()
{
    QString sizeOfDicomdirText, sizeText;
    double sizeInMb;
    Settings settings;

    sizeInMb = m_dicomdirSizeBytes / ( 1024.0 * 1024 );//passem a Mb
    sizeText.setNum( sizeInMb , 'f' , 0 );

    //Si les imatges s'han de convertir a LittleEndian obtenim el tamany que ocuparà l'estudi de manera aproximada
    if (settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool())
    {
        sizeOfDicomdirText = tr("DICOMDIR size: %1 Mb approximately - Available Space: %2 Mb").arg(sizeText).arg(m_availableSpaceToRecordInBytes/(1024*1024));
    }
    else
    {
        sizeOfDicomdirText = tr("DICOMDIR size: %1 Mb - Available Space: %2 Mb").arg(sizeText).arg(m_availableSpaceToRecordInBytes/(1024*1024));
    }

    m_dicomdirSizeOnDiskLabel->setText( sizeOfDicomdirText );

    if ( sizeInMb < m_progressBarOcupat->maximum() )
        m_progressBarOcupat->setValue( (int)sizeInMb );
    else
        m_progressBarOcupat->setValue( m_progressBarOcupat->maximum() );

    m_progressBarOcupat->repaint();

    sizeOfDicomdirText = tr("%1 Mb").arg( sizeText );
    m_labelMbCdDvdOcupat->setText( sizeOfDicomdirText );
}

void QCreateDicomdir::addStudies(const QList<Study *> &studies)
{
    QStringList existingStudies;
    QStringList notAddedStudies;
    qint64 studySizeBytes;
    Status state;
    Settings settings;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    foreach( Study *study, studies )
    {
        if ( !studyExistsInDICOMDIRList( study->getInstanceUID() ) )
        {
            // \TODO Xapussa perquè ara, a primera instància, continui funcionant amb les classes Study i demés. Caldria unificar el tema
            // "a quin directori està aquest study"?
            studySizeBytes = getStudySizeInBytes(settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool(), study->getInstanceUID());
            // Comprovem si tenim prou espai per l'estudi
            if ( studySizeBytes + m_dicomdirSizeBytes > m_availableSpaceToRecordInBytes )
            {
                notAddedStudies << study->getInstanceUID();
            }
            else
            {
                //Afegim la informació de l'estudi a la llista
                QTreeWidgetItem* item = new QTreeWidgetItem( m_dicomdirStudiesList );
                m_dicomdirSizeBytes = m_dicomdirSizeBytes + studySizeBytes;
            
                Patient *patient = study->getParentPatient();
                item->setText( 0, patient->getFullName() );
                item->setText( 1, patient->getID() );
                item->setText( 2, study->getPatientAge() );
                item->setText( 3, study->getDescription() );
                item->setText( 4, study->getModalitiesAsSingleString() );
                item->setText( 5, study->getDate().toString(Qt::ISODate) );
                item->setText( 6, study->getTime().toString(Qt::ISODate) );
                item->setText( 7, study->getInstanceUID() );
            }
        }
        else 
            existingStudies << study->getInstanceUID();
    }
    QApplication::restoreOverrideCursor();

    updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
    showDICOMDIRSize();

    if( notAddedStudies.size() > 0 || existingStudies.size() > 0 )
    {
        QString message;
        message = tr("The following studies were not added to the DICOMDIR list for the following reasons:");
        if( existingStudies.size() > 0 )
        {
            message += tr("\n\n\t- Already exist in the list.\n");
            message += existingStudies.join("\n");
        }
        if( notAddedStudies.size() > 0 )
        {
            message += tr("\n\n\t- Not enough space on the device.\n");
            message += notAddedStudies.join("\n");
        }
        QMessageBox::warning(0, ApplicationNameString, message );
    }
}

void QCreateDicomdir::createDicomdir()
{
    if (m_dicomdirSizeBytes > m_availableSpaceToRecordInBytes)
    {
        QMessageBox::warning(this, ApplicationNameString, tr("DICOMDIR creation aborted.\n The selected studies exceed the available space for the current device.") );
        return;
    }

    Status state;

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
                    burnDicomdir();
                 break;
    }
}

Status QCreateDicomdir::createDicomdirOnCdOrDvd()
{
    QDir temporaryDirPath;
    QString dicomdirPath = getTemporaryDICOMDIRPath();
    Status state;

    // Si el directori dicomdir ja existeix al temporal l'esborrem
    clearTemporaryDICOMDIRPath();

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

    if ( m_lineEditDicomdirPath->text().isEmpty() )
    {
        QMessageBox::information( this , ApplicationNameString , tr( "No directory specified to create the DICOMDIR" ) );
        return;
    }

    INFO_LOG ( "Iniciant la creació del DICOMDIR en discdur o usb al directori " + dicomdirPath );

    /*TODO:El codi de comprova si el directori és un DICOMDIR hauria d'estar a una DICOMDIRManager, la UI no ha de saber
      quins elements componen un DICOMDIR*/
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
    Settings settings;

    convertToDicomdir.setConvertDicomdirImagesToLittleEndian( settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool() );

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

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    for ( int i = 0; i < dicomdirStudiesList.count(); i++ )
    {
        item = dicomdirStudiesList.at( i );
        convertToDicomdir.addStudy( item->text( 7 ) ); // Indiquem a la classe convertToDicomdir, quins estudis s'ha de convertir a dicomdir, passant el UID de l'estudi
        INFO_LOG( "L'estudi " + item->text( 7 ) + " s'afegirà al DICOMDIR " );
    }

    state = convertToDicomdir.convert(dicomdirPath, m_currentDevice, haveToCopyFolderContentToDICOMDIR());

    if ( !state.good() )
    {
        QApplication::restoreOverrideCursor();

        switch ( state.code() )
        {
            case 4001: // Alguna de les imatges no compleix l'estandard dicom però es pot continuar endavant
                QMessageBox::information( this , ApplicationNameString, tr( "Some images are not 100 % DICOM compliant. It could be possible that some viewers have problems to visualize them." ) );
                break;
            case 4002:
                QMessageBox::warning( this , ApplicationNameString , tr( "%1 can't create the DICOMDIR because can't copy the content of '%2'. Be sure you have read permissions in the directory or "
                    "uncheck copy folder content option." )
                    .arg(ApplicationNameString, settings.getValue( InputOutputSettings::DICOMDIRFolderPathToCopy ).toString() ) );
                break;
            case 4003:
                QMessageBox::warning( this , ApplicationNameString , tr( "%1 can't create the DICOMDIR because the folder to copy '%2' contents an item called DICOMDIR or DICOM."
                    "\n\nRemove it from the directory or uncheck copy folder content option.")
                    .arg(ApplicationNameString, settings.getValue( InputOutputSettings::DICOMDIRFolderPathToCopy ).toString() ) );
                break;
            default :
                QMessageBox::critical( this , ApplicationNameString , tr( "Error creating DICOMDIR. Be sure you have user permissions in %1 and the directory is empty." ).arg( m_lineEditDicomdirPath->text() ) );
                ERROR_LOG( "Error al crear el DICOMDIR ERROR : " + state.text() );
                return state;
        }
    }

    INFO_LOG( "Finalitzada la creació del DICOMDIR" );
    clearQCreateDicomdirScreen();

    QApplication::restoreOverrideCursor();

    return state;
}

void QCreateDicomdir::clearQCreateDicomdirScreen()
{
    m_dicomdirStudiesList->clear();
    m_lineEditDicomdirPath->clear();

    resetDICOMDIRList();
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
    {
        m_lineEditDicomdirPath->setText(QDir::toNativeSeparators(path));
        // Actualitzem les etiquetes que indiquen la capacitat del disc
        updateAvailableSpaceToRecord();
        showDICOMDIRSize();
    }
}

void QCreateDicomdir::resetDICOMDIRList()
{
    m_dicomdirSizeBytes = 0;
    m_dicomdirStudiesList->clear();
    m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize = false;

    showDICOMDIRSize();
}

void QCreateDicomdir::removeSelectedStudy()
{
    Settings settings;

    if (m_dicomdirStudiesList->selectedItems().count() != 0)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        foreach(QTreeWidgetItem *selectedStudy, m_dicomdirStudiesList->selectedItems())
        {
            m_dicomdirSizeBytes -= getStudySizeInBytes(settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool(), selectedStudy->text(7));//La columna 7 de m_dicomdirStudiesList conté Study Instance UID

            delete selectedStudy;
        }
        QApplication::restoreOverrideCursor();

        if (m_dicomdirStudiesList->findItems( "*" , Qt::MatchWildcard, 0 ).count() == 0)
        {
            //Si no tenim cap estudi reiniciem les variables que controlen la Llista de DICOMDIR
            resetDICOMDIRList();
        }
        else

        updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
        showDICOMDIRSize();
    }
    else QMessageBox::information(this, ApplicationNameString, tr("Please select a study to remove of the list."));
}

bool QCreateDicomdir::studyExistsInDICOMDIRList( QString studyUID )
{
    QList<QTreeWidgetItem *> dicomdirStudiesList( m_dicomdirStudiesList ->findItems( studyUID , Qt::MatchExactly, 7 ) );

    if ( dicomdirStudiesList.count() > 0 )
    {
        return true;
    }
    else return false;
}

void QCreateDicomdir::burnDicomdir()
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    QString dicomdirPath = getTemporaryDICOMDIRPath(), isoPath;

    // Indiquem al directori i nom de la imatge a crear
    isoPath = dicomdirPath + "/dicomdir.iso";

    // Es crea un ProgressDialog 
    m_progressBar = new QProgressDialog( QObject::tr( "Creating DICOMDIR Image..." ), "", 0, 0 );
    m_progressBar->setMinimumDuration( 0 );
    m_progressBar->setCancelButton( 0 );
    m_progressBar->setModal( true );
    m_progressBar->setValue( 1 );

    // Es crea un Timer per tal de poder moure la barra de progres cada segon
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), m_progressBar, SLOT(setValue(m_progressBar->value()+1)));
    m_timer->start(1000);

    m_isoImageFileCreator = new IsoImageFileCreator();

    // Es conecta el signal que indica la finalització de la creació del fitxer d'imatge ISO amb l'Slot que obre el programa de gravació
    connect(m_isoImageFileCreator, SIGNAL(finishedCreateIsoImageFile(bool)), this, SLOT(openBurningApplication(bool)));

    m_isoImageFileCreator->setInputPath(dicomdirPath);
    m_isoImageFileCreator->setOutputIsoImageFilePath(isoPath);
    m_isoImageFileCreator->setIsoImageLabel(QString("%1 DICOMDIR").arg(ApplicationNameString));
    
    m_isoImageFileCreator->startCreateIsoImageFile();
}

void QCreateDicomdir::openBurningApplication(bool createIsoResult)
{
    m_timer->stop();
    delete m_timer;
    m_progressBar->close();
    delete m_progressBar;

    if(createIsoResult)
    {
        DICOMDIRBurningApplication dicomdirBurningApplication;
        dicomdirBurningApplication.setCurrentDevice(m_currentDevice);
        dicomdirBurningApplication.setIsoPath(m_isoImageFileCreator->getOutputIsoImageFilePath());
        
        if( !dicomdirBurningApplication.burnIsoImageFile() )
        {
            QMessageBox::critical(this, tr("DICOMDIR Burning Failure"), tr("There was an error during the burning of the DICOMDIR ISO image file.\n") + dicomdirBurningApplication.getLastErrorDescription() + "\n\n" + tr("Please, contact your system administrator to solve this problem.") );
            ERROR_LOG( "Error al gravar la imatge ISO amb descripció: " + dicomdirBurningApplication.getLastErrorDescription() );
        }
    }
    else
    {
        QMessageBox::critical(this, tr("DICOMDIR Creation Failure"), tr("There was an error during the creation of the DICOMDIR ISO image file. ") + m_isoImageFileCreator->getLastErrorDescription() + "\n\n" + tr("Please, contact your system administrator to solve this problem.") );
        ERROR_LOG( "Error al crear ISO amb descripció: " + m_isoImageFileCreator->getLastErrorDescription() );
    }
    delete m_isoImageFileCreator;
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

void QCreateDicomdir::clearTemporaryDICOMDIRPath()
{
    QString dicomdirPath = getTemporaryDICOMDIRPath();

    if ( QFile::exists( dicomdirPath ) )
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
    m_currentDevice = (CreateDicomdir::recordDeviceDicomDir) index;

    updateAvailableSpaceToRecord();
    /*Starviewer donoa la possibilitat de copiar al contingut d'una carpeta als DICOMDIR que es generen^,
     Cridem aquest mètode perquè en funció del dispositu potser que es copïi o no la carpeta al DICOMDIR per tant s'ha d'actualitzar
     la mida del DICOMDIR*/
    updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
    
    switch( m_currentDevice )
    {
        case CreateDicomdir::UsbPen:
        case CreateDicomdir::HardDisk:
            m_stackedWidget->setCurrentIndex(1);

            if ( m_dicomdirSizeBytes > m_availableSpaceToRecordInBytes )
            {
                QMessageBox::warning( this , ApplicationNameString , tr( "The selected device doesn't have enough space to create a DICOMDIR with all this studies, please remove some studies. The capacity of the device is %1 Mb." ).arg(m_availableSpaceToRecordInBytes/(1024*1024)) );
            }

            break;
        case CreateDicomdir::CdRom:
        case CreateDicomdir::DvdRom:
            int maximumDeviceCapacity;

            if (checkDICOMDIRBurningApplicationConfiguration())
            {
                //La configuració de l'aplicació per gravar cd/dvd és vàlida
                maximumDeviceCapacity = m_currentDevice == CreateDicomdir::CdRom ? CDRomSizeMb : DVDRomSizeMb;
                
                m_stackedWidget->setCurrentIndex(0);//Indiquem que es mostri la barra de progrés
                
                m_progressBarOcupat->setMaximum(maximumDeviceCapacity);

                if (m_dicomdirSizeBytes > m_availableSpaceToRecordInBytes)
                {
                    QMessageBox::warning( this , ApplicationNameString , tr( "The selected device doesn't have enough space to create a DICOMDIR with all this studies, please remove some studies. The capacity of a cd is %1 Mb." ).arg(maximumDeviceCapacity) );
                }
            }
            else
            {
                //La configuració de l'aplicació per gravar cd/dvd no ès vàlida
                QMessageBox::warning( this, ApplicationNameString, 
                                        tr( "Invalid DICOMDIR burning application configuration.\n" 
                                            "It can be solved in Tools -> Configuration -> DICOMDIR menu.") );
            
                //Marquem la opció de crear el dicomdir al disc dur
                m_hardDiskAction->trigger();
            }

            break;
    }

    showDICOMDIRSize();//El cridem per refrescar la barra de progrés   
}

bool QCreateDicomdir::checkDICOMDIRBurningApplicationConfiguration()
{
    Settings settings;
    return QFile::exists((settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey)).toString());
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
        size = (bits-allocated / 8) * rows * columns * samples-per-pixel;
        size += size % 2;
      Llavors hem d'afegir la capçalera => dicom header size */

    quint64 imageSizeInBytesInLittleEndianTransferSyntax = 0;

    imageSizeInBytesInLittleEndianTransferSyntax = ( image->getBitsAllocated() / 8) * image->getRows() * image->getColumns() * image->getSamplesPerPixel();
    imageSizeInBytesInLittleEndianTransferSyntax += imageSizeInBytesInLittleEndianTransferSyntax % 2;

    //afegim el tamany de la capçalera
    imageSizeInBytesInLittleEndianTransferSyntax += DicomHeaderSizeBytes;

    return imageSizeInBytesInLittleEndianTransferSyntax;
}

void QCreateDicomdir::updateAvailableSpaceToRecord()
{
    QString path = m_lineEditDicomdirPath->text();
    HardDiskInformation diskInfo;
    switch( m_currentDevice )
    {
        case CreateDicomdir::UsbPen:
        case CreateDicomdir::HardDisk:
            if( path.isEmpty() || !QDir(path).exists() )
            {
                m_lineEditDicomdirPath->setText( QDir::toNativeSeparators( QDir::rootPath() ) );
                path = QDir::rootPath();
            }
            m_availableSpaceToRecordInBytes = diskInfo.getNumberOfFreeBytes(path);
            break;
        
        case CreateDicomdir::CdRom:
            m_availableSpaceToRecordInBytes = CDRomSizeBytes;
            break;
        
        case CreateDicomdir::DvdRom:
            m_availableSpaceToRecordInBytes = DVDRomSizeBytes;
            break;
    }
}

QString QCreateDicomdir::getTemporaryDICOMDIRPath()
{
    return QDir::tempPath() + "/DICOMDIR";
}

quint64 QCreateDicomdir::getFolderToCopyToDICOMDIRSizeInBytes()
{
    Settings settings;
    int size = 0;

    if (settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toBool())
    {
        QString folderPathToCopy = settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString();

        if (QFile::exists(folderPathToCopy))
        {
            size = HardDiskInformation().getDirectorySizeInBytes(folderPathToCopy);
        }
    }

    return size;
}

bool QCreateDicomdir::haveToCopyFolderContentToDICOMDIR()
{
    //S'ha de copiar el visor DICOM si està configurat així als settings i el dispositiu actual és cd/dvd
    return (m_copyFolderContentToDICOMDIRCdDvdCheckBox->isChecked() &&  (m_currentDevice == CreateDicomdir::CdRom || m_currentDevice == CreateDicomdir::DvdRom)) || 
        (m_copyFolderContentToDICOMDIRUsbHardDiskCheckBox->isChecked() && (m_currentDevice == CreateDicomdir::UsbPen || m_currentDevice == CreateDicomdir::HardDisk));
}

void QCreateDicomdir::updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize()
{
    QList<QTreeWidgetItem*> dicomdirStudies = m_dicomdirStudiesList->findItems( "*" , Qt::MatchWildcard, 0 );

    if (haveToCopyFolderContentToDICOMDIR() &&  dicomdirStudies.count() > 0 &&
        !m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize)
    {
        /*Si hem de copiar el contingut de la carpeta al DICOMDIR, tenim un estudi o més i no l'havíem tingut en compte 
          la mida de la carpeta a copiar en el tamany del DICOMDIR, li afegim*/
        m_dicomdirSizeBytes += getFolderToCopyToDICOMDIRSizeInBytes();
        m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize = true;
    }

    if (!haveToCopyFolderContentToDICOMDIR() &&  dicomdirStudies.count() > 0 &&
        m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize)
    {
        /*Si no hem de copiar el contingut de la carpeta al DICOMDIR, tenim un estudi o més i l'havíem tingut en compte 
          la mida de la carpeta a copiar en el tamany del DICOMDIR, li restem*/

        m_dicomdirSizeBytes -= getFolderToCopyToDICOMDIRSizeInBytes();
        m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize = false;        
    }
}

void QCreateDicomdir::copyContentFolderToDICOMDIRCheckBoxsStateChanged()
{
    updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
    showDICOMDIRSize();
}
}
