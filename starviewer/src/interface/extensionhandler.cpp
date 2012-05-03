#include "extensionhandler.h"

// Qt
#include <QFileInfo>
#include <QDir>
#include <QProgressDialog>
#include <QMessageBox>
// Recursos
#include "logging.h"
#include "extensionworkspace.h"
#include "qapplicationmainwindow.h"
#include "volume.h"
#include "extensionmediatorfactory.h"
#include "extensionfactory.h"
#include "extensioncontext.h"
#include "singleton.h"
#include "starviewerapplication.h"
#include "interfacesettings.h"
#include "screenmanager.h"

// PACS --------------------------------------------
#include "queryscreen.h"
#include "patientfiller.h"

namespace udg {

typedef SingletonPointer<QueryScreen> QueryScreenSingleton;

QHash<QString, bool> ExtensionHandler::m_patientsSimilarityUserDecision;

ExtensionHandler::ExtensionHandler(QApplicationMainWindow *mainApp, QObject *parent)
 : QObject(parent)
{
    m_mainApp = mainApp;

    createConnections();

    // TODO:xapussa per a que l'starviewer escolti les peticions del RIS, com que tot el codi d'escoltar les peticions del ris està a la
    // queryscreen l'hem d'instanciar ja a l'inici perquè ja escolti les peticions
    QueryScreenSingleton::instance();
    connect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), SLOT(processInput(QList<Patient*>, bool)));
}

ExtensionHandler::~ExtensionHandler()
{
    // Si és la última finestra oberta, hem de tancar la queryscreen
    if (m_mainApp->getCountQApplicationMainWindow() == 1)
    {
        // TODO:Xapussa invoquem el close() de la QueryScreen que tanca totes les finestres dependent de la QueryScreen perquè s'emeti el signal
        // lasWindowClosed al qual responem des de la main fent un quit, no podríem fer un quit directament des d'aquí?
        QueryScreenSingleton::instance()->close();
    }
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
            break;

        case 8:
            // HACK degut a que la QueryScreen és un singleton, això provoca efectes colaterals quan teníem
            // dues finestres (mirar ticket #542). Fem aquest petit hack perquè això no passi.
            // Queda pendent resoldre-ho de la forma adequada
            disconnect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), 0, 0);
            QueryScreenSingleton::instance()->openDicomdir();
            connect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), SLOT(processInput(QList<Patient*>, bool)));
            break;

        case 10:
            // Mostrar local
            // HACK degut a que la QueryScreen és un singleton, això provoca efectes colaterals quan teníem
            // dues finestres (mirar ticket #542). Fem aquest petit hack perquè això no passi.
            // Queda pendent resoldre-ho de la forma adequada
            disconnect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), 0, 0);
            QueryScreenSingleton::instance()->showLocalExams();
            connect(QueryScreenSingleton::instance(), SIGNAL(selectedPatients(QList<Patient*>, bool)), SLOT(processInput(QList<Patient*>, bool)));
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

void ExtensionHandler::updateConfiguration(const QString &configuration)
{
    QueryScreenSingleton::instance()->updateConfiguration(configuration);
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
    progressDialog.setWindowTitle(tr("Patient loading"));
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
        QMessageBox::critical(0, ApplicationNameString, tr("Sorry, it seems that there is no patient data we can load."));
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
        QMessageBox::critical(0, ApplicationNameString, tr("Sorry, an error occurred while loading the data of patients:<br> %1").arg(patientsWithError));
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
    // Si de tots els pacients que es carreguen intentem carregar-ne un d'igual al que ja tenim carregat, el mantenim
    bool canReplaceActualPatient = true;
    if (m_mainApp->getCurrentPatient())
    {
        QListIterator<Patient*> patientsIterator(patientsList);
        while (canReplaceActualPatient && patientsIterator.hasNext())
        {
            Patient *patient = patientsIterator.next();
            QListIterator<QApplicationMainWindow*> mainAppsIterator(QApplicationMainWindow::getQApplicationMainWindows());
            while (canReplaceActualPatient && mainAppsIterator.hasNext())
            {
                QApplicationMainWindow *mainApp = mainAppsIterator.next();
                canReplaceActualPatient = !askForPatientsSimilarity(mainApp->getCurrentPatient(), patient);
            }
        }
    }

    bool firstPatient = true;

    // Afegim els pacients carregats correctament
    foreach (Patient *patient, patientsList)
    {
        generatePatientVolumes(patient, QString());
        QApplicationMainWindow *mainApp = this->addPatientToWindow(patient, canReplaceActualPatient, loadOnly);

        if (mainApp)
        {
            if (mainApp->isMinimized())
            {
                //Si la finestra d'Starviewer està minimitzada la tornem al seu estat original al visualitzar l'estudi
                ScreenManager().restoreFromMinimized(mainApp);
            }

            if (firstPatient)
            {
                mainApp->activateWindow();
            }
        }
        firstPatient = false;

        // Un cop carregat un pacient, ja no el podem reemplaçar
        canReplaceActualPatient = false;
    }
}

