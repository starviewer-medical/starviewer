/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQINPUTOUTPUTLOCALDATABASEWIDGET_H
#define UDGQINPUTOUTPUTLOCALDATABASEWIDGET_H

#include "ui_qinputoutputlocaldatabasewidgetbase.h"

#include "localdatabasemanager.h"
#include "qdeleteoldstudiesthread.h"
#include "dicommask.h"
#include "pacsdevice.h"

#include <QMenu>

// fordward declarations
class QString;

namespace udg {

// fordward declarations
class Patient;
class StatsWatcher;
class QCreateDicomdir;
class Study;
class QWidgetSelectPacsToStoreDicomImage;
class PACSJob;
class SendDICOMFilesToPACSJob;
class PacsManager;

/** 
 * Widget en el que controla les operacions d'entrada/sortida de la base de dades local
 */
class QInputOutputLocalDatabaseWidget : public QWidget, private Ui::QInputOutputLocalDatabaseWidgetBase
{
Q_OBJECT

public:
    
    QInputOutputLocalDatabaseWidget(QWidget *parent = 0);
    ~QInputOutputLocalDatabaseWidget();

    ///Especifiquem l'instància de PacsManager utilitza per les operacions amb el PACS
    void setPacsManager(PacsManager *pacsManager);

    ///Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask);

    /**Li passem un punte a la interfície que crea dicomdir, per a que s'hi pugui comunicar-se per preguntar
     * si l'estudi abans de ser esborrat està a llista d'estudis per crear un nou dicomdir, o per indicar-li 
     * que s'ha afegit demanat afegir un nou estudi a llista d'estudis per crear dicomdir
     */
    //TODO s'hauria buscar una manera més elegant de comunicar les dos classes, fer un singletton de QCreateDicomdir ?
    void setQCreateDicomdir(QCreateDicomdir *qcreateDicomdir);

    ///Neteja els resultats que es mostren de la cerca
    void clear();

    ///Envia les imatges passades per paràmetre al PACS especificat
    void sendDICOMFilesToPACS(PacsDevice pacsDevice, QList<Image*> images);

public slots:
    ///Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    /// Afegim un terer paràmetre "loadOnly" que ens indicarà si únicament volem carregar les dades sense necessitat de fer un "view"
    /// Així diferenciem els casos en que volem carregar dades del pacient "en background" (només fusionem dades del pacient i prou, 
    /// sense aplicar canvis visibles sobre la interfície) útil pels casos de carregar estudis previs, per exemple.
    void view(QStringList selectedStudiesInstanceUID, QString selectedSeriesInstanceUID, bool loadOnly = false );

    ///Afegeix l'estudi amb l'Study Instance UID passat per paràmetre al Widget
    void addStudyToQStudyTreeWidget(QString studyInstanceUID);

    ///Treu l'estudi amb l'Study Instance UID passat per paràmetre del QStudyTreeWidget
    void removeStudyFromQStudyTreeWidget(QString studyInstanceUID);

signals:
    ///Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    ///Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    /// Afegim un segon paràmetre per indicar si volem fer un "view" o únicament carregar en background les dades de pacient i prou
    void viewPatients(QList<Patient*> patientsToView, bool onlyLoad);

private:
    ///Crea les connexions entre signals i slots
    void createConnections();

    ///Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    ///Mostrar l'error que s'ha produït amb les operacions a la base de dades
    bool showDatabaseManagerError(LocalDatabaseManager::LastError error, const QString &doingWhat = "");

    ///Esborra els estudis vells
    // TODO Aquesta responsabilitat d'esborrar els estudis vells al iniciar-se l'aplicació s'hauria de 
    // traslladar a un altre lloc, no és responsabilitat d'aquesta inferfície
    void deleteOldStudies();

    ///Retorna totes les imatges d'un pacient
    QList<Image*> getAllImagesFromPatient(Patient *patient);

private slots:
    ///Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandSeriesOfStudy(QString seriesInstanceUID);

    ///Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandImagesOfSeries(QString studyIntanceUID, QString seriesInstanceUID);

    ///Mostra al SeriesListWidget la previsualització de la sèrie seleccionada en aquell moment al QStudyTreeWidget
    void setSeriesToSeriesListWidget();

    ///Esborra de la base de dades els estudis seleccionats en el QStudyTreeWidgetView
    void deleteSelectedItemsFromLocalDatabase();

    ///Slot que es dispara quan ha finalitzat el thread que esborrar els estudis vells, aquest slot comprova que no s'hagi produït cap error esborrant els estudis vells
    void deleteOldStudiesThreadFinished();

    ///Afegeix els estudis seleccionats a la llista d'estudis a convertir a dicomdir
    void addSelectedStudiesToCreateDicomdirList();

    ///Visualitza l'estudi que se li ha fet doble click QSeriesListWidget
    void viewFromQSeriesListWidget();

    ///Visualitza els estudis seleccionats a la QStudyTreeWidget
    void viewFromQStudyTreeWidget();

    ///Fa signal indicant que els estudis seleccionats s'han de guardar al PACS
    void selectedStudiesStoreToPacs();

    ///Guarda la posició de l'splitter quan l'han mogut
    void qSplitterPositionChanged();

    ///Guarda els estudis seleccionats al PACS que l'usuari ha seleccionat
    void sendSelectedStudiesToSelectedPacs();

    ///Slot que s'activa quan un SendDICOMFilesToPACSJob acaba
    void sendDICOMFilesToPACSJobFinished(PACSJob *);

    /*Cada vegada que encuem un nou Job comprovem si és un RetrieveDICOMFileFromPACSJob i si és així connectem amb el Signal StudyFromCacheWillBeDeleted
      per si s'esborren estudis de la caché poder-los treure de la QStudyTreeWidget*/
    void newPACSJobEnqueued(PACSJob *);

private:

    QMenu m_contextMenuQStudyTreeWidget;
    QDeleteOldStudiesThread m_qdeleteOldStudiesThread;
    QCreateDicomdir *m_qcreateDicomdir;
    StatsWatcher *m_statsWatcher;
    QWidgetSelectPacsToStoreDicomImage *m_qwidgetSelectPacsToStoreDicomImage;
    PacsManager *m_pacsManager;
};

};// end namespace udg

#endif
