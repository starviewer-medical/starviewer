/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "extensionhandler.h"

// Qt
#include <QProgressDialog>
#include <QMessageBox>
// Recursos
#include "logging.h"
#include "extensionworkspace.h"
#include "qapplicationmainwindow.h"
#include "volume.h"
#include "extensionmediatorfactory.h"
#include "extensionfactory.h"
#include "singleton.h"
#include "starviewerapplication.h"
#include "interfacesettings.h"
#include "screenmanager.h"
#include "patientcomparer.h"
#include "patient.h"
#include "image.h"
#include "volumehelper.h"

// PACS --------------------------------------------
#include "queryscreen.h"
#include "patientfiller.h"

namespace udg {

typedef SingletonPointer<QueryScreen> QueryScreenSingleton;
typedef Singleton<PatientComparer> PatientComparerSingleton;

ExtensionHandler::ExtensionHandler(QApplicationMainWindow *mainApp, QObject *parent)
 : QObject(parent)
{
    m_mainApp = mainApp;

    createConnections();

    // Cada cop que creem una nova finestra tancarem qualsevol instància de QueryScreen. Així queda més clar que 
    // la finestra que la invoca és la que rep el resultat d'aquesta
    // TODO Cal millorar el disseny de la interacció amb la QueryScreen per tal de no tenir problemes com els que s'exposen als tickets
    // #1858, #1018. De moment ho solventem amb aquests hacks, però no són una bona solució
    // TODO:xapussa per a que l'starviewer escolti les peticions del RIS, com que tot el codi d'escoltar les peticions del ris està a la
    // queryscreen l'hem d'instanciar ja a l'inici perquè ja escolti les peticions
    disconnect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), 0, 0);
    connect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), SLOT(processInput(QList<Patient*>, bool)));

    connect(QueryScreenSingleton::instance(), SIGNAL(closed()), SLOT(queryScreenIsClosed()));
    m_haveToCloseQueryScreen = false;
}

ExtensionHandler::~ExtensionHandler()
{
    // Cada cop que tanquem una finestra forçarem el tancament de la queryscreen. Això es fa perquè quedi clar que
    // QueryScreen <-> finestra on s'obren els estudis, estan lligats segons qui l'ha invocat
    // TODO Tot això precisa d'un millor disseny, però de moment evita problemes com els del ticket #1858
    if (m_haveToCloseQueryScreen)
    {
        QueryScreenSingleton::instance()->close();
    }
}

void ExtensionHandler::queryScreenIsClosed()
{
    m_haveToCloseQueryScreen = false;
}

void ExtensionHandler::request(int who)
{
    // \TODO: crear l'extensió amb el factory ::createExtension, no com està ara
    // \TODO la numeració és completament temporal!!! s'haurà de canviar aquest sistema
    INFO_LOG("Request d'extensió amb ID: " + QString::number(who));
    switch (who)
    {
        case 1:
            m_importFileApp.open();
            break;

        case 6:
            m_importFileApp.openDirectory();
            break;

        case 7:
            // HACK degut a que la QueryScreen és un singleton, això provoca efectes colaterals quan teníem
            // dues finestres (mirar ticket #542). Fem aquest petit hack perquè això no passi.
            // Queda pendent resoldre-ho de la forma adequada
            disconnect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), 0, 0);
            QueryScreenSingleton::instance()->showPACSTab();
            connect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), SLOT(processInput(QList<Patient*>, bool)));
            m_haveToCloseQueryScreen = true;
            break;

        case 8:
            // HACK degut a que la QueryScreen és un singleton, això provoca efectes colaterals quan teníem
            // dues finestres (mirar ticket #542). Fem aquest petit hack perquè això no passi.
            // Queda pendent resoldre-ho de la forma adequada
            disconnect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), 0, 0);
            QueryScreenSingleton::instance()->openDicomdir();
            connect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), SLOT(processInput(QList<Patient*>, bool)));
            m_haveToCloseQueryScreen = true;
            break;

        case 10:
            // Mostrar local
            // HACK degut a que la QueryScreen és un singleton, això provoca efectes colaterals quan teníem
            // dues finestres (mirar ticket #542). Fem aquest petit hack perquè això no passi.
            // Queda pendent resoldre-ho de la forma adequada
            disconnect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), 0, 0);
            QueryScreenSingleton::instance()->showLocalExams();
            connect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), SLOT(processInput(QList<Patient*>, bool)));
            m_haveToCloseQueryScreen = true;
            break;
    }
}