bool ExtensionHandler::askForPatientsSimilarity(Patient *patient1, Patient *patient2)
{
    if (!patient1 || !patient2)
    {
        return false;
    }

    Patient::PatientsSimilarity patientsSimilarity = patient1->compareTo(patient2);
    if (patientsSimilarity == Patient::SamePatients)
    {
        return true;
    }
    else if (patientsSimilarity == Patient::SamePatientIDsDifferentPatientNames || patientsSimilarity == Patient::SamePatientNamesDifferentPatientIDs)
    {
        QString idNamePatient1 = QString("%1_%2").arg(patient1->getID()).arg(patient1->getFullName());
        QString idNamePatient2 = QString("%1_%2").arg(patient2->getID()).arg(patient2->getFullName());

        QString hashKey;
        if (QString::compare(idNamePatient1, idNamePatient2) > 0)
        {
            hashKey = QString("%1_%2").arg(idNamePatient1).arg(idNamePatient2);
        }
        else
        {
            hashKey = QString("%1_%2").arg(idNamePatient2).arg(idNamePatient1);
        }

        if (m_patientsSimilarityUserDecision.contains(hashKey))
        {
            return m_patientsSimilarityUserDecision.value(hashKey);
        }
        else
        {
            QString text = tr("We are not able to determine the similarity of these patients.\n\nDo you consider they are the same?\n\n");
            text += tr("Patient 1\nID: %1\nName: %2\n\n").arg(patient1->getID()).arg(patient1->getFullName());
            text += tr("Patient 2\nID: %1\nName: %2").arg(patient2->getID()).arg(patient2->getFullName());
            bool userDecision = QMessageBox::question(0, ApplicationNameString, text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
            m_patientsSimilarityUserDecision.insert(hashKey, userDecision);

            return userDecision;
        }
    }

    return false;
}

void ExtensionHandler::generatePatientVolumes(Patient *patient, const QString &defaultSeriesUID)
{
    Q_UNUSED(defaultSeriesUID);
    foreach (Study *study, patient->getStudies())
    {
        // Per cada sèrie, si les seves imatges són multiframe o de mides diferents entre sí aniran en volums separats
        foreach (Series *series, study->getViewableSeries())
        {
            int currentVolumeNumber;
            QMap<int, QList<Image*> > volumesImages;
            foreach (Image *image, series->getImages())
            {
                currentVolumeNumber = image->getVolumeNumberInSeries();
                if (volumesImages.contains(currentVolumeNumber))
                {
                    volumesImages[currentVolumeNumber] << image;
                }
                else
                {
                    QList<Image*> newImageList;
                    newImageList << image;
                    volumesImages.insert(currentVolumeNumber, newImageList);
                }
            }
            typedef QList<Image*> ImageListType;
            foreach (ImageListType imageList, volumesImages)
            {
                int numberOfPhases = 1;
                bool found = false;
                int i = 0;
                while (!found && i<imageList.count() - 1)
                {
                    if (imageList.at(i + 1)->getPhaseNumber() > imageList.at(i)->getPhaseNumber())
                    {
                        numberOfPhases++;
                    }
                    else
                    {
                        found = true;
                    }
                    i++;
                }
                int numberOfSlicesPerPhase = imageList.count() / numberOfPhases;

                Volume *volume = new Volume;
                volume->setImages(imageList);
                volume->setNumberOfPhases(numberOfPhases);
                volume->setNumberOfSlicesPerPhase(numberOfSlicesPerPhase);
                volume->setThumbnail(imageList.at(imageList.count() / 2)->getThumbnail(true));
                series->addVolume(volume);
            }
        }
    }
    DEBUG_LOG(QString("Patient:\n%1").arg(patient->toString()));
}

QApplicationMainWindow* ExtensionHandler::addPatientToWindow(Patient *patient, bool canReplaceActualPatient, bool loadOnly)
{
    QApplicationMainWindow *usedMainApp = NULL;

    if (canReplaceActualPatient && !loadOnly)
    {
        m_mainApp->setPatient(patient);
        usedMainApp = m_mainApp;
    }
    else
    {
        QApplicationMainWindow *mainApp;
        bool found = false;

        QListIterator<QApplicationMainWindow*> mainAppsIterator(QApplicationMainWindow::getQApplicationMainWindows());
        while (!found && mainAppsIterator.hasNext())
        {
            mainApp = mainAppsIterator.next();
            found = askForPatientsSimilarity(mainApp->getCurrentPatient(), patient);
        }

        if (found)
        {
            mainApp->connectPatientVolumesToNotifier(patient);
            *(mainApp->getCurrentPatient()) += *patient;
            DEBUG_LOG("Ja teníem dades d'aquest pacient. Fusionem informació");

            // Mirem si hi ha alguna extensió oberta, sinó obrim la de per defecte
            if (mainApp->getExtensionWorkspace()->count() == 0)
            {
                openDefaultExtension();
            }

            if (!loadOnly)
            {
                // Hem fet un "view", per tant cal reinicialitzar les extensions que ho requereixin
                QMap<QWidget *, QString> extensions = mainApp->getExtensionWorkspace()->getActiveExtensions();
                QMapIterator<QWidget *, QString> iterator(extensions);
                while (iterator.hasNext())
                {
                    iterator.next();
                    ExtensionMediator *mediator = ExtensionMediatorFactory::instance()->create(iterator.value());
                    mediator->reinitializeExtension(iterator.key());
                }
            }

            usedMainApp = mainApp;
        }
        else
        {
            if (!loadOnly)
            {
                usedMainApp = m_mainApp->setPatientInNewWindow(patient);
                DEBUG_LOG("Tenim pacient i no ens deixen substituir-lo. L'obrim en una finestra nova.");
            }
            else
            {
                // \TODO Tenint en compte els problemes explicats al tiquet #1087, eliminar els objectes aquí pot fer petar l'aplicació en cas de demanar estudis
                //       sense passar explícitament per la QueryScreen. Donat que en aquest punt encara no s'ha carregat cap volum a memòria,
                //       per tant no hi haurà fugues importants de memòria si no ho eliminem, obtem per no esborrar cap objecte i així minimitzem el problema.
            }
        }
    }

    return usedMainApp;
}

void ExtensionHandler::openDefaultExtension()
{
    if (m_mainApp->getCurrentPatient())
    {
        Settings settings;
        QString defaultExtension = settings.getValue(InterfaceSettings::DefaultExtension).toString();
        if (!request(defaultExtension))
        {
            WARN_LOG("Ha fallat la petició per la default extension anomenada: " + defaultExtension + ". Engeguem extensió 2D per defecte(hardcoded)");
            DEBUG_LOG("Ha fallat la petició per la default extension anomenada: " + defaultExtension + ". Engeguem extensió 2D per defecte(hardcoded)");
            request("Q2DViewerExtension");
        }
    }
    else
    {
        DEBUG_LOG("No hi ha dades de pacient!");
    }
}

};  // end namespace udg
