/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qinputoutputdicomdirwidget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QShortcut>

#include "status.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "dicommask.h"
#include "patient.h"
#include "statswatcher.h"
#include "inputoutputsettings.h"

namespace udg
{

QInputOutputDicomdirWidget::QInputOutputDicomdirWidget( QWidget *parent ) : QWidget( parent )
{
    setupUi( this );

    createConnections();
    createContextMenuQStudyTreeWidget();

    Settings settings;
    settings.restoreColumnsWidths( InputOutputSettings::dicomdirStudyListColumnsWidthKey, m_studyTreeWidget->getQTreeWidget() );

    m_statsWatcher = new StatsWatcher("QueryInputOutputDicomdirWidget",this);
    m_statsWatcher->addClicksCounter( m_viewButton );
    m_statsWatcher->addClicksCounter( m_retrieveButton );
    m_statsWatcher->addClicksCounter( m_openDICOMDIRToolButton );
}

QInputOutputDicomdirWidget::~QInputOutputDicomdirWidget()
{
    Settings settings;
    settings.saveColumnsWidths( InputOutputSettings::dicomdirStudyListColumnsWidthKey, m_studyTreeWidget->getQTreeWidget() );
}

void QInputOutputDicomdirWidget::createConnections()
{
    connect (m_openDICOMDIRToolButton, SIGNAL(clicked()), SLOT( openDicomdir())); 

    connect(m_studyTreeWidget, SIGNAL(studyExpanded(QString)), SLOT(expandSeriesOfStudy(QString)));
    connect(m_studyTreeWidget, SIGNAL(seriesExpanded(QString, QString)), SLOT(expandImagesOfSeries(QString, QString)));

    connect(m_studyTreeWidget, SIGNAL(studyDoubleClicked()), SLOT(view()));
    connect(m_studyTreeWidget, SIGNAL(seriesDoubleClicked()), SLOT(view()));
    connect(m_studyTreeWidget, SIGNAL(imageDoubleClicked()), SLOT(view()));
    connect(m_viewButton, SIGNAL(clicked()), SLOT(view()));
    connect(m_retrieveButton, SIGNAL(clicked()), SLOT(retrieveSelectedStudies()));
}

void  QInputOutputDicomdirWidget::createContextMenuQStudyTreeWidget()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidget.addAction( QIcon(":/images/view.png") , tr( "&View" ) , this , SLOT( view() ) , tr("Ctrl+V") );
    (void) new QShortcut( action->shortcut() , this , SLOT( view() ) );

    action = m_contextMenuQStudyTreeWidget.addAction( QIcon(":/images/retrieve.png") , tr("&Import") , this , SLOT( retrieveSelectedStudies() ) , tr("Ctrl+R") );
    (void) new QShortcut( action->shortcut() , this , SLOT( retrieveSelectedStudies() ) );

    m_studyTreeWidget->setContextMenu( & m_contextMenuQStudyTreeWidget ); //Especifiquem que es el menu del dicomdir
}

void QInputOutputDicomdirWidget::openDicomdir()
{
    Settings settings;
    QString dicomdirPath;
    Status state;

    dicomdirPath = QFileDialog::getOpenFileName(0, QFileDialog::tr( "Open" ), settings.getValue( InputOutputSettings::lastOpenedDICOMDIRPathKey ).toString(), "DICOMDIR");

    if (!dicomdirPath.isEmpty())//Si és buit no ens han seleccionat cap fitxer
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        state = m_readDicomdir.open (dicomdirPath);//Obrim el dicomdir
        QApplication::restoreOverrideCursor();
        if ( !state.good() )
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Error openning dicomdir" ) );
            ERROR_LOG( "Error al obrir el dicomdir " + dicomdirPath + state.text() );
        }
        else
        {
            INFO_LOG( "Obert el dicomdir " + dicomdirPath );
            settings.setValue( InputOutputSettings::lastOpenedDICOMDIRPathKey, QFileInfo(dicomdirPath).dir().path() );
        }

        emit clearSearchTexts();//Netegem el filtre de cerca al obrir el dicomdir

        //cerquem els estudis al dicomdir per a que es mostrin
        queryStudy(DicomMask());
    }
}

void QInputOutputDicomdirWidget::queryStudy( DicomMask queryMask )
{
    QList<Patient *> patientStudyList;
    Status state;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    m_studyTreeWidget->clear();

    state = m_readDicomdir.readStudies( patientStudyList , queryMask );
    if ( !state.good() )
    {
        QApplication::restoreOverrideCursor();
        if ( state.code() == 1302 ) //Aquest és l'error quan no tenim un dicomdir obert l'ig
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Before search you have to open a dicomdir." ) );
            ERROR_LOG( "No s'ha obert cap directori dicomdir " + state.text() );
        }
        else
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Error quering in dicomdir" ) );
            ERROR_LOG( "Error cercant estudis al dicomdir " + state.text() );
        }
        return;
    }

    if ( patientStudyList.isEmpty() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information( this , ApplicationNameString , tr( "No study match found." ) );
    }
    else
    {
        m_studyTreeWidget->insertPatientList( patientStudyList );
        m_studyTreeWidget->setSortColumn( QStudyTreeWidget::ObjectName );//ordenem pel nom
        QApplication::restoreOverrideCursor();
    }
}

void QInputOutputDicomdirWidget::clear()
{
    m_studyTreeWidget->clear();
}

void QInputOutputDicomdirWidget::expandSeriesOfStudy( QString studyInstanceUID )
{
    QList<Series*> seriesList;

    INFO_LOG( "Cerca de sèries al DICOMDIR de l'estudi " + studyInstanceUID );

    m_readDicomdir.readSeries( studyInstanceUID , "" , seriesList ); //"" pq no busquem cap serie en concret

    if ( seriesList.isEmpty() )
    {
        QMessageBox::information( this , ApplicationNameString , tr( "No series match for this study.\n" ) );
    }
    else m_studyTreeWidget->insertSeriesList( studyInstanceUID , seriesList );//inserim la informació de la sèrie al llistat
}