bool ExtensionHandler::request(const QString &who)
{
    bool ok = true;
    ExtensionMediator *mediator = ExtensionMediatorFactory::instance()->create(who);
    if (!mediator)
    {
        WARN_LOG("No s'ha pogut crear el mediator per: " + who);
        DEBUG_LOG("No s'ha pogut crear el mediator per: " + who);
        ok = false;
        return ok;
    }

    bool createExtension = true;
    int extensionIndex = 0;
    QString requestedExtensionLabel = mediator->getExtensionID().getLabel();
    if (!Settings().getValue(InterfaceSettings::AllowMultipleInstancesPerExtension).toBool())
    {
        // Només volem una instància per extensió
        // Cal comprovar llavors que l'extensió que demanem no estigui ja creada
        int count = m_mainApp->getExtensionWorkspace()->count();
        bool found = false;
        while (extensionIndex < count && !found)
        {
            if (m_mainApp->getExtensionWorkspace()->tabText(extensionIndex) == requestedExtensionLabel)
            {
                found = true;
            }
            else
            {
                extensionIndex++;
            }
        }
        // Si la trobem, no caldrà crear-la de nou
        if (found)
        {
            createExtension = false;
        }
    }

    // Segons la configuració i les extensions existents, haurem de crear o no l'extensió demanada
    if (createExtension)
    {
        QWidget *extension = ExtensionFactory::instance()->create(who);
        if (extension)
        {
            INFO_LOG("Activem extensió: " + who);
            mediator->initializeExtension(extension, m_extensionContext);
            m_mainApp->getExtensionWorkspace()->addApplication(extension, requestedExtensionLabel, who);
        }
        else
        {
            ok = false;
            DEBUG_LOG("Error carregant " + who);
        }
    }
    else
    {
        // Sinó mostrem l'extensió ja existent
        m_mainApp->getExtensionWorkspace()->setCurrentIndex(extensionIndex);
    }

    delete mediator;

    return ok;
}

void ExtensionHandler::setContext(const ExtensionContext &context)
{
    m_extensionContext = context;
}

ExtensionContext &ExtensionHandler::getContext()
{
    return m_extensionContext;
}

void ExtensionHandler::createConnections()
{
    connect(&m_importFileApp, SIGNAL(selectedFiles(QStringList)), SLOT(processInput(QStringList)));
}

