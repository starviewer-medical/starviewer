/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQINPUTOUTPUTLOCALDATABASEWIDGET_H
#define UDGQINPUTOUTPUTLOCALDATABASEWIDGET_H

#include "ui_qinputoutputlocaldatabasewidgetbase.h"

#include <QMenu>

#include "localdatabasemanager.h"
#include "qdeleteoldstudiesthread.h"

class QString;

namespace udg {

/** Widget en el que controla les operacions d'entrada/sortida d'un dicomdir
*/

class DicomMask;
class Patient;
class StatsWatcher;
class QCreateDicomdir;

class QInputOutputLocalDatabaseWidget : public QWidget, private Ui::QInputOutputLocalDatabaseWidgetBase
{
Q_OBJECT

public:
    /// Constructor de la classe
    QInputOutputLocalDatabaseWidget(QWidget *parent = 0);

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

    ~QInputOutputLocalDatabaseWidget();

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

private:

    QMenu m_contextMenuQStudyTreeWidget;
    QDeleteOldStudiesThread m_qdeleteOldStudiesThread;
    QCreateDicomdir *m_qcreateDicomdir;
    StatsWatcher *m_statsWatcher;

    ///Crea les connexions entre signals i slots
    void createConnections();

    ///Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    ///Carrega l'amplada de les columnes de QStudyTreeView guardada al QSettings
    void setQStudyTreeWidgetColumnsWidth();

    ///Guarda al QSettings l'amplada de les columnes del QStudyTreeView, perquè quan es torni a carregar el widget es motri l'amplada de les columnes igual a quan es va tancar
    void saveQStudyTreeWidgetColumnsWidth();

    ///Carrega l'estat del QSplitter entre el QStudyTreeWidget i el QSeriesListWidget 
    void setQSplitterState();

    ///Guarda l'estat del QSplitter entre el QStudyTreeWidget i el QSeriesListWidget
    void saveQSplitterState();

    ///Mostrar l'error que s'ha produït amb les operacions a la base de dades
    bool showDatabaseManagerError(LocalDatabaseManager::LastError error, const QString &doingWhat = "");

    ///Esborra els estudis vells
    /*TODO: Aquesta responsabilitat d'esborrar els estudis vells al iniciar-se l'aplicació s'hauria de traslladar a un altre lloc, no és responsabilitat
           d'aquesta inferfície */
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

};

};// end namespace udg

#endif