void QInputOutputDicomdirWidget::expandImagesOfSeries(QString studyInstanceUID, QString seriesInstanceUID)
{
    QList<Image*> imageList;

    INFO_LOG("Cerca d'imatges al DICOMDIR de l'estudi " + studyInstanceUID + " i serie " + seriesInstanceUID);

    m_readDicomdir.readImages(seriesInstanceUID, "", imageList);

    if (imageList.isEmpty())
    {
        QMessageBox::information( this , ApplicationNameString , tr( "No images match for this series.\n" ) );
        return;
    }
    else m_studyTreeWidget->insertImageList( studyInstanceUID, seriesInstanceUID, imageList );
}

void QInputOutputDicomdirWidget::retrieveSelectedStudies()
{
    DICOMDIRImporter importDicom;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // TODO ara només permetrem importar estudis sencers
    foreach(QString studyInstanceUID, m_studyTreeWidget->getSelectedStudiesUID())
    {
        importDicom.import(m_readDicomdir.getDicomdirFilePath(), studyInstanceUID, QString(), QString());
        if (importDicom.getLastError() != DICOMDIRImporter::Ok)
        {
            //S'ha produït un error
            QApplication::restoreOverrideCursor();
            showDICOMDIRImporterError(studyInstanceUID, importDicom.getLastError());
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            if (importDicom.getLastError() != DICOMDIRImporter::PatientInconsistent &&
                importDicom.getLastError() != DICOMDIRImporter::DicomdirInconsistent)
            {
                //Si es produeix qualsevol dels altres errors parem d'importar estudis, perquè segurament les següents importacions també fallaran
                break;
            }
        }
        else emit studyRetrieved(studyInstanceUID);
    }

    QApplication::restoreOverrideCursor();

    //queryStudy("Cache"); //Actualitzem la llista tenint en compte el criteri de cerca
}

void QInputOutputDicomdirWidget::view()
{
    QStringList selectedStudiesInstanceUID = m_studyTreeWidget->getSelectedStudiesUID();
    DicomMask patientToProcessMask;
    Patient *patient;
    QList<Patient *> selectedPatientsList;

    if(selectedStudiesInstanceUID.isEmpty())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select at least one study to view"));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //TODO: S'hauria de millorar el mètode ja que per la seva estructura lo d'obrir l'estudi per la sèrie que ens tinguin seleccionada només ho farà per un estudi ja que aquest mètode només se li passa per paràmetre una sèrie per defecte
    foreach(QString studyInstanceUIDSelected, selectedStudiesInstanceUID)
    {
        patientToProcessMask.setStudyUID(studyInstanceUIDSelected);
        patient = m_readDicomdir.retrieve(patientToProcessMask);

        if(patient)
        {
            // Marquem la sèrie per defecte
            // TODO ara sempre posem el mateix UID, per tant de moment només funciona bé del tot quan seleccionem un únic estudi
            patient->setSelectedSeries(m_studyTreeWidget->getCurrentSeriesUID());
            selectedPatientsList << patient;
        }
        else DEBUG_LOG("No s'ha pogut obtenir l'estudi amb UID " + studyInstanceUIDSelected );
    }

    DEBUG_LOG("Llançat signal per visualitzar estudi del pacient " + patient->getFullName());
    QApplication::restoreOverrideCursor();

    emit viewPatients( Patient::mergePatients( selectedPatientsList ) );
}

void QInputOutputDicomdirWidget::showDICOMDIRImporterError(QString studyInstanceUID, DICOMDIRImporter::DICOMDIRImporterError error)
{
    QString message;

    switch (error)
    {
        case DICOMDIRImporter::ErrorOpeningDicomdir :
            message = tr("Trying to import study with UID %1 ").arg(studyInstanceUID);
            message += tr("the dicomdir could not be opened. Be sure that the dicomdir path is correct.\n");
            message += tr("\n\nIf the problem persist contact with an administrator.");
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
        case DICOMDIRImporter::ErrorCopyingFiles :
            message = tr("Some files of study with UID %2 could not be imported. Be sure that you have write permissions on the %1 cache directory.").arg(ApplicationNameString, studyInstanceUID);
            message += tr("\n\nIf the problem persist contact with an administrator.");
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
        case DICOMDIRImporter::NoEnoughSpace :
            message = tr("There is not enough space to retrieve studies, please free space.");
            QMessageBox::warning( this , ApplicationNameString , message );
            break;
        case DICOMDIRImporter::ErrorFreeingSpace :
            message = tr("An error has ocurred freeing space, some studies can't be imported.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
        case DICOMDIRImporter::DatabaseError :
            message = tr("A database error has ocurred, some studies can't be imported.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
       case DICOMDIRImporter::PatientInconsistent :
            message = tr("The study with UID %2 can't be imported, because %1 has not been capable of read correctly dicom information of the study.").arg(ApplicationNameString, studyInstanceUID);
            message += tr("\n\nThe study may be corrupted, if It is not corrupted please contact with %1 team.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
       case DICOMDIRImporter::DicomdirInconsistent :
            message = tr("Trying to import study with UID %1 ").arg(studyInstanceUID);
            message += tr("has ocurred an error. This dicomdir is inconsistent, can't be imported.");
            message += tr("\n\nPlease contact with %1 team.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
      case DICOMDIRImporter::Ok :
            break;
        default:
            message = tr("An unknown error has ocurred importing dicomdir.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
    }
}

};