void ExtensionHandler::processInput(const QStringList &inputFiles)
{
    if (inputFiles.isEmpty())
    {
        return;
    }

    QProgressDialog progressDialog(m_mainApp);
    progressDialog.setModal(true);
    progressDialog.setRange(0, 0);
    progressDialog.setMinimumDuration(0);
    progressDialog.setWindowTitle(tr("Patient Loading"));
    progressDialog.setLabelText(tr("Loading, please wait..."));
    progressDialog.setCancelButton(0);

    qApp->processEvents();
    PatientFiller patientFiller;
    connect(&patientFiller, SIGNAL(progress(int)), &progressDialog, SLOT(setValue(int)));

    QList<Patient*> patientsList = patientFiller.processFiles(inputFiles);

    progressDialog.close();

    int numberOfPatients = patientsList.size();

    if (numberOfPatients == 0)
    {
        QMessageBox::critical(0, ApplicationNameString, tr("Sorry, it seems that there is no patient data that can be loaded."));
        ERROR_LOG("Error fent el fill de patientFiller. Ha retornat 0 pacients.");
        return;
    }

    DEBUG_LOG(QString("NumberOfPatients: %1").arg(numberOfPatients));

    QList<int> correctlyLoadedPatients;

    for (int i = 0; i < numberOfPatients; i++)
    {
        DEBUG_LOG(QString("Patient #%1\n %2").arg(i).arg(patientsList.at(i)->toString()));

        bool error = true;

        // Marquem les series seleccionades
        QList<Study*> studyList = patientsList.at(i)->getStudies();
        if (!studyList.isEmpty())
        {
            Study *study = studyList.first();

            QList<Series*> seriesList = study->getSeries();
            if (!seriesList.isEmpty())
            {
                Series *series = seriesList.first();
                series->select();
                error = false;
            }
            else
            {
                ERROR_LOG(patientsList.at(i)->toString());
                ERROR_LOG("Error carregant aquest pacient. La serie retornada és null.");
            }
        }
        else
        {
            ERROR_LOG(patientsList.at(i)->toString());
            ERROR_LOG("Error carregant aquest pacient. L'study retornat és null.");
        }

        if (!error)
        {
            correctlyLoadedPatients << i;
        }
    }

    QString patientsWithError;
    if (correctlyLoadedPatients.count() != numberOfPatients)
    {
        for (int i = 0; i < numberOfPatients; i++)
        {
            if (!correctlyLoadedPatients.contains(i))
            {
                patientsWithError += "- " + patientsList.at(i)->getFullName() + "; ID: " + patientsList.at(i)->getID() + "<br>";
            }
        }
        QMessageBox::critical(0, ApplicationNameString, tr("Sorry, an error occurred while loading data from patients:<br>%1").arg(patientsWithError));
    }
    if (patientsWithError.isEmpty())
    {
        // No hi ha cap error, els carreguem tots
        processInput(patientsList);
    }
    else
    {
        // Carreguem únicament els correctes
        QList<Patient*> rightPatients;
        foreach (int index, correctlyLoadedPatients)
        {
            rightPatients << patientsList.at(index);
        }
        processInput(rightPatients);
    }
}

void ExtensionHandler::processInput(QList<Patient*> patientsList, bool loadOnly)
{
    if (patientsList.isEmpty())
    {
        return;
    }

    // First merge Patients so that all studies from the same real patient are in the same Patient object
    patientsList = mergePatients(patientsList);

    // Now assign the patients to windows. One will be assigned to this window and the others to new windows, we don't care about other existing windows.
    // If one patient in the list matches the current one in this window it will be merged into it.
    // Otherwise the current patient in this window will be replaced by the first one in the list.
    // We assume that loadOnly can only be true if there's a match.

    // Check if the list contains the same patient as the current one in this ExtensionsHandler's window;
    // if that's the case, it will be moved to the beginning of the list so it gets assigned to this window
    bool merge = false;

    for (int i = 0; i < patientsList.size(); i++)
    {
        if (PatientComparerSingleton::instance()->areSamePatient(m_mainApp->getCurrentPatient(), patientsList[i]))  // this works if current patient is null
        {
            patientsList.move(i, 0);
            merge = true;
            break;
        }
    }

    for (int i = 0; i < patientsList.size(); i++)
    {
        Patient *patient = patientsList[i];
        VolumeHelper::generatePatientVolumes(patient);

        if (i == 0)
        {
            if (merge)
            {
                mergeIntoCurrentPatient(patient, loadOnly);
                delete patient; // the studies are now under the current patient, we can delete this one
            }
            else
            {
                // Replace current patient
                m_mainApp->setPatient(patient);
            }

            if (m_mainApp->isMinimized())
            {
                ScreenManager().restoreFromMinimized(m_mainApp);
            }

            m_mainApp->activateWindow();
        }
        else
        {
            m_mainApp->setPatientInNewWindow(patient);
        }
    }
}

