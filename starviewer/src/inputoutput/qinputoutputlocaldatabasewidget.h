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

#include <QMenu>

// fordward declarations
class QString;

namespace udg {

// fordward declarations
class Patient;
class StatsWatcher;
class QCreateDicomdir;
class Study;

/** 
 * Widget en el que controla les operacions d'entrada/sortida de la base de dades local
 */
class QInputOutputLocalDatabaseWidget : public QWidget, private Ui::QInputOutputLocalDatabaseWidgetBase
{
Q_OBJECT

public:
    
    QInputOutputLocalDatabaseWidget(QWidget *parent = 0);
    ~QInputOutputLocalDatabaseWidget();

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

public slots:
    ///Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    void view(QStringList selectedStudiesInstanceUID, QString selectedSeriesInstanceUID);

    ///Afegeix l'estudi amb l'Study Instance UID passat per paràmetre al Widget
    void addStudyToQStudyTreeWidget(QString studyInstanceUID);

    ///Treu l'estudi amb l'Study Instance UID passat per paràmetre del QStudyTreeWidget
    void removeStudyFromQStudyTreeWidget(QString studyInstanceUID);

signals:
    ///Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    ///Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    void viewPatients(QList<Patient*>);

    /**Signal indicant que s'han d'enviar el PACS els objectes que compleixin la màscara.
      *El primer paràmetre indica l'estudi al que pertany l'objecte a guardar, i en segon lloc la màscara per poder especificar si passar tot l'estudi o una
      *sèrie de l'estudi, o només una imatge*/
    /*TODO: S'hauria de passar un objecte study que contingués només les imatges a guardar del PACS, però degut a que Operation que és l'objecte que es passa
        a QExecuteOperationThread per indicar l'operació de guardar objectes el PACS, no se li pot especificar una llista d'objectes DICOM, sinó que se
        li ha d'especificar la màscara dels objectes que s'han de guardar, enviem las màscara*/
    void storeDicomObjectsToPacs(Study *study, DicomMask dicomMask);

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

private slots:
    ///Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandSeriesOfStudy(QString seriesInstanceUID);

    ///Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandImagesOfSeries(QString studyIntanceUID, QString seriesInstanceUID);

    ///Mostra al SeriesListWidget la previsualització de la sèrie seleccionada en aquell moment al QStudyTreeWidget
    void setSeriesToSeriesListWidget();

    ///Esborra de la base de dades els estudis seleccionats en el QStudyTreeWidgetView
    void deleteSelectedStudiesLocalDatabase();

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

private:
    QMenu m_contextMenuQStudyTreeWidget;
    QDeleteOldStudiesThread m_qdeleteOldStudiesThread;
    QCreateDicomdir *m_qcreateDicomdir;
    StatsWatcher *m_statsWatcher;
};

};// end namespace udg

#endif