QList<Patient*> ExtensionHandler::mergePatients(const QList<Patient*> &patientList)
{
    QList<Patient*> mergedList;
    Patient *currentPatient = 0;
    QList<Patient*> workList = patientList;

    while (!workList.isEmpty())
    {
        currentPatient = workList.takeFirst();
        foreach (Patient *patientToCheck, workList)
        {
            if (currentPatient->compareTo(patientToCheck) == Patient::SamePatients)
            {
                *currentPatient += *patientToCheck;
                workList.removeAll(patientToCheck);
            }
        }
        mergedList << currentPatient;
    }

    return mergedList;
}

void ExtensionHandler::mergeIntoCurrentPatient(Patient *patient, bool loadOnly)
{
    m_mainApp->connectPatientVolumesToNotifier(patient);
    *m_mainApp->getCurrentPatient() += *patient;

    openDefaultExtensions(patient->getStudies());

    if (!loadOnly)
    {
        // Notify open extensions about the new studies
        // TODO This should be merged with the request for newly instanced extensions to improve performance, but it would be too much rework for now, so it's
        //      left as future work
        QList<Study*> newStudies = patient->getStudies();
        DicomEntityFlags newDicomEntities = m_extensionContext.getDicomEntities(newStudies);
        QMap<QWidget*, QString> extensions = m_mainApp->getExtensionWorkspace()->getActiveExtensions();
        QMapIterator<QWidget*, QString> iterator(extensions);

        while (iterator.hasNext())
        {
            iterator.next();
            std::unique_ptr<ExtensionMediator> mediator(ExtensionMediatorFactory::instance()->create(iterator.value()));
            DicomEntityFlags supportedDicomEntities = mediator->getSupportedDicomEntities();
            DicomEntityFlags matchedEntities = newDicomEntities & supportedDicomEntities;

            if (matchedEntities)  // if any entity matches
            {
                // Find the first of the new studies to assign to the extension
                bool found = false;

                for (int i = 0; !found && i < newStudies.size(); i++)
                {
                    Study *study = newStudies[i];
                    DicomEntityFlags studyEntities = m_extensionContext.getDicomEntities({study});

                    if (matchedEntities & studyEntities)    // if this study is supported by this extension
                    {
                        mediator->viewNewStudiesFromSamePatient(iterator.key(), study->getInstanceUID());
                        found = true;
                    }
                }
            }
        }
    }
}

void ExtensionHandler::openDefaultExtensions(QList<Study*> newStudies)
{
    if (newStudies.isEmpty() && (!m_mainApp->getCurrentPatient() || m_mainApp->getCurrentPatient()->getNumberOfStudies() == 0))
    {
        DEBUG_LOG("No studies or patient to visualize");
        return;
    }

    DicomEntityFlags dicomEntities = newStudies.isEmpty() ? m_extensionContext.getDicomEntities() : m_extensionContext.getDicomEntities(newStudies);
    Settings settings;

    if (dicomEntities.testFlag(DicomEntity::EncapsulatedDocument))
    {
        QString defaultExtension = settings.getValue(InterfaceSettings::DefaultEncapsulatedDocumentExtension).toString();

        if (!request(defaultExtension))
        {
            WARN_LOG(QString("Request for default encapsulated document extension '%1' failed. Requesting hardcoded default.").arg(defaultExtension));
            request("PdfExtension");
        }
    }

    // Images are done the last so that their extension gets the focus if requested, so that they have priority over other entities
    if (dicomEntities.testFlag(DicomEntity::Image))
    {
        QString defaultExtension = settings.getValue(InterfaceSettings::DefaultImageExtension).toString();

        if (!request(defaultExtension))
        {
            WARN_LOG(QString("Request for default image extension '%1' failed. Requesting hardcoded default.").arg(defaultExtension));
            request("Q2DViewerExtension");
        }
    }

    if (m_mainApp->getExtensionWorkspace()->count() == 0)
    {
        WARN_LOG(QString("No extension has been open yet. Requesting 2d viewer extension by default."));
        request("Q2DViewerExtension");
    }
}

}   // end namespace udg
